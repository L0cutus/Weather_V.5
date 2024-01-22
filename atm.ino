void readATM()
{
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure() / 100.0F;

    debug("Temperature:");
    debugln(temperature);
    debug("Humidity:");
    debugln(humidity);
    debug("Pressure:");
    debugln(pressure);
}

void sendATMData()
{
  ftp.OpenConnection();
  ftp.ChangeWorkDir(dirName);

  //Create a new Directory
  ftp.InitFile(COMMAND_XFER_TYPE_ASCII);
  ftp.NewFile("ATMData.json");

  // build wind send JSON string
  // tutti moltiplicati per 100 per eliminare i float meno gestibili
  sprintf(buff, "[{ \"epoch\":\"%u\",", rtc.getEpoch());
  ftp.Write(buff);
  sprintf(buff, "\"temperature\":\"%d\",", (int)(temperature * 100));
  ftp.Write(buff);
  sprintf(buff, "\"humidity\":\"%u\",", (int)(humidity * 100));
  ftp.Write(buff);
  sprintf(buff, "\"pressure\":\"%u\",", (int)(pressure * 100));
  ftp.Write(buff);
  sprintf(buff, "\"batteryV\":\"%u\",", (int)(batteryV * 100));
  ftp.Write(buff);
  sprintf(buff, "\"rain\":\"%u\" }]", rainCounter); // da moltiplicare per 0.2794
  ftp.Write(buff);

  ftp.CloseFile();
  // Close ATM connection
  ftp.CloseConnection();
}