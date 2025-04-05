#include <Arduino.h>
#include <PinsDef.h>
#include <Keypad.h>


Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rowNum, colNum);
String passwordFromMemory = "";

String ReadPassword()
{
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
