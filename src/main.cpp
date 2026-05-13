#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* 
 * PROJECT: The Obsidian Oracle (Interactive Magic 8-Ball)
 * HARDWARE: 
 * - I2C LCD 16x2 (SDA: A4, SCL: A5)
 * - Passive Buzzer (Pin 8)
 * - Push Button (Pin 2, INPUT_PULLUP)
 * - Analog Joystick (VRy: A0)
 * - Sound Sensor (A1)
 */

// Pin Definitions
const int JOYSTICK_Y_PIN = A0;
const int SOUND_SENSOR_PIN = A1;
const int BUTTON_PIN = 4;
const int BUZZER_PIN = 3;
const int SERVO_PIN = 9;

// LCD Settings (Address 0x27 is common; change to 0x3F if necessary)
LiquidCrystal_I2C lcd(0x27, 16, 2);


// State Machine Definitions
enum State {
  IDLE_MENU,       // State 0
  WAKE_UP,         // State 1
  INVOCATION,      // State 2
  SUSPENSE_REVEAL, // State 3
  THE_ANSWER,      // State 4
  RESET_COOLDOWN   // State 5
};

State currentState = IDLE_MENU;

// Personality Modes
enum Personality { CLASSIC, SARCASTIC, CRYPTIC };
Personality currentMode = CLASSIC;
const char* modeNames[] = {"1: Classic", "2: Sarcastic", "3: Cryptic"};

// Timing variables
unsigned long lastInteractionTime = 0;
const unsigned long backlightTimeout = 3000;
unsigned long stateTimer = 0;

// Answer Arrays
const char* classicAnswers[] = {
  "Yes.", "No.", "Ask Again.", "Signs point to yes.", 
  "Very doubtful.", "Most likely.", "Outlook good.", "Never."
};

const char* sarcasticAnswers[] = {
  "Terrible Idea.", "Who Cares?", "Obviously Not.", "In your dreams.", 
  "Google it.", "Meh.", "Why ask me?", "Error 404: Care Not Found."
};

const char* crypticAnswers[] = {
  "The stars align.", "Shadows whisper.", "The void awaits.", 
  "Balance is key.", "Seek the raven.", "A gate opens.", "Watch the moon.",
  "Nevermove", "Tor Illisar' teranol'", "Ash' Belanore", "Hi Ma'am Wency",
  "Nilihity", "Aray mo!!","Allen Kalbo."
};

// Helper Functions
void playWakeUpChime() {
  tone(BUZZER_PIN, 440, 100); delay(150);
  tone(BUZZER_PIN, 554, 100); delay(150);
  tone(BUZZER_PIN, 659, 150); delay(200);
}

void playSuspense() {
  // Rising classical arpeggio (C Major)
  int notes[] = {261, 329, 392, 523, 659, 783, 1046};
  for (int i = 0; i < 7; i++) {
    tone(BUZZER_PIN, notes[i], 150);
    delay(200);
  }
}


void setup() {
  // Pin Modes
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SOUND_SENSOR_PIN, INPUT);
  pinMode(JOYSTICK_Y_PIN, INPUT);
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Obsidian Oracle");
  delay(1500);
  lcd.noBacklight();
  lcd.clear();
}

void loop() {
  switch (currentState) {
    
    case IDLE_MENU: {
      int joyY = analogRead(JOYSTICK_Y_PIN);
      
      // Menu Scrolling (A0)
      // Note: Thresholds (200/800) may need tuning based on specific joystick hardware
      if (joyY < 200 || joyY > 800) {
        lcd.backlight();
        lastInteractionTime = millis();
        
        if (joyY < 200) currentMode = static_cast<Personality>((currentMode + 1) % 3);
        if (joyY > 800) currentMode = static_cast<Personality>((currentMode + 2) % 3);
        
        lcd.clear();
        lcd.print("Set Mode:");
        lcd.setCursor(0, 1);
        lcd.print(modeNames[currentMode]);
        delay(300); // Debounce scroll speed
      }

      // Backlight Timeout
      if (millis() - lastInteractionTime > backlightTimeout) {
        lcd.noBacklight();
      }

      // Transition to WAKE UP
      if (digitalRead(BUTTON_PIN) == LOW) {
        currentState = WAKE_UP;
      }
      break;
    }

    case WAKE_UP:
      lcd.backlight();
      playWakeUpChime();
      lcd.clear();
      lcd.print("SPEAK YOUR");
      lcd.setCursor(0, 1);
      lcd.print("QUESTION...");
      stateTimer = millis();
      currentState = INVOCATION;
      break;

    case INVOCATION: {
      // Sample sound for 3.5 seconds to seed randomness
      long seedAccumulator = 0;
      while (millis() - stateTimer < 3500) {
        seedAccumulator += analogRead(SOUND_SENSOR_PIN);
        delay(10);
      }
      // Combine sound fluctuations with time for high-entropy seed
      randomSeed(seedAccumulator + millis());
      currentState = SUSPENSE_REVEAL;
      break;
    }

    case SUSPENSE_REVEAL:
      lcd.clear();
      playSuspense();
      // Slowly open the mechanical shutter
      currentState = THE_ANSWER;
      break;

    case THE_ANSWER: {
      lcd.clear();
      lcd.print("THE ORACLE SAYS:");
      delay(2000); // Dramatic pause
      lcd.clear();
      
      // Select the answer based on mode
      int r = random(0, 100);
      const char* answerPtr;
      if (currentMode == CLASSIC) answerPtr = classicAnswers[r % 8];
      else if (currentMode == SARCASTIC) answerPtr = sarcasticAnswers[r % 8];
      else answerPtr = crypticAnswers[r % 7];

      String msg = String(answerPtr);
      
      // Smart Row Splitting: If answer is longer than 16 chars, split at a space
      if (msg.length() <= 16) {
        lcd.print(msg);
      } else {
        int splitIndex = msg.lastIndexOf(' ', 16); // Find last space within first row
        if (splitIndex == -1) splitIndex = 16;     // Fallback if no space found
        
        lcd.setCursor(0, 0);
        lcd.print(msg.substring(0, splitIndex));
        lcd.setCursor(0, 1);
        lcd.print(msg.substring(splitIndex + 1)); // Print remainder on second row
      }
      
      stateTimer = millis();
      currentState = RESET_COOLDOWN;
      break;
    }

    case RESET_COOLDOWN:
      // Keep answer on screen for 5 seconds
      if (millis() - stateTimer > 5000) {
        // Close the shutter and reset to IDLE
        lcd.noBacklight();
        lcd.clear();
        currentState = IDLE_MENU;
      }
      break;
  }
}
