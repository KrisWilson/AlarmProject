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
//    HARD RTC (External) config.cpp-- void setupRTC(), zmienna Rtc
//    RFID czytnik     -- misc.cpp  -- void checkRFID()
//    Na sygnał świetlny widocznie wypada mieć jakiś określony sposób....?

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
  switch(armMode){

// 0. Rozbrojony          - czujniki nieaktywne, kamera wyłączona
  case ROZBROJONY: // stan Rozbrojony
    // wyświetlanie aktualnej daty i czasu
    Serial.print("Aktualna data i godzina: ");
    Serial.print(dateTime(now));

    wyswietl("Rozbrojony", 0);
    wyswietl(dateTime(now), 1);
    // TOOD: CHECK INPUT 
  break;

// 1. Okres przejściowy po wpisaniu kodu oraz przed wpisaniem kodu
//      przykład gdy ktoś przełacza na tryb uzbrojony z rozbrojonego i chce opuścić lokal
//      albo gdy ktoś otwiera drzwi i wchodzi do lokalu podczas uzbrojonego stanu
  case OPUSCLOKAL: 
    wyswietl("")
  break;

// 2. Uzbrojony           - czujnik krańcowy i ruchu aktywne
  case UZBROJONY:
    if(readPIR(pirSensor))  currentMenuOption = 3; // wykrycie ruchu, bez otwarcia drzwi = instant ban
    if(readDoor(doorSensor)) currentMenuOption = 1;// Wykrycie otwarcia drzwi = daje czas na wpisanie kodu
  break;


// 3. Alarm aktywny       - kamera, sygnał dźwiękowy i świetlny włącza się po wykryciu ruchu
  case ALARM:
  break;



  default:
    Serial.print("Something went unexpected wrong >:(");

    wyswietl("Error: unknown status", 0);
    wyswietl(dateTime(now), 1);
    
  } 



}
