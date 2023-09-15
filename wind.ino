void sendWindData()
{
  debugln("\nStarting connection to FTP server to send WIND data...");
  ftp.OpenConnection();

  debugln("\nChanging to home directory into FTP server...");
  ftp.ChangeWorkDir(dirName);

  //Create a new Directory
  ftp.InitFile(COMMAND_XFER_TYPE_ASCII);
  ftp.NewFile("WINDData.json");

  ftp.Write("[ ");

  // build ands send partial wind JSON string
  for(int i=0; i < MAX_DATA_RECORDS; i++)
  {
    sprintf(buff, "{ \"epoch\":\"%u\",\"wind\":\"%u\",\"winddir\":\"%u\" }", startingEpochToSend + (i*3), windToSend[i], windDirToSend[i]) ;

    if(i + 1 < MAX_DATA_RECORDS)
      strcat(buff, ",") ;

    ftp.Write(buff);

  }

  ftp.Write(" ]");

  ftp.CloseFile();

  // Close WIND connection
  ftp.CloseConnection();

  // ************************ data ready to send to database *************************
  ftp.OpenConnection();
  ftp.ChangeWorkDir(dirName);
  ftp.InitFile(COMMAND_XFER_TYPE_ASCII);
  ftp.NewFile("Data_Rdy.txt");
  ftp.Write("Hey! There are new data to put into database!");
  ftp.CloseFile();

  // Close data ready connection
  debugln("CloseConnection");
  ftp.CloseConnection();
}

uint16_t readWindDir(void)
{
  float readWindDir = (float) (3.3 / 4095.0) * analogRead(pinWindDir);
  if (readWindDir >= 3.00)
        return 2700;          // 270.0
  else  if (readWindDir >= 2.81)
          return 3150;        // 315.0
  else  if (readWindDir >= 2.63)
          return 2925;        // 292.5
  else  if (readWindDir >= 2.50)
          return 0;           // 0 
  else  if (readWindDir >= 2.23)
          return 3375;        // 337.5
  else  if (readWindDir >= 2.00)
          return 2250;        // 225.0
  else  if (readWindDir >= 1.90)
          return 2475;        // 247.5
  else  if (readWindDir >= 1.46)
          return 450;         // 45.0
  else  if (readWindDir >= 1.28)
          return 225;         // 22.5
  else  if (readWindDir >= 0.91)
          return 1800;        // 180.0
  else  if (readWindDir >= 0.77)
          return 2025;        // 202.5
  else  if (readWindDir >= 0.58)
          return 1350;        // 135.0
  else  if (readWindDir >= 0.39)
          return 1575;        // 157.5
  else  if (readWindDir >= 0.29)
          return 900;         // 90.0
  else  if (readWindDir >= 0.26)
          return 675;         // 67.5
  else  if (readWindDir >= 0.20)
          return 1125;        // 112.5
}
