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
  lcdSetup(); // inicjalizacja wyświetlacza LCD 16x2 
  wyswietl("Konfiguracja",0)
  Serial.begin(115200);

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
