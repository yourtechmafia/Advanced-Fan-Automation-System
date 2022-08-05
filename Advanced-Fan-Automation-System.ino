#include <EEPROM.h> //  EEPROM library
#include <LiquidCrystal.h>  //  LCD library
LiquidCrystal lcd(0, 1, 2, 3, 4, 5);  //  LCD board-pin arrangement
#include "ACS712.h" //  ACS712 library
ACS712  ACS(A1, 5.0, 1023, 185);  //  ACS712 module initialization (Pin A1, 20A with 185mA sensitivity)
#define button_zero 13 //  Device button for speed 0
#define button_one 12 //  Device button for speed 1
#define button_two 11 //  Device button for speed 2
#define button_three 10 //  Device button for speed 3
#define ac_voltage A0 //  AC voltage sensor
#define tempSensor A5 //  Temperate sensor
#define relayOne A2 //  Control relay for speed 1
#define relayTwo A3 //  Control relay for speed 2
#define relayThree A4 //  Control relay for speed 3

int temp, current, voltage; //  Variables to hold the temperature, current and voltage
boolean autoModeBool = 0; //  Boolean to set the device to run in automatic speed mode, defaults to 0 (manual mode)
const int maxVolt = 260;  //  Device voltage limit to warn user of overvoltage
const int8_t auto_temp_range[] = {35, 40};  //  Temperature range for automatic mode
const int8_t buttonPins[] = {button_zero, button_one, button_two, button_three};  //  Buttons pin on board
const int8_t relayPins[] = {relayOne, relayTwo, relayThree};  //  Relay pins on board
int8_t zero_counter, one_counter, two_counter, three_counter, four_counter; //  count number of times individual button is pressed

void setup() {
  ACS.autoMidPoint(); //  Initialize and configure ACS712
  lcd.begin(16, 2); lcd.clear();  //  Initialize and clear LCD
  //  Board input and output configurations
  for (int8_t i = 0; i < 4; i++) pinMode(buttonPins[i], INPUT);
  for (int8_t i = 0; i < 3; i++) pinMode(relayPins[i], OUTPUT);
  updateData();
  if (voltage >= maxVolt) { //  If there is an overvoltage,
    fanSpeed(0);  //  Set fan speed to zero, turns off all relays
    lcd.setCursor(0, 0);  lcd.print("(V) ");  //  indicate V on screen
  } else {
    switch (EEPROM.read(0)) { //  Load the speed state the fan was from EEPROM address 0
      case 0: //  last state of fan was speed 0
        fanSpeed(0);  //  Sets the fan speed to 0
        break;
      case 1:
        fanSpeed(1);
        lcd.setCursor(0, 1);  lcd.print(" M+"); //  Display "M+" to indicate current fan state was restored from memory
        break;
      case 2:
        fanSpeed(2);
        lcd.setCursor(0, 1);  lcd.print(" M+");
        break;
      case 3:
        fanSpeed(3);
        lcd.setCursor(0, 1);  lcd.print(" M+");
        break;
    }
    lcd.setCursor(0, 0);  lcd.print("( ) ");
  }
  temp = random(31, 36);  //  You can get the value for temperature here
}

void loop() {
  updateData();  //  Continuously update LCD screen with data
  if (voltage >= maxVolt) { //  Continuously checks if their ain't overvoltage. If there is:
    one_counter = two_counter = three_counter = four_counter = 0; //  Reset all button speed counters,
    autoModeBool = 0; //  Fan cannot run in automatic mode also,
    fanSpeed(0);  //  Fan speed must also be at 0. All relays off,
    EEPROM.update(0, 0);  //  Update the last memory variable to 0
    lcd.setCursor(0, 0);  lcd.print("(V) ");  //   Displays '(V)' on the screen to indicate overvoltage
    lcd.setCursor(0, 1);  lcd.print("   ");
  } else {  //  No overvoltage
    lcd.setCursor(0, 0);  lcd.print("( ) ");
    if (autoModeBool)  autoMode();  //  Fan can run in automatic mode if button 0 is pressed twice
    buttons();  //  Continuously checks for input from buttons
  }
}
//  Checks all buttons for input
void buttons() {
  if (digitalRead(button_zero)) { //  Button 0 is pressed
    delay(10);  //  A little delay to strike out debounce
    autoModeBool = 0; //  Fan is not in automatic mode
    zero_counter++; //  Increase the counter for pressing button 0
    one_counter = two_counter = three_counter = four_counter = 0; //  Reset all other speed counter values
    lcd.setCursor(0, 1);
    if (zero_counter == 1) {  //  If its 1st time of pressing button 0, then:
      lcd.print("   ");
      fanSpeed(0);  //  Set fan speed to zero, turns off all relays
      EEPROM.update(0, 0);  //  Update the memory to 0: Fan will stay off in event of power interruption
    } else if (zero_counter == 2) {  //  If its the 2nd time of pressing button 0, then:
      zero_counter = 0; //  Resets the 0 counter
      autoModeBool = 1; //  Fan goes into automatic mode
      lcd.print(" A");  lcd.print(char(223)); //  Display 'A' to indicate automatic mode
    }
    while (digitalRead(button_zero)); //  To avoid error from long button press
//    Similar actions for button 0 repeated for button 1
  } else if (digitalRead(button_one)) {
    delay(10);
    autoModeBool = 0;
    one_counter++;
    zero_counter = two_counter = three_counter = four_counter = 0;
    if (EEPROM.read(0) == 1) one_counter = 3; //  If their is an already saved state for button 1
    lcd.setCursor(0, 1);
    if (one_counter == 1) { //  If its the 2nd time of pressing button 1, then:
      lcd.print("   ");
      fanSpeed(1);  //  Set fan speed to 1
      EEPROM.update(0, 0);  //  Reset memory state
    } else if (one_counter == 2) {  //  If its the 2nd time of pressing button 1, then:
      lcd.print(" M+"); //  Displays "M+" to indicate current speed is saved onto memory
      EEPROM.update(0, 1);  //  Store speed 1 into memory for loading after power interruption
    } else if (one_counter == 3) {
      lcd.print("   ");
      EEPROM.update(0, 0);
      one_counter = 1;
    }
    while (digitalRead(button_one));
//`Similar actions for button 1 repeated for button 2
  } else if (digitalRead(button_two)) {
    delay(10);
    autoModeBool = 0;
    two_counter++;
    zero_counter = one_counter = three_counter = four_counter = 0;
    if (EEPROM.read(0) == 2) two_counter = 3;
    lcd.setCursor(0, 1);
    if (two_counter == 1) {
      lcd.print("   ");
      fanSpeed(2);
      EEPROM.update(0, 0);
    } else if (two_counter == 2) {
      lcd.print(" M+");
      EEPROM.update(0, 2);
    } else if (two_counter == 3) {
      lcd.print("   ");
      EEPROM.update(0, 0);
      two_counter = 1;
    }
    while (digitalRead(button_two));
//`Similar actions for buttons 1 & 2 repeated for button 3
  } else if (digitalRead(button_three)) {
    delay(10);
    autoModeBool = 0;
    three_counter++;
    zero_counter = one_counter = two_counter = four_counter = 0;
    if (EEPROM.read(0) == 3) three_counter = 3;
    lcd.setCursor(0, 1);
    if (three_counter == 1) {
      lcd.print("   ");
      fanSpeed(3);
      EEPROM.update(0, 0);
    } else if (three_counter == 2) {
      lcd.print(" M+");
      EEPROM.update(0, 3);
    } else if (three_counter == 3) {
      lcd.print("   ");
      EEPROM.update(0, 0);
      three_counter = 1;
    }
  }
  while (digitalRead(button_three));
}
//  Function to get values from sensors and update LCD accordingly
void updateData() {
  unsigned long start_time = millis();
  while ((millis() - start_time) < 1000) {  //  Runs every 1 second
    current = ACS.mA_AC();  //  Get current reading from ACS
    voltage = (analogRead(ac_voltage) * 500.0 / 1023.0);  //  Evaluates voltage from ac_voltage pin
  }
//  Update the LCD
  lcd.setCursor(3, 1);  lcd.print("   (");
  lcd.setCursor(8, 1);  lcd.print("), " + String(temp) + char(223) + 'C');
  lcd.setCursor(4, 0);  lcd.print(String(voltage) + "V | ");
  if (current < 1000) {
    lcd.setCursor(11, 0);  lcd.print(String(current) + "mA ");  //  Display current as mA
  } else {
    lcd.setCursor(11, 0);  lcd.print(String(current / 1000) + "A   ");  //  Divide current by 1000 and display as Ampere
  }
}
//  Runs when fan is in automatic mode. Switches speed automatically based on temperature value.
void autoMode() {
  if (temp <= auto_temp_range[0])  fanSpeed(1);
  else if (temp > auto_temp_range[0] & temp < auto_temp_range[1])  fanSpeed(2);
  else if (temp >= auto_temp_range[3]) fanSpeed(3);
}
//  Accept an argument (0/1/2/3) to set as fan speed
void fanSpeed(int8_t fanState) {
  switch (fanState) {
    case 0: //  Set fan speed to 0 as received by turning off all relays
      digitalWrite(relayOne, 0);
      digitalWrite(relayTwo, 0);
      digitalWrite(relayThree, 0);
      break;
    case 1: //  Turn on relay for speed 1 and other two speeds off
      digitalWrite(relayTwo, 0);
      digitalWrite(relayThree, 0);
      digitalWrite(relayOne, 1);
      break;
    case 2: //  Turn on relay for speed 2 and other two speeds off
      digitalWrite(relayOne, 0);
      digitalWrite(relayThree, 0);
      digitalWrite(relayTwo, 1);
      break;
    case 3: //  Turn on relay for speed 3 and other two speeds off
      digitalWrite(relayOne, 0);
      digitalWrite(relayTwo, 0);
      digitalWrite(relayThree, 1);
      break;
  }
  lcd.setCursor(7, 1); lcd.print(fanState); //  Indicate current fan speed (state) on LCD
}
