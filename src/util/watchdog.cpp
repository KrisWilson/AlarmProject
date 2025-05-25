//////////////////////////////////////////////////////////////////////////////////////////////
//                                 Temat projektu                                           //                                                                              //
// System ochrony (alarm) dla domu lub firmy                                                //
// Wykorzystanie czujnika ruchu oraz łącznika (np. drzwiowego) do uaktywnienia sygnału      //
// dźwiękowego, optycznego oraz dodatkowego łącznika (do załączenia kamery). Zasilanie 5V.  //
//////////////////////////////////////////////////////////////////////////////////////////////

//      Lista elementów:
// ESP32 DEVKIT        -- main.cpp
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
#include <RtcDS1302.h>
#include "util/inc/PinsDef.h"
#include "util/inc/include.h"

#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

// Learn more about using SPI/I2C or check the pin assigment for your board: https://github.com/OSSLibraries/Arduino_MFRC522v2#pin-layout
MFRC522DriverPinSimple ss_pin(5);

MFRC522DriverSPI driver{ss_pin}; // Create SPI driver
//MFRC522DriverI2C driver{};     // Create I2C driver
MFRC522 mfrc522{driver};         // Create MFRC522 instance

bool disarmed  = false; // zmienna do rozbrojenia systemu
#define DEBUG        -1 // DEBUG wszystko wyrzuca na port szeregowy.
#define ROZBROJONY    0 // chilluje i czeka na uzbrojenie (0 -> 2 -> 1)
#define UZBROJONY     1 // agresywnie wyszukuje inputów   (1 -> 5 lub 1 -> 3 -> 5)
#define OPUSCLOKAL    2 // Czas na opuszczenie lokalu     (2 -> 1 lub 2 -> 5)
#define WPISZKOD      3 // Czas na wpisanie kodu          (3 -> 5 lub 3 -> 0)
#define ZABLOKOWANY   4 // ?? Zakładam moment błędnie wpisanego kodu, ale to bez sensu. (4->5)
#define ALARM        10 // Alarm sygnalizuje katastrofę   (5 -> 0) 
#define KOPERNIKCARD "04 04 4e 62 c5 24 81 " // ID Card - Kopernika
#define BLUEPIN      "e2 5f 9a d4 "          // ID Blue - Pinezka
int armMode = ROZBROJONY; // aktualny status watchdog'a      

// później nadpisana funkcja
void changeMode(int mode);

void pinSetup(){  //inicjacja trybu pinów
  pinMode(buzzerpin,  OUTPUT);
  pinMode(ledStatus,  OUTPUT);
  pinMode(ledWaiting, OUTPUT);
  pinMode(doorSensor, INPUT);
  pinMode(pirSensor,  INPUT);
}

void watchdogSetup(){
  lcdSetup(); // inicjalizacja wyświetlacza LCD 16x2 
  wyswietl("Konfiguracja"); // LCD test

  pinSetup(); // inicjalizacja pinów (ustawienie ich trybów)

  setupRTC(); // inicjalizacja RTC (defualtowy kod z dokumentacji)
  wyswietl(getDate(), 1); // wyświetl odczytaną datę z RTC  


  mfrc522.PCD_Init();     // RFID: Init MFRC522 board.


  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);	// Show details of PCD - MFRC522 Card Reader details.
  Serial.println("Aktualna data:   " + getDate());
  Serial.println("Data kompilacji: " + (String)__DATE__ + " " + (String)__TIME__); 
  Serial.println("Inicjalizacja systemu zakończona");

  changeMode(armMode);    // ustaw tryb watchdoga na początkowy
 }

// Funkcja która pozwala zmieniać wiele elementów jednocześnie przy zmianie stanu
void changeMode(int _new){  // zmiana trybu watchdoga - przypisanie odpowiedniej konfiguracji
  wyczyscLCD();
  lcdBackight();
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
      play(buzzerpin,0,0,false);      // Brak sygnału dźwiękowego
      wyswietl("Rozbrojony", 0);
    break;

    case UZBROJONY:
      armMode = UZBROJONY;
      light(ledStatus, HIGH); // Uzbrojony
      light(ledWaiting, LOW); // Nie oczekujący
      play(buzzerpin,0,0,false);      // Silent mode
      wyswietl("Uzbrojony", 0);
    break;

    case OPUSCLOKAL:
      armMode = OPUSCLOKAL;
      light(ledStatus, HIGH); // Uzbrojony
      light(ledWaiting, LOW); // Nie oczekujacy na kod
      play(buzzerpin,2);      // Sygnał bip bip bip w celu informacji ze uzbraja
      wyswietl("Uzbrajanie...");
    break;

    case WPISZKOD:
      armMode = WPISZKOD;
      light(ledStatus, HIGH); // Uzbrojony
      light(ledWaiting, HIGH);// Czeka na kod
      play(buzzerpin,0);      // Silent mode, aby nie wiedzieli ze mamy alarm
      wyswietl("Oczekiwanie");
      wyswietl("Oczekiwanie",1); 
      break;

    case ALARM:
      armMode = ALARM;
      light(ledStatus, HIGH); // Uzbrojony
      light(ledWaiting, HIGH);// Czeka na kod
      play(buzzerpin,1);      // I po ptokach, mamy sygnał dźwiękowy
      wyswietl("ALARM");
      lcdBackight(false);
    break;

    default:                  // Technicznie to stan nieokreślony.
      armMode = ZABLOKOWANY;
      light(ledStatus, HIGH); // Uzbrojony
      light(ledWaiting, HIGH);// Czeka na kod
      play(buzzerpin,2);      // I po ptokach, mamy sygnał dźwiękowy
    break;
  }
  Serial.println(getDate() + " Nowy tryb watchdog: " + (String)_new);  
}

bool checkValidCard(){
  if (mfrc522.PICC_ReadCardSerial() || mfrc522.PICC_IsNewCardPresent()) {
    String uidString = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if (mfrc522.uid.uidByte[i] < 0x10) {
        uidString += "0"; 
      }
      uidString += String(mfrc522.uid.uidByte[i], HEX) + " ";
    }

    Serial.println(getDate() + " Odczytano: " + uidString);
    if(uidString == KOPERNIKCARD || uidString == BLUEPIN){
      Serial.println(getDate() + " Wykryto zgodność UID RFID");
      return true;
    }
  }
  return false;
}

unsigned int checkTime; // zmienna do debugowania czasu trwania cyklu
void checkState(){
  checkTime = millis();
  char c;
  switch(armMode){
  // -1 Debug Mode
  case DEBUG:
    wyczyscLCD();
    wyswietl("Debugging", 0);
    wyswietl(getDate(), 1);
    Serial.print(getDate() + "   ");
    Serial.print((String)analogRead(doorSensor) + " \t" + (String)analogRead(pirSensor) + " \t");
    Serial.print(readDoor(doorSensor)? "Zamknięte drzwi":"Otwarte drzwi");
    Serial.print("\t");
    Serial.println(readPIR(pirSensor)? "Wykryto Ruch":"Nie wykryto ruchu");
    checkValidCard(); // check valid card - output serial
  break;

  // 0. Rozbrojony          - czujniki nieaktywne, kamera wyłączona
    case ROZBROJONY: // stan Rozbrojony
      wyswietl(getDate(), 1);
      if(checkValidCard()) changeMode(OPUSCLOKAL);
    break;

  // 1. Okres przejściowy po wpisaniu kodu oraz przed wpisaniem kodu
  //      przykład gdy ktoś przełacza na tryb uzbrojony z rozbrojonego i chce opuścić lokal
  //      albo gdy ktoś otwiera drzwi i wchodzi do lokalu podczas uzbrojonego stanu
    case OPUSCLOKAL: 
      // w tym stanie on oznajmia że będzie uzbrajac system i masz X sekund na opuszczenie zasięgu działania PIR i zamkniecie drzwi
     delay(500); 
     for(float i=0;i<getExitTime();i+=0.25){
        delay(250); 
        if(checkValidCard()){ 
          wyswietl("Anulowano Uzbrojenie");
          delay(1000);
          changeMode(ROZBROJONY);
          Serial.print(getDate() + " Anulowano uzbrojenie - czytnik RFID");
          return; 
        };
      }
      changeMode(UZBROJONY);
    break;

    case WPISZKOD:     
    for(float i=0;i<getExitTime();i+=0.25){
      delay(250); 
      if(checkValidCard()){ 
        wyswietl("Anulowano Uzbrojenie");
        delay(1000);
        changeMode(ROZBROJONY);
        Serial.println(getDate() + " Anulowano uzbrojenie - czytnik RFID");
        return;  
      };
    }
      changeMode(ALARM);
    break;

  // 2. Uzbrojony           - czujnik krańcowy i ruchu aktywne
    case UZBROJONY:
    
      Serial.println(readDoor(doorSensor)? "Zamknięte drzwi":"Otwarte drzwi");
      if(readPIR(pirSensor)){
        Serial.println(getDate() + " Sensor PIR uruchamia ALARM");
        changeMode(ALARM);     // Wykrycie ruchu, bez otwarcia drzwi = instant ban
      }        
      else if(!readDoor(doorSensor)){
        Serial.println(getDate() + " Drzwi striggerowały akcje czasu na rozbrojenie " + readDoor(doorSensor));
        changeMode(WPISZKOD);  // Wykrycie otwarcia drzwi = daje czas na wpisanie kodu  
      }  
      else if(checkValidCard()){
        Serial.println(getDate() + " Akcja niedozwolona - użycie karty mimo zamkniętych drzwi i bycie nie wykrytym przez czujnik PIR");
        changeMode(ZABLOKOWANY);
      }      
      lcdBackight(false);
    break;


  // 3. Alarm aktywny       - kamera, sygnał dźwiękowy i świetlny włącza się po wykryciu ruchu
    case ALARM:
      if(checkValidCard()) changeMode(ROZBROJONY);
    break;

    default:
      Serial.print("Something went unexpected wrong >:(");
      wyczyscLCD();
      wyswietl("unknown status", 0);
      wyswietl(getDate(), 1);
      break;
    } 
//    Serial.println(getDate() + " pętla zakończona: " + (String)(millis()-checkTime) + " [ms]");
}

bool watchdog(){
  bool watchdogAlive = true;
  while (watchdogAlive)
    checkState();
  return false;
}
