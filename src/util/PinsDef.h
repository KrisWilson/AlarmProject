#include <Arduino.h>


//Sensors
#define pirSensor   13
#define hallSensor  10
#define doorSensor  10
#define buzzer      1
#define ledStatus   14
#define ledWaiting  15

//Keyboard
#define rowNum      4
#define colNum      4
byte rowPins[rowNum] = {9, 8, 7, 6}; // Connect to the row pinouts of the keypad
byte colPins[colNum] = {5, 4, 3, 2}; // Connect to the column pinouts of the keypad
char keys[rowNum][colNum] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};



