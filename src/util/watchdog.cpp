#include <util/misc.cpp>
bool disarmed = false;  // zmienna do rozbrojenia systemu
#define ROZBROJONY    0 // chilluje i czeka na uzbrojenie (0 -> 2 -> 1)
#define UZBROJONY     1 // agresywnie wyszukuje inputów   (1 -> 5 lub 1 -> 3 -> 5)
#define OPUSCLOKAL    2 // Czas na opuszczenie lokalu     (2 -> 1 lub 2 -> 5)
#define WPISZKOD      3 // Czas na wpisanie kodu          (3 -> 5 lub 3 -> 0)
#define ZABLOKOWANY   4 // ?? Zakładam moment błędnie wpisanego kodu, ale to bez sensu. (4->5)
#define ALARM         10// Alarm sygnalizuje katastrofę   (5 -> 0) 
int armMode = ROZBROJONY;// aktualny status watchdog'a      

TaskHandle_t clockTaskHandle = NULL;
TaskHandle_t inputDelayTaskHandle = NULL;


bool watchdog(){
  bool watchdogAlive = true;
  while (watchdogAlive){
    checkState();
  }
}

// Funkcja która pozwala zmieniać wiele elementów jednocześnie przy zmianie stanu
void changeMode(int _new){
  switch(_new){
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
}


void inputDelay(void *pvParameters)
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


void checkState(){
  RtcDateTime now = Rtc.GetDateTime();
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
      wyswietl("Uzbrajanie...");
    break;

  // 2. Uzbrojony           - czujnik krańcowy i ruchu aktywne
    case UZBROJONY:
      if(readPIR(pirSensor))   changeMode(ALARM);     // wykrycie ruchu, bez otwarcia drzwi = instant ban
      if(readDoor(doorSensor)) changeMode(WPISZKOD);  // Wykrycie otwarcia drzwi = daje czas na wpisanie kodu   

      Serial.print("Uzbrojony");
      Serial.print(dateTime(now));

      wyswietl("Uzbrojony", 0);
      wyswietl(dateTime(now), 1);
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

void ArmedSystem()
{
  // Wyświetlenie odliczania zadanego przez użytkownika
  int i = 0;
  while (i < exitTime)
  {
    Serial.print(i);
    delay(1000);
    i++;
  }
  // beep beep system uzbrojony
  if (readPIR()) // wadą tego rozwiązania jest to, że jak nie wykryje ruchu to nie można rozbroić systemu
  {
    //Start odliczania 30s na wpisanie hasła w innym wątku 
    xTaskCreatePinnedToCore(
                          inputDelay, /* Task function. */
                          "TaskClock",   /* name of task. */
                          10000,     /* Stack size of task */
                          NULL,      /* parameter of the task */
                          1,         /* priority of the task */
                          &inputDelayTaskHandle,    /* Task handle to keep track of created task */
                          0);  
                          
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
      Serial.println("Podano 3 błędne hasła! Uruchominie alarmu");
      // ALARM!!!!!!!!!!!!!!!!!!!!!!! WOŁAJTA POLICJE ZŁODZIEJE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
    else
    {
      disarmed = true;
      Serial.println("System rozbrojony!");
      // wyłączenie alarmu
      // wyłączenie kamery
      // wyłączenie syreny
      // wyłączenie diod
      vTaskDelete(inputDelayTaskHandle); // usunięcie taska odliczania czasu na wpisanie hasła
    }
  }
}

