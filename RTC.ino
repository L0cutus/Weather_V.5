void setupRTC(){
  // CONFIG RTC
  rtc.begin();

  unsigned long epoch;
  int numberOfTries = 0;
  int maxTries = 6;
  do {
    epoch = WiFi.getTime();
    numberOfTries++;
  } while ((epoch == 0) && (numberOfTries < maxTries));

  if (numberOfTries > maxTries) {
    debugln("NTP unreachable!");
    // don't continue:
    while(1);
  } else {
    rtc.setEpoch(epoch);
    lastTimeUpdate = rtc.getEpoch(); // the last time we have updated the rtc clock from internet
  }
}

void updateRTC(){
  if ((rtc.getEpoch() - lastTimeUpdate) < intervalTimeUpdate)
    return;

  unsigned long epoch;
  int numberOfTries = 0;
  int maxTries = 6;
  do {
    epoch = WiFi.getTime();
    numberOfTries++;
  } while ((epoch == 0) && (numberOfTries < maxTries));

  if (numberOfTries > maxTries) {
    debugln("NTP unreachable!");
    // don't continue:
    while(1);
  } else {
    rtc.setEpoch(epoch);
    lastTimeUpdate = rtc.getEpoch(); // the last time we have updated the rtc clock from internet
  }
}
