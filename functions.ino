void setupPins() {
  pinMode(pinWind, INPUT);
  pinMode(pinRain, INPUT);
  pinMode(pinPMS5003, OUTPUT);
  digitalWrite(pinPMS5003, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  WiFiDrv::pinMode(RED, OUTPUT);
  WiFiDrv::pinMode(GREEN, OUTPUT);
  WiFiDrv::pinMode(BLUE, OUTPUT);
}

void setupBatteryCharger()
{
  PMIC.begin();
  PMIC.disableCharge();  
  PMIC.end();
}

void setupBME()
{
  unsigned status;

  status = bme.begin(0x76, &Wire);
  // Initialize BME280 sensor
  if (!status) 
  {
        debugln("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        debug("SensorID was: "); debugln(bme.sensorID());
        debug("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        debug("   ID of 0x56-0x58 represents a BMP 280,\n");
        debug("        ID of 0x60 represents a BME 280.\n");
        debug("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
  }
}


void checkAlarm() {
  if (alarm) {
    debugln();
    debug("windIndex: ");
    debugln(windIndex);
    debug("Epoch: ");
    debugln(rtc.getEpoch());

    alarm = false;
  }
}

void setAlarm(){
  /*
    I watched a bit closer onto your setAlarm().
    I don't think it's wrong but IMHO not very maintenance friendly.
    Changing INTERVAL to anything else than 60 might break this code.
    I would suggest instead: 
    
    second_alarm += INTERVAL; minute_alarm += second_alarm / 60;
    second_alarm %= 60; hour_alarm += minute_alarm / 60;
    minute_alarm %= 60; hour_alarm %= 24;
    
    There is no more if necessary what's usually a performance
    plus (even with some more arithmetic operations which are
    on most modern CPUs much faster than potential branches)
  */
  static uint8_t INTERVAL = 2;
  uint8_t seconds_alarm = rtc.getSeconds(); 
  uint8_t minutes_alarm = rtc.getMinutes();
  uint8_t hours_alarm   = rtc.getHours();

  seconds_alarm += INTERVAL;
  minutes_alarm += seconds_alarm / 60;
  seconds_alarm %= 60;
  hours_alarm   += minutes_alarm / 60;
  minutes_alarm %= 60;
  hours_alarm   %= 24;

  rtc.setAlarmSeconds(seconds_alarm); // setta il wakeup a 3 secondi dall'ora attuale
  rtc.setAlarmMinutes(minutes_alarm); // setta il wakeup a 3 secondi dall'ora attuale
  rtc.setAlarmHours(hours_alarm); // setta il wakeup a 3 secondi dall'ora attuale
  rtc.attachInterrupt(alarmMatch);
  rtc.enableAlarm(rtc.MATCH_HHMMSS);
}

void checkSendData(){
  
  if(readPMS)
    ReadPMSData();
  
  if (sendData) 
  {
    #ifdef DEBUG3
      WiFiDrv::analogWrite(RED, 16);   //RED
      WiFiDrv::analogWrite(GREEN, 16); //GREEN
      WiFiDrv::analogWrite(BLUE, 0);   //BLUE
    #endif

    #ifdef DEBUG2
      debugln();
      debug("Orario misurazione: ");
      debug(rtc.getHours());
      debug(":");
      debug(rtc.getMinutes());
      debug(":");
      debugln(rtc.getSeconds());
    #endif

    readATM();
    readBattery();
    setupWiFi();
    updateRTC();

    // ************************ send ATM data ********************************
    sendATMData();

    // ************************ send PMS5003 data ********************************
    sendPMSData();

    // ************************ send WIND data ********************************
    sendWindData();

    checkForNewFirmware();

    WiFi.end();
    rainCounter = 0;

     // SEND DATA TO SERVER
    sendData = false;

    #ifdef DEBUG3
      WiFiDrv::analogWrite(RED, 0);   //RED
      WiFiDrv::analogWrite(GREEN, 0); //GREEN
      WiFiDrv::analogWrite(BLUE, 0);   //BLUE
    #endif
  }

  #ifdef DEBUG1
    checkAlarm();
  #endif
}

void readBattery()
{
    float batteryADC = analogRead(A2) * (3.3 / 4095);
    batteryV = batteryADC * (4.2 / 3.3);
    
    debug("battery: ");
    debugln(batteryV);
}

void timerHandler()
{
  static bool toggle = true;

  //timer interrupt toggles pin LED_BUILTIN
  digitalWrite(LED_BUILTIN, toggle);
  toggle = !toggle;
}
