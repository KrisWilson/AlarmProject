#include <LiquidCrystal_I2C.h>
// Konfiguracja LCD 16x2
int backlightTime = 0; // czas podświetlenia ekranu
LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x3F for a 16 chars and 2 line display

void wyswietl(
        String _txt,    // określona wartość tekstu (max 16)
        int pos = 0,    // górna (zero) albo dolna (jeden) linijka
        int mode = 0    // tryb alignu tekstu, 0 - center, 1 - left, 2 - right
        ){
      lcd.setCursor((16-_txt.length())/2,pos);
      for(int i=0;i<_txt.length())
        lcd.write((_txt[i])

}