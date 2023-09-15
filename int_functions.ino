void alarmMatch() {
  #ifdef LIVE
    timerHandler();
  #endif

  if(windIndex == 0)
  {
    pmsIndex = 0;
    readPMS = true;
  } else if( windIndex == 100)
  {
    pmsIndex = 1;
    readPMS = true;
  }

  windDir[windIndex] = readWindDir();
  windIndex++;

  if(windIndex >= MAX_DATA_RECORDS) {   // if the 10 minutes buffer is full, 
                                        // send it and reinitialize to zero
    windIndex = 0;    // Reinitialize the array index to zero
    startingEpochToSend = startingEpoch;
    
    // copy all data from actula wind buffer to buffer to send to server
    for(int i = 0; i < MAX_DATA_RECORDS; i++)
    {
      windToSend[i] = wind[i];
      wind[i] = 0;
    }

    // copy all data from actual "wind buffer" to "wind buffer to send" to server
    for(int i = 0; i < MAX_DATA_RECORDS; i++)
    {
      windDirToSend[i] = windDir[i];
      windDir[i] = 0;
    }

    // copy all data from actual "pms5003 buffer" to "pms5003 buffer to send" to server
    for(int i = 0; i < 2; i++)
    {
      pms5003ToSend[i].PM_AE_UG_1_0 = pms5003[i].PM_AE_UG_1_0;
      pms5003ToSend[i].PM_AE_UG_2_5 = pms5003[i].PM_AE_UG_2_5;
      pms5003ToSend[i].PM_AE_UG_10_0 = pms5003[i].PM_AE_UG_10_0;
    }

    startingEpoch = rtc.getEpoch();
    sendData = true;  // SEND DATA TO SERVER
  }
  setAlarm(); // set alarm again
  alarm = true; // set alarm again
}

void windISR() {
  wind[windIndex]++; // every wind gauge tournament = 2.54Km/h
}

void rainISR() {
  rainCounter++;
}

// Attach the interrupt handler to the SERCOM
void SERCOM3_Handler()
{
  mySerial.IrqHandler();
}