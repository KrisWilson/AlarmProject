#include <util/include.h>
// Tryby pracy
// 1. Niezainicjalizowany - brak konfiguracji.
// 2. Rozbrojony          - czujniki nieaktywne, kamera wyłączona
// 3. Wstępnie uzbrojony  - czujki nieaktywne, odliczanie zadanego czasu do uzbrojenia, możliwość wpisania hasła aby rozbroić?
// 4. Uzbrojony           - czujnik krańcowy i ruchu aktywne
// 5. Alarm aktywny       - kamera, sygnał dźwiękowy i świetlny włącza się po wykryciu ruchu

bool disarmed = false; // zmienna do rozbrojenia systemu


TaskHandle_t clockTaskHandle = NULL;
TaskHandle_t inputDelayTaskHandle = NULL;




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

void clockTask(void *pvParameters)
{
  // Funkcja do odliczania czasu
  while (true)
  {
    // Odczytanie aktualnej daty i godziny
    Serial.print("Aktualna data i godzina: ");
    Serial.print(day);
    Serial.print(".");
    Serial.print(month);
    Serial.print(".");
    Serial.print(year);
    Serial.print(" ");
    Serial.print(hour);
    Serial.print(":");
    Serial.println(minutes);
    vTaskDelay(1000*60 / portTICK_PERIOD_MS); //chyba to jest 1 minuta ale nie wiem lol
    minutes++;
    if (minutes >= 60)
    {
      minutes = 0;
      hour++;
      if (hour >= 24)
      {
        hour = 0;
        day++;
        if (day > 31)
        {
          day = 1;
          month++;
          if (month > 12)
          {
            month = 1;
            year++;
          }
        }
      }
    }
  }
}

