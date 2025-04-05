#include <Arduino.h>
#include <EEPROM.h>
#include <RtcDS1302.h>
#include <util/keys.cpp>
#include <RtcDS1302.h>


// W konfiguracji przechowujemy RTC i konfigi zwiazane z RTC// CONNECTIONS:
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO --> 4
// DS1302 RST/CE --> 2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND
RtcDS1302<ThreeWire> Rtc(myWire);


// Konfiguracyjne zmienne - defaultowe wartości, nim będą przypisane z eepromu
String password = "12345";
int exitTime = 15; // czas na wyjście po zabezpieczeniu
int day = 0, month = 0, year = 0, minutes = 0, hour = 0;

#define EEPROM_SIZE 1024              //Rozmiar jest potrzebny do zainicjowania emulacji pamięci na ESP32
#define configExistAddress 0x00       
#define passwordAddress 0x001
#define exitTimeAddress 0x3fe
#define backlightTimeAddress 0x3fc

void setupRTC(){
    Rtc.Begin();
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing
        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
}

String dateTime(const RtcDateTime& dt)
{
    char datestring[26];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
        
    return datestring;
}


bool setupFromEEPROM(){
    // ONLY FOR TESTING PURPOSES
    // CLEAN MEMORY
    // for(int i = 0; i < EEPROM_SIZE; i++)
    // {
    //   EEPROM.write(i, 0x00);
    // }
    // EEPROM.commit(); // faktyczne zapisanie danych do pamięci flash
    // END TESTING

    // Sprawdzanie czy istnieje konfiguracja
    // Jeśli tak to podaj hasło
    // Jeśli nie to stwórz
    bool configExist = (bool)EEPROM.read(configExistAddress); // to trzeba pozmieniać na readBool ta samo jak na write bool
    if (!configExist)
    {
        // Wypisanie tekstu powitalnego
        Serial.println("Witaj w systemie alarmowym HOMESEC XD");
        Serial.println("Konfiguracja systemu...");
        passwordSection:
        // Wprowadź hasło do systemu max długość 1015 znaków
        String password = ReadPassword(); // zastąp ReadPassword() funkcją do odczytu hasła
        if (password.length() > 1015)
        {
            Serial.println("Hasło jest za długie!");
            goto passwordSection;
        }
        
        passwordFromMemory = password;
        for (int i = 0; i < password.length(); i++)
        {
            byte byteAtCurrentStringPosition = (byte)password[i];
            EEPROM.write(passwordAddress + i, byteAtCurrentStringPosition);
        }

        // Zapisz czas na wyjście po zabezpieczeniu
        Serial.println("Wprowadź czas na wyjście po zabezpieczeniu (s):");
        exitTime = ReadNumericInput(0, 255); // w sekundach
        EEPROM.write(exitTimeAddress, exitTime);
        // Zapisz czas podświetlenia ekranu
        Serial.println("Wprowadź czas podświetlenia ekranu (s):");
        int backlightTime = ReadNumericInput(0, 255); // w sekundach
        EEPROM.write(backlightTimeAddress, backlightTime);

        // Zapisz w pierwszym bajcie pamięci że konfiguracja istnieje
        EEPROM.write(configExistAddress, 0x01); // sprawdź czy to działa

        EEPROM.commit(); // faktyczne zapisanie danych do pamięci flash
    }
    else
    {
        // Sprawdź czy hasło jest poprawne
        // Odczytaj hasło z pamięci
        for (int i = 0; i < 1015; i++)
        {
            byte byteAtCurrentStringPosition = EEPROM.read(passwordAddress + i);
            if (byteAtCurrentStringPosition == 0)
            break; // koniec hasła
            passwordFromMemory += (char)byteAtCurrentStringPosition;
        }
        // Wpisanie hasła i porównanie z hasłem z pamięci
        int passwordAttempts = 0;
        passwordInput:
        Serial.println("Podaj hasło: ");
        String password = ReadPassword();
        if (password != passwordFromMemory && passwordAttempts < 3)
        {
            Serial.println("Hasło jest niepoprawne!");
            passwordAttempts++;
            goto passwordInput;
            }
        else if (password != passwordFromMemory && passwordAttempts >= 3)
        {
            Serial.println("Za dużo prób!");
            // ALARM ???
        }
        // Jeśli wpisane hasło jest poprawne to podaj date i aktualny czas

        Serial.println("Podaj dzień:");
        day = ReadNumericInput(1, 31);
        Serial.println("Podaj miesiąc:");
        month = ReadNumericInput(1, 12);
        Serial.println("Podaj rok:");
        year = ReadNumericInput(0, 99);
        Serial.println("Podaj godzinę:");
        hour = ReadNumericInput(0, 23);
        Serial.println("Podaj minutę:");
        minutes = ReadNumericInput(0, 59);
        }
    return false;
}