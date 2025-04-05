#include <util/misc.cpp>
bool disarmed = false; // zmienna do rozbrojenia systemu
#define ROZBROJONY    0 // chilluje i czeka na uzbrojenie (0 -> 2 -> 1)
#define UZBROJONY     1 // agresywnie wyszukuje inputów   (1 -> 5 lub 1 -> 3 -> 5)
#define OPUSCLOKAL    2 // Czas na opuszczenie lokalu     (2 -> 1 lub 2 -> 5)
#define WPISZKOD      3 // Czas na wpisanie kodu          (3 -> 5 lub 3 -> 0)
#define ZABLOKOWANY   4 // ?? Zakładam moment błędnie wpisanego kodu, ale to bez sensu. (4->5)
#define ALARM         10// Alarm sygnalizuje katastrofę   (5 -> 0) 
int armMode = 0;

TaskHandle_t clockTaskHandle = NULL;
TaskHandle_t inputDelayTaskHandle = NULL;

void changeMode(int _new){
  switch(_new){
    case ROZBROJONY:
      armMode = ROZBROJONY;
      light(ledStatus, LOW);
      light(ledWaiting, LOW);
    break;

    case UZBROJONY:
      armMode = UZBROJONY;
      light(ledStatus, HIGH);
      light(ledWaiting, LOW);
    break;

    case OPUSCLOKAL:
      armMode = OPUSCLOKAL;
      light(ledStatus, HIGH);
      light(ledWaiting, LOW);
    break;

    case WPISZKOD:
      armMode = WPISZKOD;
      light(ledStatus, HIGH);
      light(ledWaiting, HIGH);
    break;

    case ALARM:
      armMode = ALARM;
      light(ledStatus, HIGH);
      light(ledWaiting, HIGH);
    break;

    default:
      armMode = ZABLOKOWANY;
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

