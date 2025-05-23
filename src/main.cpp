#include "util/inc/watchdog.h"
void setup()
{
  Serial.begin(115200);
  watchdogSetup();
  watchdog();
}

void loop()
{
  wyswietl("watchdog.exe crashed",0,0);
}
