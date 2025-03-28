//Sensors
#define pirSensor   1
#define hallSensor  2
#define doorSensor  3
#define buzzer      4

//KEYBOARD
#define rowNum      4
#define colNum      4
char keys[rowNum][colNum] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[rowNum] = {9, 8, 7, 6}; // Connect to the row pinouts of the keypad
byte colPins[colNum] = {5, 4, 3, 2}; // Connect to the column pinouts of the keypad


