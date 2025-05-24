#include <AccelStepper.h>
#include <LiquidCrystal.h>


// Defining stepper motor pins
AccelStepper stepperX(AccelStepper::DRIVER, 3, 2);

// Initializing LCD Screen
LiquidCrystal lcd(0, 1, 8, 11, 12, 13);


// Defining button pins
const int buttonPowerPin = 4;
const int limitSwitchPin = 5;
const int buttonForwardPin = 7;
const int buttonBackwardPin = 6;


// RGB LED Pins
const int redPin = 9;
const int greenPin = 10;


// Variables for StepsPerSecond Equation
double flowRate = 5; // mL/min
// double diameter = 14.9; // Smaller Syringe 
double diameter = 19.2; // Larger Syringe 
double area = (PI * pow(diameter / 2.0, 2));
double stepsPerSecond = (flowRate / 60.0) * 1000.0 / area * 0.5 * 200 * 16;


// Remaining Time
double Volume = .5; // mL
float remainingTime = (Volume / flowRate) * 60.0 * 2;


// Time calculation variables
unsigned long previousMillis = 0; // Store the time for updates
unsigned long interval = 1000; // Update every second



void setRGB(int red, int green) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
}


void setup() {
  // Buttons and Limit Switch Setup
  pinMode(buttonPowerPin, INPUT_PULLUP);
  pinMode(limitSwitchPin, INPUT_PULLUP);
  pinMode(buttonForwardPin, INPUT_PULLUP);
  pinMode(buttonBackwardPin, INPUT_PULLUP);

  // RGB Setup
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  setRGB(255, 125);

  // Stepper Motor Setup
  stepperX.setMaxSpeed(1000);
  stepperX.setSpeed(stepsPerSecond);

  // LCD Screen Setup
  lcd.begin(16, 2);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setting Up...");
}


void loop() {
  unsigned long currentMillis = millis();

  // On/Off Button State with Limit Switch State
  if (digitalRead(buttonPowerPin) == LOW && digitalRead(limitSwitchPin) == HIGH) {
    setRGB(255, 0);
    stepperX.setSpeed(0);
    lcd.setCursor(0, 0);
    lcd.print("Liquid Empty    ");
  }
  else if (digitalRead(buttonPowerPin) == LOW) {
    setRGB(0, 255);
    stepperX.setSpeed(stepsPerSecond);
    stepperX.runSpeed();
    lcd.setCursor(0, 0);
    lcd.print("Flow Rate: ");
    lcd.print(flowRate);
    lcd.print(" mL/min");

    if (remainingTime > 0) {
      // Calculate and update time remaining
      if (currentMillis - previousMillis >= interval) {
        // Update every second
        previousMillis = currentMillis;

        // Decrease time by 1 second
        remainingTime -= 1;

        // Update the LCD with the remaining time (in seconds)
        lcd.setCursor(0, 1);
        lcd.print("Time Left: ");
        lcd.print(remainingTime);
      }
    }
  }
  else {
    setRGB(255, 125);
    lcd.setCursor(0, 0);
    lcd.print("Paused: Waiting ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }

  // Forward and Backwards Button States
  if (digitalRead(buttonBackwardPin) == LOW) {
    stepperX.setSpeed(-900);
  }
  else if (digitalRead(buttonForwardPin) == LOW) {
    stepperX.setSpeed(900);
  }
  else {
      stepperX.setSpeed(0);
  }
  stepperX.runSpeed();
}