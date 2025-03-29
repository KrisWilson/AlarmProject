// TO DO
//  usuń linijkę z zapisywaniem czasu do pamięci w konfiguracji 
// przy każdorazowym włączeniu podawaj aktualną datę i godzinę 
// rozpocznij nowy wątek z odliczaniem aktualnego czasu 

#include <Arduino.h>
#include <PinsDef.h>
#include <EEPROM.h>
#include <Keypad.h>

// 0x001	Password	   1015	Numeric password (max storage)
// 0x3F8	Date	          3	(YY MM DD) - 1 byte each
// 0x3FB	Time	          3	(HH MM SS) - 1 byte each
// 0x3FE	Exit Time	      2	(Seconds, 16-bit integer)
// 0x3FC	Backlight Time	2	(Seconds, 16-bit integer)

#define EEPROM_SIZE 1024
#define configExistAddress 0x00
#define passwordAddress 0x001
#define dateAddress 0x3f8
#define timeAddress 0x3fB
#define exitTimeAddress 0x3fe
#define backlightTimeAddress 0x3fc

int currentMenuOption = 0;
// Menu: (wyświetla się jedna linia)
// 0:  12.03.25 12:00
// 1:  Zmiana hasła
// 2:  Zmiana godziny
// 3:  Zmiana czasu na wyjście
//

String passwordFromMemory = "";
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rowNum, colNum);

//////////////////////////////////////////////////////////////////////////////////////////////
//                                 Temat projektu                                           //                                                                              //
// System ochrony (alarm) dla domu lub firmy                                                //
// Wykorzystanie czujnika ruchu oraz łącznika (np. drzwiowego) do uaktywnienia sygnału      //
// dźwiękowego, optycznego oraz dodatkowego łącznika (do załączenia kamery). Zasilanie 5V.  //
//////////////////////////////////////////////////////////////////////////////////////////////

// Lista elementów
// Arduino / esp32 - kontroler całego systemu, wyświetla menu, odbiera sygnały z czujników
// Klawiatura
// Czujnik pir
// Czujnik krańcowy
// Syrena
// Ledy
// Raspberry pi - serwer do zapisywania wideo z kamery, odbiera sygnał z arduino o urochomienie kamery
// Kamera USB

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

// Tryby pracy
// 1. Niezainicjalizowany - brak konfiguracji.
// 2. Rozbrojony          - czujniki nieaktywne, kamera wyłączona
// 3. Wstępnie uzbrojony  - czujki nieaktywne, odliczanie zadanego czasu do uzbrojenia, możliwość wpisania hasła aby rozbroić?
// 4. Uzbrojony           - czujnik krańcowy i ruchu aktywne
// 5. Alarm aktywny       - kamera, sygnał dźwiękowy i świetlny włącza się po wykryciu ruchu

void enterMenuOption(int option)
{
  switch (option)
  {
  case 2:
    // Kod do zmiany daty i godziny
    break;
    //...
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

bool DetectMovement()
{
  while (true)
  {
    // Czujnik ruchu
    if (digitalRead(pirSensor) == HIGH)
    {
      return true;
    }
    // Czujnik krańcowy
    if (digitalRead(doorSensor) == HIGH)
    {
      return true;
    }
  }
}

void ArmedSystem()
{
  // Wyświetlenie odliczania zadanego przez użytkownika
  int i = 0;
  while (i < 30)
  {
    Serial.print(i);
    delay(1000);
    i++;
  }
  // beep beep system uzbrojony
  if (DetectMovement()) // wadą tego rozwiązania jest to, że jak nie wykryje ruchu to nie można rozbroić systemu
  {
    // Oliczanie 30 sekund na wpisanie hasła
    // jak to zrobić jeśli nie masz multithreadingu? 
    int passwordAttempts = 0;
  passwordInput:
    Serial.println("Podaj hasło: ");
    String password = ReadPassword();
    if (password != passwordFromMemory && passwordAttempts < 3)
    {
      Serial.println("Hasło jest niepoprawne!");
      passwordAttempts++;
      goto passwordInput;
    }
    else if (password != passwordFromMemory && passwordAttempts >= 3)
    {
      Serial.println("Podano 3 błędne hasła! Uruchominie alarmu");
      // ALARM!!!!!!!!!!!!!!!!!!!!!!! WOŁAJTA POLICJE ZŁODZIEJE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
  }
}

String ReadDate() // AI
{
  String date = "";
  Serial.println("Enter date in format YY MM DD:");
  while (date.length() < 8) // Expecting "YY MM DD" (7 characters + 1 space)
  {
    char key = keypad.getKey();
    if (key != NO_KEY)
    {
      date += key;
      Serial.print(key);
    }
  }
  Serial.println();
  return date;
}

String ReadTime() // AI
{
  String time = "";
  Serial.println("Enter time in format HH MM SS:");
  while (time.length() < 8) // Expecting "HH MM SS" (7 characters + 1 space)
  {
    char key = keypad.getKey();
    if (key != NO_KEY)
    {
      time += key;
      Serial.print(key);
    }
  }
  Serial.println();
  return time;
}

int ReadExitTime() // AI
{
  int exitTime = 0;
  Serial.println("Enter exit time in seconds:");
  while (true)
  {
    char key = keypad.getKey();
    if (key != NO_KEY)
    {
      if (key >= '0' && key <= '9') // Only accept numeric input
      {
        exitTime = exitTime * 10 + (key - '0');
        Serial.print(key);
      }
      else if (key == '#') // Confirm input with '#'
      {
        Serial.println();
        break;
      }
    }
  }
  return exitTime;
}

int ReadBacklightTime() // AI
{
  int backlightTime = 0;
  Serial.println("Enter backlight time in seconds:");
  while (true)
  {
    char key = keypad.getKey();
    if (key != NO_KEY)
    {
      if (key >= '0' && key <= '9') // Only accept numeric input
      {
        backlightTime = backlightTime * 10 + (key - '0');
        Serial.print(key);
      }
      else if (key == '#') // Confirm input with '#'
      {
        Serial.println();
        break;
      }
    }
  }
  return backlightTime;
}

void setup()
{
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(115200);
  // Używanie wewnętrznej pamięci arduino
  // https://www.digikey.com/en/maker/blogs/2021/how-to-permanently-store-data-on-an-arduinos-built-in-eeprom
  // W Esp nie ma EEPROM za to używan się poamięci flash do zapisywania danych, biblioteka jest ta sama ale lekko inne użycie

  // Sprawdzanie czy istnieje konfiguracja
  // Jeśli tak to podaj hasło
  // Jeśli nie to stwórz 
  bool configExist = (bool)EEPROM.read(configExistAddress); // to trzeba pozmieniać na readBool ta samo jak na write bool
  if (!configExist)
  {
    // Wypisanie tekstu powitalnego
    Serial.println("Witaj w systemie alarmowym HOMESEC XD");
    Serial.println("Konfiguracja systemu...");
  passwordSection:
    // Wprowadź hasło do systemu max długość 1015 znaków
    String password = ReadPassword(); // zastąp ReadPassword() funkcją do odczytu hasła
    if (password.length() > 1015)
    {
      Serial.println("Hasło jest za długie!");
      goto passwordSection;
    }
    passwordFromMemory = password;
    for (int i = 0; i < password.length(); i++)
    {
      byte byteAtCurrentStringPosition = (byte)password[i];
      EEPROM.write(passwordAddress + i, byteAtCurrentStringPosition);
    }
    // Zapisz date i czas
    Serial.println("Wprowadź date (YY MM DD):");
    String date = ReadDate(); // zastąp ReadDate() funkcją do odczytu daty
    for (int i = 0; i < date.length(); i++)
    {
      byte byteAtCurrentStringPosition = (byte)date[i];
      EEPROM.write(dateAddress + i, byteAtCurrentStringPosition);
    }
    Serial.println("Wprowadź czas (HH MM SS):");
    String time = ReadTime();
    for (int i = 0; i < time.length(); i++)
    {
      byte byteAtCurrentStringPosition = (byte)time[i];
      EEPROM.write(timeAddress + i, byteAtCurrentStringPosition);
    }
    // Zapisz czas na wyjście po zabezpieczeniu
    Serial.println("Wprowadź czas na wyjście po zabezpieczeniu (s):");
    int exitTime = ReadExitTime();
    EEPROM.write(exitTimeAddress, exitTime);
    // Zapisz czas podświetlenia ekranu
    Serial.println("Wprowadź czas podświetlenia ekranu (s):");
    int backlightTime = ReadBacklightTime();
    EEPROM.write(backlightTimeAddress, backlightTime);

    // Zapisz w pierwszym bajcie pamięci że konfiguracja istnieje
    EEPROM.write(configExistAddress, 0x01); // sprawdź czy to działa

    EEPROM.commit(); // faktyczne zapisanie danych do pamięci flash
  }
  else
  {
    // Sprawdź czy hasło jest poprawne
    // Odczytaj hasło z pamięci
    for (int i = 0; i < 1015; i++)
    {
      byte byteAtCurrentStringPosition = EEPROM.read(passwordAddress + i);
      if (byteAtCurrentStringPosition == 0)
        break; // koniec hasła
      passwordFromMemory += (char)byteAtCurrentStringPosition;
    }
    // Wpisanie hasła i porównanie z hasłem z pamięci
    int passwordAttempts = 0;
  passwordInput:
    Serial.println("Podaj hasło: ");
    String password = ReadPassword();
    if (password != passwordFromMemory && passwordAttempts < 3)
    {
      Serial.println("Hasło jest niepoprawne!");
      passwordAttempts++;
      goto passwordInput;
    }
    else if (password != passwordFromMemory && passwordAttempts >= 3)
    {
      Serial.println("Za dużo prób!");
      // ALARM ???
    }
  }
}

void loop()
{
  // Stan po włączeniu: Rozbrojony
  // czy da się w ogóle wyświetlać czas jeśli nie ma multithreadingu?
  // wyświetlanie aktualnej daty i czasu
  // niżej wyświetlanie menu
  // możliwośc uzbrojenia systemu ...
  char key = keypad.getKey();
  switch (key)
  {
  case '#': // wprowadzenie hasła i uzbrojenie systemu
  {
    int passwordAttempts = 0;
    bool correctPasswordInput = false;
    while (!correctPasswordInput)
    {
      Serial.println("Podaj hasło: ");
      String password = ReadPassword();
      if (password != passwordFromMemory && passwordAttempts < 3)
      {
        Serial.println("Hasło jest niepoprawne!");
        passwordAttempts++;
      }
      else if (password != passwordFromMemory && passwordAttempts >= 3)
      {
        Serial.println("Za dużo prób!");
        // ALARM ???
      }
      else
      {
        correctPasswordInput = true;
      }
    }
    // Uzbrojenie systemu
    ArmedSystem();

    break;
  }
  case '1':
  {
    // przejście wyżej w menu
    if (currentMenuOption > 0)
    {
      currentMenuOption--;
    }

    break;
  }
  case '9':
  {
    // przejście niżej w menu
    if (currentMenuOption < 6)
    {
      currentMenuOption++;
    }
    break;
  }
  case '6':
  {
    // wejście do opcji w menu
    enterMenuOption(currentMenuOption);
    break;
  }
  default:

    break;
  }
}
