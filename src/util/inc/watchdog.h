#include <Arduino.h>
void watchdogSetup();
void watchdog();
void wyswietl(
        String _txt,    // określona wartość tekstu (max 16)
        int pos = 0,    // górna (zero) albo dolna (jeden) linijka
        int mode = 0    // tryb alignu tekstu, 0 - center, 1 - left, 2 - right
        );