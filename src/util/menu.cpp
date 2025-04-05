#include <EEPROM.h>

// 1. Wyświetl tekst powitalny
// 2. Jeśli konfiguracja (nie)istanieje to
// 3A. Wyświetl Prompta o hasło
//
// 3B. Wyświetl menu konfiguracji
//                1) Wprowadź hasło
//                2) Ustaw date i czas
//                3) Ustaw czas na wyjście po zabezpieczeniu
//                4) Ustaw czas podświetlenia ekranu
//                5) Ustawienia kamery(czy ma działać, jak się z nią połączyć) ???????????????????????????????????????????????????????????
//
// 4. Po zalogowaniu do systemu wyświetl datę i czas oraz możliwośc do wejścia do menu z ustawieniami



// 0x001	Password	   1015	Numeric password (max storage)
// 0x3FE	Exit Time	      2	(Seconds, 16-bit integer)
// 0x3FC	Backlight Time	2	(Seconds, 16-bit integer)



int currentMenuOption = 0;
// Menu: (wyświetla się jedna linia)
// 0:  12.03.25 12:00
// 1:  Zmiana hasła
// 2:  Zmiana godziny
// 3:  Zmiana czasu na wyjście




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