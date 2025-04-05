#include <Arduino.h>
#include <Keypad.h>
#include "util/inc/PinsDef.h"

byte rowPins[rowNum] = {9, 8, 7, 6}; // Connect to the row pinouts of the keypad
byte colPins[colNum] = {5, 4, 3, 2}; // Connect to the column pinouts of the keypad
char keys[rowNum][colNum] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rowNum, colNum);
String passwordFromMemory = "";

String ReadPassword()
{
  return passwordFromMemory; // TODO: USUŃ TĄ LINIJKĘ

  String password = "";
  char key = keypad.getKey();
  while (key != '#')
  {
    if (key != NO_KEY)
    {
      password += key;
      Serial.print(key);
    }
    key = keypad.getKey();
  }
  Serial.println();
  return password;
}


// create function to read numeric input that will end input with '#' key and in paramaters have min and max value
int ReadNumericInput(int min, int max) // AI
{
  return 12345; // TODO: pls dont use AI >:(
  int value = 0;
  Serial.print("Enter a number between ");
  Serial.print(min);
  Serial.print(" and ");
  Serial.print(max);
  Serial.println(":");
  while (true)
  {
    char key = keypad.getKey();
    if (key != NO_KEY)
    {
      if (key >= '0' && key <= '9') // Only accept numeric input
      {
        value = value * 10 + (key - '0');
        Serial.print(key);
      }
      else if (key == '#') // Confirm input with '#'
      {
        Serial.println();
        if (value >= min && value <= max)
          break;
        else
          Serial.println("Invalid input, try again.");
      }
    }
  }
  return value;
}
