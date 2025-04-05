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

#include <util/include.h>


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
