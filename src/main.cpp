// TO DO
// rozpocznij nowy wątek z odliczaniem aktualnego czasu
// zamień wszystki printy na wyświetlanie na ekranie wiadomości
// poukładać funkcjie w innym pliku / posprzątać 
// dodać funkcje ALARM()
// sprawdź czy zapisywanie danych do pamięci flash działa

//////////////////////////////////////////////////////////////////////////////////////////////
//                                 Temat projektu                                           //                                                                              //
// System ochrony (alarm) dla domu lub firmy                                                //
// Wykorzystanie czujnika ruchu oraz łącznika (np. drzwiowego) do uaktywnienia sygnału      //
// dźwiękowego, optycznego oraz dodatkowego łącznika (do załączenia kamery). Zasilanie 5V.  //
//////////////////////////////////////////////////////////////////////////////////////////////

//      Lista elementów:
// ESP32 DEVKIT        -- main.cpp
//    Klawiatura       -- keys.cpp
//    LCD HD44780 16x2 --  lcd.cpp
//    Czujnik pir      -- misc.cpp  -- bool readPIR(pin)
//    Czujnik krańcowy -- misc.cpp  -- bool readDoor(pin)
//    Syrena           -- misc.cpp  -- void play(pin, song)
//    Ledy             -- misc.cpp  -- void light()

// Raspberry pi - serwer do zapisywania wideo z kamery, odbiera sygnał z arduino o urochomienie kamery
//    Kamera USB       







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



#include <Arduino.h>
#include <PinsDef.h>
#include <EEPROM.h>

// dodatkowe oposażenie
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// patrz notatke na samej górze:
#include <keys.cpp>
#include <lcd.cpp>
#include <misc.cpp>
#include <menu.cpp>


// 0x001	Password	   1015	Numeric password (max storage)
// 0x3FE	Exit Time	      2	(Seconds, 16-bit integer)
// 0x3FC	Backlight Time	2	(Seconds, 16-bit integer)

#define EEPROM_SIZE 1024              //Rozmiar jest potrzebny do zainicjowania emulacji pamięci na ESP32
#define configExistAddress 0x00       
#define passwordAddress 0x001
#define exitTimeAddress 0x3fe
#define backlightTimeAddress 0x3fc


int currentMenuOption = 0;
// Menu: (wyświetla się jedna linia)
// 0:  12.03.25 12:00
// 1:  Zmiana hasła
// 2:  Zmiana godziny
// 3:  Zmiana czasu na wyjście

int day = 0, month = 0, year = 0, minutes = 0, hour = 0;
int exitTime = 0; // czas na wyjście po zabezpieczeniu
int backlightTime = 0; // czas podświetlenia ekranu
TaskHandle_t clockTaskHandle = NULL;
TaskHandle_t inputDelayTaskHandle = NULL;
bool disarmed = false; // zmienna do rozbrojenia systemu


void inputDelay(void *pvParameters)
{
  int i = 0;
  while(!disarmed){
    delay(1000);
    i++;
    if (i >= 30)
    {
      // alarm
      Serial.println("Alarm!");
      // włączenie alarmu
      // włączenie kamery
      // włączenie syreny
      // włączenie diod
      break;
    }
  }
}

void ArmedSystem()
{
  // Wyświetlenie odliczania zadanego przez użytkownika
  int i = 0;
  while (i < exitTime)
  {
    Serial.print(i);
    delay(1000);
    i++;
  }
  // beep beep system uzbrojony
  if (readPIR()) // wadą tego rozwiązania jest to, że jak nie wykryje ruchu to nie można rozbroić systemu
  {
    //Start odliczania 30s na wpisanie hasła w innym wątku 
    xTaskCreatePinnedToCore(
                          inputDelay, /* Task function. */
                          "TaskClock",   /* name of task. */
                          10000,     /* Stack size of task */
                          NULL,      /* parameter of the task */
                          1,         /* priority of the task */
                          &inputDelayTaskHandle,    /* Task handle to keep track of created task */
                          0);  
                          
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
    else
    {
      disarmed = true;
      Serial.println("System rozbrojony!");
      // wyłączenie alarmu
      // wyłączenie kamery
      // wyłączenie syreny
      // wyłączenie diod
      vTaskDelete(inputDelayTaskHandle); // usunięcie taska odliczania czasu na wpisanie hasła
    }
  }
}

void clockTask(void *pvParameters)
{
  // Funkcja do odliczania czasu
  while (true)
  {
    // Odczytanie aktualnej daty i godziny
    Serial.print("Aktualna data i godzina: ");
    Serial.print(day);
    Serial.print(".");
    Serial.print(month);
    Serial.print(".");
    Serial.print(year);
    Serial.print(" ");
    Serial.print(hour);
    Serial.print(":");
    Serial.println(minutes);
    vTaskDelay(1000*60 / portTICK_PERIOD_MS); //chyba to jest 1 minuta ale nie wiem lol
    minutes++;
    if (minutes >= 60)
    {
      minutes = 0;
      hour++;
      if (hour >= 24)
      {
        hour = 0;
        day++;
        if (day > 31)
        {
          day = 1;
          month++;
          if (month > 12)
          {
            month = 1;
            year++;
          }
        }
      }
    }
  }
}


void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(115200);

  // ONLY FOR TESTING PURPOSES
  // CLEAN MEMORY
      // for(int i = 0; i < EEPROM_SIZE; i++)
      // {
      //   EEPROM.write(i, 0x00);
      // }
      // EEPROM.commit(); // faktyczne zapisanie danych do pamięci flash
  // END TESTING





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

    // Zapisz czas na wyjście po zabezpieczeniu
    Serial.println("Wprowadź czas na wyjście po zabezpieczeniu (s):");
    exitTime = ReadNumericInput(0, 255); // w sekundach
    EEPROM.write(exitTimeAddress, exitTime);
    // Zapisz czas podświetlenia ekranu
    Serial.println("Wprowadź czas podświetlenia ekranu (s):");
    int backlightTime = ReadNumericInput(0, 255); // w sekundach
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
    // Jeśli wpisane hasło jest poprawne to podaj date i aktualny czas

    Serial.println("Podaj dzień:");
    day = ReadNumericInput(1, 31);
    Serial.println("Podaj miesiąc:");
    month = ReadNumericInput(1, 12);
    Serial.println("Podaj rok:");
    year = ReadNumericInput(0, 99);
    Serial.println("Podaj godzinę:");
    hour = ReadNumericInput(0, 23);
    Serial.println("Podaj minutę:");
    minutes = ReadNumericInput(0, 59);

    //Stowrzenie nowego taska do odliczanania czasu 
    xTaskCreatePinnedToCore(
                            clockTask, /* Task function. */
                            "TaskClock",   /* name of task. */
                            10000,     /* Stack size of task */
                            NULL,      /* parameter of the task */
                            1,         /* priority of the task */
                            &clockTaskHandle,    /* Task handle to keep track of created task */
                            0);        /* pin task to core 0 */
    
  }
}

void loop()
{
  // Stan po włączeniu: Rozbrojony
  // wyświetlanie aktualnej daty i czasu
  // niżej wyświetlanie menu
  // możliwośc uzbrojenia systemu ...
  if(currentMenuOption == 0)
  {
    Serial.print("Aktualna data i godzina: ");
    Serial.print(day);
    Serial.print(".");
    Serial.print(month);
    Serial.print(".");
    Serial.print(year);
    Serial.print(" ");
    Serial.print(hour);
    Serial.print(":");
    Serial.println(minutes);
  }
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
