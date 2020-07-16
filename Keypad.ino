/*
* Keypad.ino
*
* Created: 13/08/2019 18:51:10
* Author : Leonardo Lisa
*/

// NOTE:
// Useful link
// https://omerk.github.io/lcdchargen/

#include <LiquidCrystal.h>

// PINS
#define BUZZER_PIN 17

#define R_LED_PIN 15
#define G_LED_PIN 16

#define ROW0_PIN 6
#define ROW1_PIN 7
#define ROW2_PIN 8
#define ROW3_PIN 9
#define COLUMN0_PIN 10
#define COLUMN1_PIN 13
#define COLUMN2_PIN 14

#define ROWS 4
#define COLS 3

#define PASSWORD_LENGTH 8

const byte rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// OTHERS
#define DENIED 0
#define ALLOWED 1

// CONSTANTS
const byte row_pins[4] = {ROW0_PIN, ROW1_PIN, ROW2_PIN, ROW3_PIN};
const byte keys[ROWS][COLS] = {
  {1, 2, 3},
  {4, 5, 6},
  {7, 8, 9},
  {'R', 0,'F'}
};
byte padlock_lck[8] = {
  B01110,
  B10001,
  B10001,
  B10001,
  B11111,
  B11011,
  B11011,
  B11111
};
byte padlock_un[8] = {
  B01110,
  B10001,
  B10000,
  B10000,
  B11111,
  B11011,
  B11011,
  B11111
};
const byte password[PASSWORD_LENGTH]= {1, 6, 0, 8, 2, 0, 1, 9};

// VARIABLES
byte row = 0, column = 0, buffer_count = 0, access = 0, timeOUT = 0;
byte buffer_password[PASSWORD_LENGTH]= {};

void setup() {
  // Keypad pin configuration
  pinMode(ROW0_PIN, INPUT);
  digitalWrite(ROW0_PIN, HIGH);
  pinMode(ROW1_PIN, INPUT);
  digitalWrite(ROW1_PIN, HIGH);
  pinMode(ROW2_PIN, INPUT);
  digitalWrite(ROW2_PIN, HIGH);
  pinMode(ROW3_PIN, INPUT);
  digitalWrite(ROW3_PIN, HIGH);
  pinMode(COLUMN0_PIN, OUTPUT);
  digitalWrite(COLUMN0_PIN, LOW);
  pinMode(COLUMN1_PIN, OUTPUT);
  digitalWrite(COLUMN1_PIN, LOW);
  pinMode(COLUMN2_PIN, OUTPUT);
  digitalWrite(COLUMN2_PIN, LOW);
  // Led pin configuration
  pinMode(R_LED_PIN, OUTPUT);
  digitalWrite(R_LED_PIN, HIGH);
  pinMode(G_LED_PIN, OUTPUT);
  digitalWrite(G_LED_PIN, LOW);
  // Buzzer pin configuration
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  // Create a new custom characters
  lcd.createChar(0, padlock_lck);
  lcd.createChar(1, padlock_un);
  // Set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // UI stuff
  lcd.print("TYPE PASSWORD ");
  lcd.write((uint8_t)0);
  lcd.setCursor(0, 1);
  lcd.print(">> ");
}

void loop() {
  loop:
  // The code below perform key capture, password check and UI (UI == User Interface)
  // Detect key press
  if(!(digitalRead(ROW0_PIN) & digitalRead(ROW1_PIN) & digitalRead(ROW2_PIN) & digitalRead(ROW3_PIN))) {
    // Decode key
    // Detect key row
    row = 0*!digitalRead(ROW0_PIN) + 1*!digitalRead(ROW1_PIN) + 2*!digitalRead(ROW2_PIN) + 3*!digitalRead(ROW3_PIN);
    // This implementation inverts rows
    // row = 0*digitalRead(ROW0_PIN) + digitalRead(ROW1_PIN) + 2*digitalRead(ROW2_PIN) + 3*digitalRead(ROW3_PIN) - 3;
    // If multiple keys are pressed row could be grater than 3, the code below prevent this situation
    if(row > 3) {
      row = 0;
      }
    // Detect key column
    detectColumn:
    digitalWrite(COLUMN1_PIN, HIGH);
    digitalWrite(COLUMN2_PIN, HIGH);
    if(!digitalRead(row_pins[row])) {
      column = 0;
    }else{
      digitalWrite(COLUMN0_PIN, HIGH);
      digitalWrite(COLUMN1_PIN, LOW);
      if(!digitalRead(row_pins[row])) {
        column = 1;
        }else{
          digitalWrite(COLUMN1_PIN, HIGH);
          digitalWrite(COLUMN2_PIN, LOW);
          if(!digitalRead(row_pins[row])) {
            column = 2;
            }else{
              digitalWrite(COLUMN0_PIN, LOW);
              digitalWrite(COLUMN1_PIN, LOW);
              // If multiple keys are pressed the code below prevent infinite loop
              timeOUT++;
              if(timeOUT == 50) {
                goto loop;
                }
              goto detectColumn;
              }
          }
    }
    digitalWrite(COLUMN0_PIN, LOW);
    digitalWrite(COLUMN1_PIN, LOW);
    digitalWrite(COLUMN2_PIN, LOW);
    switch (keys[row][column]) {
    case 'R':
    // Buzzer tone
    tone(BUZZER_PIN, 3729, 150);
    if(buffer_count != 0) {
      buffer_count--;
      // UI stuff
      lcd.setCursor(buffer_count + 3, 1);
      lcd.print(" ");
      lcd.setCursor(buffer_count + 3, 1);
      }
      delay(300);
    break;
    case 'F':
    // Buzzer tone
    tone(BUZZER_PIN, 3951, 150);
    // Check password
    for(byte i = 0; buffer_password[i] == password[i]; i++) {
      if(i == PASSWORD_LENGTH - 1) {
        access = ALLOWED;
        break;
        }
      }
    // Clear buffer pointer and password buffer
    buffer_count = 0;
    for(byte c = 0; c != PASSWORD_LENGTH; c++) {
      buffer_password[c] = 0;
      }
    // UI stuff: padlock unlock animation
    lcd.setCursor(14, 0);
    lcd.write((uint8_t)1);
    delay(2000);
    if(access == ALLOWED) {
      // UI stuff
      lcd.clear();
      lcd.print(" ACCESS ALLOWED ");
      // Change led color: red --> greeen
      digitalWrite(R_LED_PIN, LOW);
      digitalWrite(G_LED_PIN, HIGH);
      // Lock again if 'F' is pressed
      while(digitalRead(ROW3_PIN)) {  
        }
      // Buzzer tone
      tone(BUZZER_PIN, 3951, 150);
      access = DENIED;
      // UI stuff: padlock lock animation
      lcd.clear();
      lcd.print("  LOCKING ");
      lcd.write((uint8_t)1);
      lcd.write((uint8_t)1);
      lcd.write((uint8_t)1);
      delay(1200);
      lcd.setCursor(10, 0);
      lcd.write((uint8_t)0);
      // Buzzer tone
      tone(BUZZER_PIN, 3951, 100);
      delay(400);
      lcd.write((uint8_t)0);
      // Buzzer tone
      tone(BUZZER_PIN, 3951, 100);
      delay(400);
      lcd.write((uint8_t)0);
      // Buzzer tone
      tone(BUZZER_PIN, 3951, 100);
      // Change led color: greeen --> red
      digitalWrite(G_LED_PIN, LOW);
      digitalWrite(R_LED_PIN, HIGH);
      delay(600);
      }else{
        // UI stuff
        lcd.clear();
        lcd.print(" ACCESS DENIED! ");
        delay(1500);
        lcd.setCursor(0, 1);
        lcd.print(">> Try again");
        delay(400);
        lcd.print(".");
        delay(400);
        lcd.print(".");
        delay(400);
        lcd.print(".");
        delay(400);
        }
    lcd.clear();
    lcd.print("TYPE PASSWORD ");
    lcd.write((uint8_t)0);
    lcd.setCursor(0, 1);
    lcd.print(">> ");
    break;
    default:
    // Buzzer tone
    tone(BUZZER_PIN, 3520, 150);
    // Fill the password buffer and prevent overflow
    if(buffer_count != PASSWORD_LENGTH) {
      buffer_password[buffer_count] = keys[row][column];
      buffer_count++;
      // UI stuff
      lcd.print("*");
      }
      // Debouncing delay
      delay(300);
    break;
    }
  }
}
