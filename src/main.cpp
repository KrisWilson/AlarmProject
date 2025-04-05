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
//    HARD RTC (External) DS1302

// Raspberry pi - serwer do zapisywania wideo z kamery, odbiera sygnał z arduino o urochomienie kamery
//    Kamera USB       

#include <util/include.h>

void setup()
{
  lcdSetup(); // inicjalizacja wyświetlacza LCD 16x2 
  wyswietl("Konfiguracja",0);
  setupRTC(); // inicjalizacja RTC
  //setupFromEEPROM();  // TODO: DOKOŃCZ tą funkcję aby poprawnie pobierała dane do konfiguracji
  Serial.begin(115200);

  //  Stowrzenie nowego taska do odliczanania czasu 
  //  xTaskCreatePinnedToCore(
  //                         clockTask, /* Task function. */
  //                         "TaskClock",   /* name of task. */
  //                         10000,     /* Stack size of task */
  //                         NULL,      /* parameter of the task */
  //                         1,         /* priority of the task */
  //                         &clockTaskHandle,    /* Task handle to keep track of created task */
  //                         0);        /* pin task to core 0 */ 
}

void loop()
{
  RtcDateTime now = Rtc.GetDateTime();
  // Rozbrojony
  if(currentMenuOption == 0)
  {
    
    // Stan po włączeniu: Rozbrojony
    // wyświetlanie aktualnej daty i czasu
    Serial.print("Aktualna data i godzina: ");
    Serial.print(dateTime(now));

    wyswietl("Rozbrojony", 0);
    wyswietl(dateTime(now), 1);

  } 
}
