/*
 * SMART COOKER SYSTEM
 * Arduino Mega 2560
 *
 * Features:
 * - Stores 10 recipes
 * - Monitors ingredient weight using HX711
 * - Displays cooking information on 16x2 I2C LCD
 * - Monitors temperature
 * - Controls heater through relay
 * - Provides buzzer notifications
 * - Automatically shuts down the heater during overheating
 *
 * Designed for Wokwi simulation and semester-project demonstration.
 */

#include <LiquidCrystal_I2C.h>
#include "HX711.h"

// ============================================================
// HARDWARE CONFIGURATION
// ============================================================

#define HEATER_RELAY 9
#define BUZZER_PIN 8
#define TEMP_SENSOR A0

#define LOADCELL_DOUT_PIN 3
#define LOADCELL_SCK_PIN 2

// Relay module uses active-low control in this simulation
#define HEATER_ON LOW
#define HEATER_OFF HIGH

// ============================================================
// RECIPE DATA
// ============================================================

struct Recipe {
  const char* name;
  float targetWeight;
  int targetTemp;
};

// Ten sample recipes for the prototype
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

// ============================================================
// COMPONENT OBJECTS
// ============================================================

LiquidCrystal_I2C lcd(0x27, 16, 2);
HX711 scale;

// Current recipe used during simulation
int currentRecipeIndex = 0;

// ============================================================
// SETUP
// ============================================================

void setup() {

  // Configure output pins
  pinMode(HEATER_RELAY, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Start with heater OFF
  digitalWrite(HEATER_RELAY, HEATER_OFF);
  noTone(BUZZER_PIN);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Initialize HX711 load cell
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Calibration factor for Wokwi simulation
  scale.set_scale(420);

  // Reset scale to zero
  scale.tare();

  // Startup message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Cooker");

  lcd.setCursor(0, 1);
  lcd.print("System Ready");

  delay(2000);
}

// ============================================================
// MAIN PROGRAM LOOP
// ============================================================

void loop() {

  // Run the current recipe
  runCookingProcess(cookbook[currentRecipeIndex]);

  // Move to the next recipe after completion
  currentRecipeIndex++;

  if (currentRecipeIndex >= 10) {
    currentRecipeIndex = 0;
  }

  // Short pause before the next recipe
  delay(1000);
}

// ============================================================
// TEMPERATURE MONITORING
// ============================================================

float readTemperature() {

  int rawValue = analogRead(TEMP_SENSOR);

  // Temperature conversion used for the Wokwi simulation
  float celsius = (rawValue * 5.0 / 1023.0) * 100.0;

  return celsius;
}

// ============================================================
// SAFETY CHECK
// ============================================================

bool checkSafety(int temperatureLimit) {

  float temperature = readTemperature();

  // Check whether temperature is above the recipe limit
  if (temperature > temperatureLimit) {

    // Immediately turn the heater OFF
    digitalWrite(HEATER_RELAY, HEATER_OFF);

    // Activate continuous warning tone
    tone(BUZZER_PIN, 1500);

    // Display safety warning
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("OVERHEAT ALERT!");

    lcd.setCursor(0, 1);
    lcd.print("TEMP:");
    lcd.print(temperature, 1);
    lcd.print("C");

    // Stop the system until reset
    while (true) {
      delay(100);
    }
  }

  return true;
}

// ============================================================
// COOKING PROCESS
// ============================================================

void runCookingProcess(Recipe recipe) {

  // Display recipe name
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(recipe.name);

  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  delay(1500);

  // Turn heater ON
  digitalWrite(HEATER_RELAY, HEATER_ON);

  while (true) {

    // --------------------------------------------------------
    // Read ingredient weight
    // --------------------------------------------------------

    float currentWeight = scale.get_units(1);

    // Prevent negative values
    if (currentWeight < 0) {
      currentWeight = 0;
    }

    // --------------------------------------------------------
    // Display weight
    // --------------------------------------------------------

    lcd.setCursor(0, 0);
    lcd.print("Weight:         ");

    lcd.setCursor(0, 1);
    lcd.print(currentWeight, 0);
    lcd.print("g/");
    lcd.print(recipe.targetWeight, 0);
    lcd.print("g ");

    // --------------------------------------------------------
    // Temperature safety check
    // --------------------------------------------------------

    checkSafety(recipe.targetTemp);

    // --------------------------------------------------------
    // Check target weight
    // --------------------------------------------------------

    if (currentWeight >= recipe.targetWeight) {

      // Turn heater OFF
      digitalWrite(HEATER_RELAY, HEATER_OFF);

      // Display completion message
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("WEIGHT MATCHED!");

      lcd.setCursor(0, 1);
      lcd.print("Cooking Ready");

      // Notify the user
      triggerNotification();

      delay(2000);

      break;
    }

    // Refresh every 100 ms
    delay(100);
  }
}

// ============================================================
// BUZZER NOTIFICATION
// ============================================================

void triggerNotification() {

  for (int i = 0; i < 3; i++) {

    // Short notification tone
    tone(BUZZER_PIN, 1000, 200);

    delay(250);

    // Make sure the tone is stopped
    noTone(BUZZER_PIN);

    delay(150);
  }
}
