bool readPIR(
        int pin            // pin na którym znajduje się sygnał z czujnika PIR
        );
bool readDoor(
        int pin = 33        // pin na którym znajduje się złącze stykowe do określonego chronionego obiektu
        );
void play(
        int pin,            // określony pin na buzzer
        int song=0,         // określony sygnał, zero oznacza domyślny = stan wysoki pisku
        int length=0,       // określona długość w sekundach, zero oznacza do odwołania
        bool active=true    // wyłączenie 
        );
void light(int pin=0, bool active=true);