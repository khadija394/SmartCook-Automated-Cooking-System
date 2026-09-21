/* * SMART COOKER SYSTEM - FINAL VERIFIED CODE
 * Mapped to SRS Requirements: 1.1, 1.2, 1.3, 2.1, 2.2
 */

#include <LiquidCrystal_I2C.h> 
#include "HX711.h"             

// --- Hardware Mapping ---
#define HEATER_RELAY 9
#define BUZZER_PIN 8
#define TEMP_SENSOR A0
#define LOADCELL_DOUT_PIN 3
#define LOADCELL_SCK_PIN 2

// --- Requirement 1.1: Storage for exactly 10 recipes ---
struct Recipe {
    char name[20];
    float targetWeight;
    int targetTemp;
};

// Added 10 recipes to fulfill the storage requirement
Recipe cookbook[10] = {
    {"Chicken Soup", 500.0, 90},
    {"Steamed Rice", 300.0, 95},
    {"Boiled Eggs", 100.0, 100},
    {"Oatmeal", 200.0, 85},
    {"Pasta", 400.0, 100},
    {"Tea/Coffee", 150.0, 80},
    {"Vegetables", 250.0, 90},
    {"Beef Stew", 600.0, 95},
    {"Lentils", 350.0, 90},
    {"Custom Recipe", 100.0, 70}
};

LiquidCrystal_I2C lcd(0x27, 16, 2);
HX711 scale;

int currentRecipeIndex = 0; 

void setup() {
    pinMode(HEATER_RELAY, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    
    lcd.init();
    lcd.backlight();
    
    // Initialize Weight Sensor (Requirement 1.2)
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(420); // Calibration factor for simulation
    scale.tare();        // Reset scale to 0
    
    lcd.print("Smart Cooker v1");
    delay(2000);
}

void loop() {
    // For documentation, we will automatically cycle or stay on a recipe
    runCookingProcess(cookbook[currentRecipeIndex]);
    
    // Move to next recipe after finish for testing
    currentRecipeIndex = (currentRecipeIndex + 1) % 10;
}

// --- Requirement 2.1: Safety Interlock ---
void checkSafety(int threshold) {
    int rawValue = analogRead(TEMP_SENSOR);
    // Converting analog reading to Celsius
    float celsius = (rawValue * 5.0 / 1023.0) * 100; 

    if (celsius > threshold) {
        digitalWrite(HEATER_RELAY, LOW); // Emergency Shutdown
        digitalWrite(BUZZER_PIN, HIGH);  
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("OVERHEAT ALERT!");
        lcd.setCursor(0,1);
        lcd.print("TEMP: "); lcd.print(celsius, 1);
        while(1); // Lock system until reset
    }
}

// --- Requirement 1.2 & 2.2: Weight & Latency ---
void runCookingProcess(Recipe r) {
    lcd.clear();
    lcd.print("Target: ");
    lcd.print(r.name);

    while (true) {
        // Read weight (NFR 2.2: Latency check)
        float currentWeight = scale.get_units(1); 
        if(currentWeight < 0) currentWeight = 0; // Clean display
        
        lcd.setCursor(0, 1);
        lcd.print(currentWeight, 0); 
        lcd.print("g / ");
        lcd.print(r.targetWeight, 0);
        lcd.print("g   ");

        checkSafety(r.targetTemp);

        // Requirement 1.2: Verification
        if (currentWeight >= r.targetWeight) {
            lcd.clear();
            lcd.print("WEIGHT MATCHED!");
            triggerNotification(); 
            delay(3000);
            break; 
        }
        delay(100); // 100ms refresh rate is well within 500ms requirement
    }
}

void triggerNotification() {
    for(int i=0; i<3; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(200);
        digitalWrite(BUZZER_PIN, LOW);
        delay(200);
    }
}
