#include <Arduino.h>
#include <PinsDef.h>

void enterMenuOption(int option)
{
  switch (option)
  {
  case 2:
    // np. zmiana daty i godziny
    break;
    // case 3...
  }
}

void ShowMenuOption(int option)
{
  switch (option)
  {
  case 0:
    // wyświetlenie daty i godziny
    Serial.println("Data i godzina: ");
    break;
  case 1:
    // zmiana hasła
    Serial.println("Zmiana hasła: ");
    break;
  case 2:
    // zmiana daty i godziny
    Serial.println("Zmiana daty i godziny: ");
    break;
  case 3:
    // zmiana czasu na wyjście
    Serial.println("Zmiana czasu na wyjście: ");
    break;
  case 4:
    // zmiana czasu podświetlenia ekranu
    Serial.println("Zmiana czasu podświetlenia ekranu: ");
    break;
  default:
    // wyświetlenie daty i godziny
    Serial.println("Data i godzina: ");
    break;
  }
}
