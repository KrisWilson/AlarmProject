#include <Arduino.h>
#include <PinsDef.h>

//////////////////////////////////////////////////////////////////////////////////////////////
//                                 Temat projektu                                           //                                                                              //
// System ochrony (alarm) dla domu lub firmy                                                //         
// Wykorzystanie czujnika ruchu oraz łącznika (np. drzwiowego) do uaktywnienia sygnału      //
// dźwiękowego, optycznego oraz dodatkowego łącznika (do załączenia kamery). Zasilanie 5V.  //
//////////////////////////////////////////////////////////////////////////////////////////////

// Lista elementów 
// Arduino R - kontroler całego systemu, wyświetla menu, odbiera sygnały z czujników
// Klawiatura  
// Czujnik pir 
// Czujnik krańcowy
// Syrena
// Ledy 
// Raspberry pi - serwer do zapisywania wideo z kamery, odbiera sygnał z arduino o urochomienie kamery
// Kamera USB


void setup() {
  // tutaj sprawdzanie czy istanieje konfiguracja ale nie wiem jak i czy da się zapisać hasło na arduino do pamięci
  // Jeśli konfiguracja nie istnieje to stwórz. 
  // Używanie wewnętrznej pamięci arduino 
  // https://www.digikey.com/en/maker/blogs/2021/how-to-permanently-store-data-on-an-arduinos-built-in-eeprom


  

}

void loop() {
// 1. Wyświetl tekst powitalny
// 2. Jeśli konfiguracja (nie)istanieje to 
// 3A. Wyświetl Prompta o hasło
// 
// 3B. Wyświetl menu konfiguracji 
//                1) Wprowadź hasło
//                2) Ustaw date i czas
//                3) Ustaw czas na wyjście po zabezpieczeniu 
//                4) Ustaw czas podświetlenia ekranu
//                5) Ustawienia kamery???????????????????????????????????????????????????????????
// 
// 4. Po zalogowaniu do systemu wyświetl datę i czas oraz możliwośc do wejścia do menu z ustawieniami 

// Tryby pracy 
// 1. Niezainicjalizowany - brak konfiguracji.
// 2. Rozbrojony          - czujniki nieaktywne, kamera wyłączona 
// 3. Wstępnie uzbrojony  - czujki nieaktywne, odliczanie zadanego czasu do uzbrojenia
// 4. Uzbrojony           - czujnik krańcowy i ruchu aktywne
// 5. Alarm aktywny       - kamera, sygnał dźwiękowy i świetlny włącza się po wykryciu ruchu


}

bool DetectMovement(){
  while(true){
    // Czujnik ruchu
    if(digitalRead(pirSensor) == HIGH){
      return true;
    }
    // Czujnik krańcowy
    if(digitalRead(doorSensor) == HIGH){
      return true;
    }
  }
}
