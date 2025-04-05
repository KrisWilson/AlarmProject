#include <util/watchdog.cpp>
void setup()
{
  watchdogSetup();
  watchdog();
}

void loop()
{
  wyswietl("watchdog.exe crashed");
  RtcDateTime now = Rtc.GetDateTime();
  wyswietl(dateTime(now), 1);
}
