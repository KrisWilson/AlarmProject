#include <Arduino.h>
#include <Keypad.h>
#include "util/inc/PinsDef.h"
                        // 19 18 5 - kolizja z RFID
byte rowPins[rowNum]   = {34, 35, 32, 33};
byte colPins[colNum ]  = {16, 4,  0,  2};

char keys[rowNum][colNum] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
// Funkcjonalność: 1-0 - zwykłe liczby
// * - cancel
// # - confirm
// A B C D

//  1 2 3 A
//  4 5 6 B
//  7 8 9 C
//  * 0 # D
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rowNum, colNum);
String passwordFromMemory = "";

char detectKey(char key = ' '){
  if(key == ' '){
    char c = keypad.getKey();
    if(c != NO_KEY)
      return c;
    else
      return NO_KEY; // NO_KEY == 0
  }
}


// TODO: Ogarnąć cały ten plik
// TODO: funkcja readKey() i readLine() dla LCD
String readPassword() // TODO: zamienić tą funkcję aby porównywałą hasło wpisane z tym z pamięci i wypluwałą BOOLA
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
int readNumericInput(int min, int max) // AI KURWA JEBANE AI
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
          {
            return value;
            break;
          }
        else
          Serial.println("Invalid input, try again.");
      }
    }
  }
  return value;
}
