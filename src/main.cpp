#include "util/inc/watchdog.h"
void setup()
{
  watchdogSetup();
  watchdog();
}

void loop()
{
  wyswietl("watchdog.exe crashed",0,0);
}
