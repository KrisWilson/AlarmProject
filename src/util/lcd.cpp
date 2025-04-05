#include <LiquidCrystal_I2C.h>
// Konfiguracja LCD 16x2
int backlightTime = 0; // czas podświetlenia ekranu
LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x3F for a 16 chars and 2 line display

void wyswietl(
        String _txt,    // określona wartość tekstu (max 16)
        int pos = 0,    // górna (zero) albo dolna (jeden) linijka
        int mode = 0    // tryb alignu tekstu, 0 - center, 1 - left, 2 - right
        ){
// Przypisanie miejsca startu tekstu na bazie trybu (center/left/right)
// mode = 0 => 16-9 to 7 miejsc wolnych, podzielonych na 2 to od 3 pozycji będzie rendering znaków (centering)
// mode = 1 => left = pos 0
// mode = 2 => right = pos 16-length  
      if(mode != 1 && mode != 2)
        lcd.setCursor((16-_txt.length())/2,pos); 
      else if (mode == 1) 
        lcd.setCursor(0,pos);
      else if (mode == 2)
        lcd.setCursor(16-_txt.length(),pos);
      
        for(int i=0;i<_txt.length();i++)
          lcd.write(_txt[i]);
}

void lcdSetup(){
  lcd.init();  
  lcd.backlight();
}

//TODO: dodaj funkcje zarządzającą backlight (toggle/set)