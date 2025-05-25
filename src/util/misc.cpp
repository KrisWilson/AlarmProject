// Wymagany czujnik ruchu:
//      Czujnik pir      -- misc.cpp
//  1 Czujnik PIR podłączony pod X pin

// Wymagany czujnik otwarcia drzwi:
//      Czujnik krańcowy -- misc.cpp
//  1 Czujnik krańcowy podłączony pod X pin

// Wymagany sygnał dźwiękowy:
//      Syrena           -- misc.cpp
//  1 buzzer podłączony pod X pin

// Wymagany sygnał świetlny:
//      Ledy             -- misc.cpp
//  2 diody podłączone pod piny: (X, Y, Z)
//      Dioda X służy do określania stanu uzbrojenia            (Stan Uzbrojony/Nieuzbrojony)
//      Dioda Y służy do określenia stanu oczekiwania na kod    (Stan przedalarmowy)
//  2 diody ponieważ nie chcę się bawić w migotanie diody, bo to wymaga używanie wielowątkowości która aktualnie może zaszkodzić przez mój brak doświadczenia

#include "inc/pitches.h"

// funkcja odczytania z czujnika PIR czy wykrył ruch
#include <io_pin_remap.h>
bool readPIR(
        int pin = 13          // pin na którym znajduje się sygnał z czujnika PIR
        ){

    // PIR - poziom HIGH na pinie oznacza wykryty ruch, LOW oznacza brak ruchu.
    // System PIR załącza się w 20 sekund, więc potencjalnie istnieje zagrożenie złego odczytu przy boot-upie.
    return ((analogRead(pin)==4095)||(digitalRead(pin))? true:false);
}


bool readDoor(
        int pin = 33            // pin na którym znajduje się złącze stykowe do określonego chronionego obiektu
        ){
    // propozycja to użycie read signal na określonym pinie, czy jest stan wysoki (Zamknięty obwód, czyli zamknięte drzwi)
    // jeszcze lepszą opcją byłoby wywołać przerwanie i wysłać sygnał otwarcia drzwi, aby nie było sytuacji że
    // otwarcie drzwi w idealnej sekundzie moze spowodować uniknięcie sygnału. - jednakże to nie realne, otwarcie i zamknięcie w mniej niż 0.1s    
    return ((analogRead(pin)==4095)||(digitalRead(pin))? true:false); 
}


//TODO: Dodaj funkcję na RFID oraz sygnał alarmowy-świetlny

//  w sumie to nie widzi mi się aby modularnie nadawać syrenę, dajmy po prostu głośny pisk

  void play(
        int pin,            // określony pin na buzzer
        int song=0,         // określony sygnał, zero oznacza domyślny = stan wysoki pisku
        int length=0,       // określona długość w sekundach, zero oznacza do odwołania
        bool active=true    // wyłączenie 
        ){
    if(active)
        digitalWrite(pin, HIGH);
        // TODO: USTAWIENIE PINU BUZZER NA WYSOKI STAN
    else
        digitalWrite(pin,LOW);
        // TODO: USTAWIENIE PINU BUZZER NA NISKI STAN (wyłączony)


    // //  jeżeli uda się to w asynchroniczny sposób zrobić to dajmy tutaj syrenę aby w tle sobie migotała
    // // notes in the melody:
    // int melody[] = {
    //     NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
    // };
    // // note durations: 4 = quarter note, 8 = eighth note, etc.:
    // int noteDurations[] = {
    //     4, 8, 8, 4, 4, 4, 4, 4
    // };
    // // iterate over the notes of the melody:
    // for (int thisNote = 0; thisNote < 8; thisNote++) {
    //     // to calculate the note duration, take one second divided by the note type.
    //     //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    //     int noteDuration = 1000 / noteDurations[thisNote];
    //     tone(25, melody[thisNote], noteDuration);
    //     // to distinguish the notes, set a minimum time between them.
    //     // the note's duration + 30% seems to work well:
    //     int pauseBetweenNotes = noteDuration * 1.30;
    //     delay(pauseBetweenNotes);
    //     // stop the tone playing:
    //     noTone(25);
    //}

}


//  ta funkcja zależnie od stanu uzbrojenia przyjmuje różne wartości wewnątrz
void light(int pin=0, bool active=true){
    digitalWrite(pin, 255);
}