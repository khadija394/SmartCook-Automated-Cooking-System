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

// Relay logic used by the simulation
#define HEATER_ON HIGH
#define HEATER_OFF LOW

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
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Initialize HX711 load cell
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Calibration factor for Wokwi simulation
  scale.set_scale(420);

  // Reset weight reading to zero
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

  // Run the currently selected recipe
  runCookingProcess(cookbook[currentRecipeIndex]);

  // Move to the next recipe after the current test finishes
  currentRecipeIndex++;

  if (currentRecipeIndex >= 10) {
    currentRecipeIndex = 0;
  }

  // Short pause before starting the next recipe
  delay(1000);
}

// ============================================================
// TEMPERATURE SAFETY MONITORING
// ============================================================

float readTemperature() {

  int rawValue = analogRead(TEMP_SENSOR);

  // Temperature conversion used for the simulation
  float celsius = (rawValue * 5.0 / 1023.0) * 100.0;

  return celsius;
}

bool checkSafety(int temperatureLimit) {

  float temperature = readTemperature();

  if (temperature > temperatureLimit) {

    // Immediately stop heating
    digitalWrite(HEATER_RELAY, HEATER_OFF);

    // Activate warning buzzer
    digitalWrite(BUZZER_PIN, HIGH);

    // Display safety warning
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("OVERHEAT ALERT!");

    lcd.setCursor(0, 1);
    lcd.print("TEMP:");
    lcd.print(temperature, 1);
    lcd.print("C");

    // Keep system stopped until reset
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

  // Display recipe information
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(recipe.name);

  delay(1500);

  // Turn heater ON for the cooking process
  digitalWrite(HEATER_RELAY, HEATER_ON);

  while (true) {

    // --------------------------------------------------------
    // Read current ingredient weight
    // --------------------------------------------------------

    float currentWeight = scale.get_units(1);

    // Prevent negative values from appearing on the LCD
    if (currentWeight < 0) {
      currentWeight = 0;
    }

    // --------------------------------------------------------
    // Display weight information
    // --------------------------------------------------------

    lcd.setCursor(0, 0);
    lcd.print("Weight:         ");

    lcd.setCursor(0, 1);
    lcd.print(currentWeight, 0);
    lcd.print("g/");
    lcd.print(recipe.targetWeight, 0);
    lcd.print("g ");

    // --------------------------------------------------------
    // Check temperature safety
    // --------------------------------------------------------

    checkSafety(recipe.targetTemp);

    // --------------------------------------------------------
    // Check whether target weight has been reached
    // --------------------------------------------------------

    if (currentWeight >= recipe.targetWeight) {

      // Stop heater
      digitalWrite(HEATER_RELAY, HEATER_OFF);

      // Show completion message
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

    // Refresh sensor/display every 100 ms
    delay(100);
  }
}

// ============================================================
// BUZZER NOTIFICATION
// ============================================================

void triggerNotification() {

  for (int i = 0; i < 3; i++) {

    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);

    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}
