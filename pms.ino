void ReadPMSData()
{
  debugln("Waking up, wait 45 seconds for stable readings...");
  
  #ifdef DEBUG3
    WiFiDrv::analogWrite(RED, 0);   //RED
    WiFiDrv::analogWrite(GREEN, 0); //GREEN
    WiFiDrv::analogWrite(BLUE, 16);   //BLUE
  #endif

  digitalWrite(pinPMS5003, HIGH); // Wakeup PMS5003 sensor
  //pms.wakeUp();
  delay(45000); // wait 45seconds for sensor stabilization

  if(pmsIndex == 0)
    PMSRead0 = rtc.getEpoch();
  else 
    PMSRead1 = rtc.getEpoch();

  debugln("Send read request...");

  pms.requestRead();

  debugln("Reading data...");
  if (pms.readUntil(data))
  {
    // Atmospheric environment
    debugln("Atmospheric environment");
    debug("PM 1.0 (ug/m3): ");
    debugln(data.PM_AE_UG_1_0);
    debug("PM 2.5 (ug/m3): ");
    debugln(data.PM_AE_UG_2_5);
    debug("PM 10.0 (ug/m3): ");
    debugln(data.PM_AE_UG_10_0);

    pms5003[pmsIndex].PM_AE_UG_1_0 = data.PM_AE_UG_1_0;
    pms5003[pmsIndex].PM_AE_UG_2_5 = data.PM_AE_UG_2_5;
    pms5003[pmsIndex].PM_AE_UG_10_0 = data.PM_AE_UG_10_0;
   
  }
  else
    debugln("No data.");
  

  #ifdef DEBUG3
    WiFiDrv::analogWrite(RED, 0);   //RED
    WiFiDrv::analogWrite(GREEN, 0); //GREEN
    WiFiDrv::analogWrite(BLUE, 0);   //BLUE
  #endif

  //pms.sleep();
  digitalWrite(pinPMS5003, LOW); // sleep PMS5003 sensor

  // pmsIndex = 0; // Reinitialize the array index to zero
  readPMS = false;
}

void sendPMSData()
{
  ftp.OpenConnection();
  ftp.ChangeWorkDir(dirName);

  //Create a new Directory
  ftp.InitFile(COMMAND_XFER_TYPE_ASCII);
  ftp.NewFile("PMSData.json");

  // build and send pms5003 JSON file
  sprintf(buff, "[{ \"epoch\":\"%u\",", PMSRead0);   // first measurement is 5 min old
  ftp.Write(buff);
  sprintf(buff, "\"PM1\":\"%u\",", pms5003ToSend[0].PM_AE_UG_1_0);
  ftp.Write(buff);
  sprintf(buff, "\"PM2_5\":\"%u\",", pms5003ToSend[0].PM_AE_UG_2_5);
  ftp.Write(buff);
  sprintf(buff, "\"PM10\":\"%u\"},", pms5003ToSend[0].PM_AE_UG_10_0);
  ftp.Write(buff);

  sprintf(buff, "{ \"epoch\":\"%u\",", PMSRead1); 
  ftp.Write(buff);
  sprintf(buff, "\"PM1\":\"%u\",", pms5003ToSend[1].PM_AE_UG_1_0);
  ftp.Write(buff);
  sprintf(buff, "\"PM2_5\":\"%u\",", pms5003ToSend[1].PM_AE_UG_2_5);
  ftp.Write(buff);
  sprintf(buff, "\"PM10\":\"%u\"}]", pms5003ToSend[1].PM_AE_UG_10_0);
  ftp.Write(buff);

  ftp.CloseFile();
  // Close PMS5003 connection
  ftp.CloseConnection();
}