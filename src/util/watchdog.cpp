// TO DO
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
// Main logic ->  watchdog.cpp


// Raspberry pi - serwer do zapisywania wideo z kamery, odbiera sygnał z arduino o urochomienie kamery
//    Kamera USB 
#include <Arduino.h>
#include <Keypad.h>
#include <RtcDS1302.h>
#include "util/inc/PinsDef.h"
#include "util/inc/include.h"
bool disarmed = false;  // zmienna do rozbrojenia systemu
#define DEBUG        -1 // DEBUG wszystko wyrzuca na port szeregowy.
#define ROZBROJONY    0 // chilluje i czeka na uzbrojenie (0 -> 2 -> 1)
#define UZBROJONY     1 // agresywnie wyszukuje inputów   (1 -> 5 lub 1 -> 3 -> 5)
#define OPUSCLOKAL    2 // Czas na opuszczenie lokalu     (2 -> 1 lub 2 -> 5)
#define WPISZKOD      3 // Czas na wpisanie kodu          (3 -> 5 lub 3 -> 0)
#define ZABLOKOWANY   4 // ?? Zakładam moment błędnie wpisanego kodu, ale to bez sensu. (4->5)
#define ALARM         10// Alarm sygnalizuje katastrofę   (5 -> 0) 
int armMode = DEBUG;// aktualny status watchdog'a      
int test;
TaskHandle_t clockTaskHandle = NULL;
TaskHandle_t inputDelayTaskHandle = NULL;

// Funkcja która pozwala zmieniać wiele elementów jednocześnie przy zmianie stanu
void changeMode(int _new){  // zmiana trybu watchdoga - przypisanie odpowiedniej konfiguracji
  switch(_new){
    case DEBUG:
      armMode = DEBUG;
      light(ledStatus, HIGH);
      light(ledWaiting, HIGH);
      play(buzzerpin, 2, 1, true);
    break;

    case ROZBROJONY:
      armMode = ROZBROJONY;
      light(ledStatus, LOW);  // Brak uzbrojenia
      light(ledWaiting, LOW); // Brak oczekiwania
      play(buzzerpin,0);      // Brak sygnału dźwiękowego
    break;

    case UZBROJONY:
      armMode = UZBROJONY;
      light(ledStatus, HIGH); // Uzbrojony
      light(ledWaiting, LOW); // Nie oczekujący
      play(buzzerpin,0);      // Silent mode
    break;

    case OPUSCLOKAL:
      armMode = OPUSCLOKAL;
      light(ledStatus, HIGH); // Uzbrojony
      light(ledWaiting, LOW); // Nie oczekujacy na kod
      play(buzzerpin,2);      // Sygnał bip bip bip w celu informacji ze uzbraja
    break;

    case WPISZKOD:
      armMode = WPISZKOD;
      light(ledStatus, HIGH); // Uzbrojony
      light(ledWaiting, HIGH);// Czeka na kod
      play(buzzerpin,0);      // Silent mode, aby nie wiedzieli ze mamy alarm
    break;

    case ALARM:
      armMode = ALARM;
      light(ledStatus, HIGH); // Uzbrojony
      light(ledWaiting, HIGH);// Czeka na kod
      play(buzzerpin,1);      // I po ptokach, mamy sygnał dźwiękowy
    break;

    default:                  // Technicznie to stan nieokreślony.
      armMode = ZABLOKOWANY;
      light(ledStatus, HIGH); // Uzbrojony
      light(ledWaiting, HIGH);// Czeka na kod
      play(buzzerpin,1);      // I po ptokach, mamy sygnał dźwiękowy
    break;
  }
  Serial.println(getDate() + " Nowy tryb watchdog: " + (String)_new);  
}

void inputDelay(void *pvParameters) // ???
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

void pinSetup(){  //inicjacja trybu pinów
  pinMode(buzzerpin,  OUTPUT);
  pinMode(ledStatus,  OUTPUT);
  pinMode(ledWaiting, OUTPUT);
  pinMode(doorSensor, INPUT);
  pinMode(pirSensor,  INPUT);
}

void checkState(){
  switch(armMode){
  // -1 Debug Mode
  case DEBUG:
  // wyświetlanie aktualnej daty i czasu
  //  Serial.print("Aktualna data i godzina: ");
  //  Serial.print(getDate());
    wyczyscLCD();
    wyswietl("Debugging", 0);
    wyswietl(getDate(), 1);
    Serial.print(getDate() + "   ");
    //Serial.print((String)analogRead(doorSensor) + " " + (String)analogRead(pirSensor) + "   ");
    Serial.print(readDoor(doorSensor)? "Zamknięte drzwi":"Otwarte drzwi");
    Serial.print("   ");
    Serial.println(readPIR(pirSensor)? "Wykryto Ruch":"Nie wykryto ruchu");

    // Wypisywanie wartości liczbowej z numpada
    //test = readNumericInput(0,99999);
    //wyswietl((String)test, 1);


  break;


  // 0. Rozbrojony          - czujniki nieaktywne, kamera wyłączona
    case ROZBROJONY: // stan Rozbrojony
    // wyświetlanie aktualnej daty i czasu
    //  Serial.print("Aktualna data i godzina: ");
    //  Serial.print(getDate());
    
      wyczyscLCD();
      wyswietl("Rozbrojony", 0);
      wyswietl(getDate(), 1);
      //if(anyKey()){
       // if(readPassword() == password);
     // }  
      // TODO: WPISZ PASSWORD lub RFID w celu uzbrojenia alarmu
      // TODO: Utwórz opcje wchodzenia w menu i konfiguracje ustawień
    break;

  // 1. Okres przejściowy po wpisaniu kodu oraz przed wpisaniem kodu
  //      przykład gdy ktoś przełacza na tryb uzbrojony z rozbrojonego i chce opuścić lokal
  //      albo gdy ktoś otwiera drzwi i wchodzi do lokalu podczas uzbrojonego stanu
    case OPUSCLOKAL: 
      wyczyscLCD();
      wyswietl("Uzbrajanie...");
      sleep(getExitTime()); // __SECONDS????????
      changeMode(UZBROJONY);
    break;

    case WPISZKOD:
      wyczyscLCD();
      wyswietl("Uga buga...");
      // TODO: WPISZ PASSWORD lub RFID w celu dezaktywowania alarmu
      changeMode(ALARM);
    break;

  // 2. Uzbrojony           - czujnik krańcowy i ruchu aktywne
    case UZBROJONY:
      if(readPIR(pirSensor))   changeMode(ALARM);     // wykrycie ruchu, bez otwarcia drzwi = instant ban
      if(readDoor(doorSensor)) changeMode(WPISZKOD);  // Wykrycie otwarcia drzwi = daje czas na wpisanie kodu   

      Serial.print("Uzbrojony");
      Serial.print(getDate());

      wyczyscLCD();
      wyswietl("Uzbrojony", 0);
      wyswietl(getDate(), 1);
    break;


  // 3. Alarm aktywny       - kamera, sygnał dźwiękowy i świetlny włącza się po wykryciu ruchu
    case ALARM:
      // TODO: w sumie to oczekuj na input dezaktywacji alarmu
    break;



    default:
      Serial.print("Something went unexpected wrong >:(");
      wyczyscLCD();
      wyswietl("Error: unknown status", 0);
      wyswietl(getDate(), 1);
      
    } 
}

// void ArmedSystem()
// {
//   // Wyświetlenie odliczania zadanego przez użytkownika
//   int i = 0;
//   while (i < getExitTime())
//   {
//     Serial.print(i);
//     delay(1000);
//     i++;
//   }
//   // beep beep system uzbrojony
//   if (readPIR(pirSensor)) // wadą tego rozwiązania jest to, że jak nie wykryje ruchu to nie można rozbroić systemu
//   {
//     //Start odliczania 30s na wpisanie hasła w innym wątku 
//     xTaskCreatePinnedToCore(
//                           inputDelay, /* Task function. */
//                           "TaskClock",   /* name of task. */
//                           10000,     /* Stack size of task */
//                           NULL,      /* parameter of the task */
//                           1,         /* priority of the task */
//                           &inputDelayTaskHandle,    /* Task handle to keep track of created task */
//                           0);  
                          
// int passwordAttempts = 0;
//   passwordInput:
//     Serial.println("Podaj hasło: ");
//     String password = readPassword();
//     if (password != passwordFromMemory && passwordAttempts < 3)
//     {
//       Serial.println("Hasło jest niepoprawne!");
//       passwordAttempts++;
//       goto passwordInput;
//     }
//     else if (password != passwordFromMemory && passwordAttempts >= 3)
//     {
//       Serial.println("Podano 3 błędne hasła! Uruchominie alarmu");
//       // ALARM!!!!!!!!!!!!!!!!!!!!!!! WOŁAJTA POLICJE ZŁODZIEJE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//     }
//     else
//     {
//       disarmed = true;
//       Serial.println("System rozbrojony!");
//       // wyłączenie alarmu
//       // wyłączenie kamery
//       // wyłączenie syreny
//       // wyłączenie diod
//       vTaskDelete(inputDelayTaskHandle); // usunięcie taska odliczania czasu na wpisanie hasła
//     }
//   }
// }



void watchdogSetup(){
  
  lcdSetup(); // inicjalizacja wyświetlacza LCD 16x2 
  wyswietl("Konfiguracja"); // LCD test
  pinSetup(); // inicjalizacja pinów (ustawienie ich trybów)
  setupRTC(); // inicjalizacja RTC (defualtowy kod z dokumentacji)
  
  wyswietl(getDate(), 1); // wyświetl odczytaną datę z RTC
  changeMode(armMode);    // ustaw tryb watchdoga na początkowy

  Serial.println("Aktualna data:   " + getDate());
  Serial.println("Data kompilacji: " + (String)__DATE__ + " " + (String)__TIME__); 
  Serial.println("Inicjalizacja systemu zakończona");
 }



bool watchdog(){
  bool watchdogAlive = true;

  while (watchdogAlive){
    checkState();
  }

  return false;
}
