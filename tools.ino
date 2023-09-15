void printTime() {
  print2digits(rtc.getHours());
  debug(":");
  print2digits(rtc.getMinutes());
  debug(":");
  print2digits(rtc.getSeconds());
  debugln();
}

void printDate() {
  debug(rtc.getDay());
  debug("/");
  debug(rtc.getMonth());
  debug("/");
  debug(rtc.getYear());
  debug(" ");
}
	
void print2digits(int number) {

  if (number < 10) {

    debug("0");

  }
  debug(number);
}

void setupISRs(){
  attachInterrupt(pinWind, windISR, FALLING);
  attachInterrupt(pinRain, rainISR, FALLING);    
}

void checkForNewFirmware(){
  const char PATH[] = "%supdate-v%d.bin";         // Set the URI to the .bin firmware
  const unsigned long CHECK_INTERVAL = 6000;      // Time interval between update checks (ms)
  const unsigned short HTTP_SERVER_PORT = 5000;   // Commonly 80 (HTTP) | 443 (HTTPS)

  HttpClient client(wifiClient, serverIP, HTTP_SERVER_PORT);  // HTTP
  // HttpClient client(wifiClientSSL, SERVER, SERVER_PORT);  // HTTPS

  char buff[32];
  snprintf(buff, sizeof(buff), PATH, firmwareDir, VERSION + 1);

  debug("Check for update file ");
  debugln(buff);

  // // Make the GET request
  client.get(buff);

  int statusCode = client.responseStatusCode();
  Serial.print("Update status code: ");
  Serial.println(statusCode);
  if (statusCode != 200) {
    client.stop();
    return;
  }

  long length = client.contentLength();
  if (length == HttpClient::kNoContentLengthHeader) {
    client.stop();
    Serial.println("Server didn't provide Content-length header. Can't continue with update.");
    return;
  }
  Serial.print("Server returned update file of size ");
  Serial.print(length);
  Serial.println(" bytes");

  if (!InternalStorage.open(length)) {
    client.stop();
    Serial.println("There is not enough space to store the update. Can't continue with update.");
    return;
  }
  byte b;
  while (length > 0) {
    if (!client.readBytes(&b, 1)) // reading a byte with timeout
      break;
    InternalStorage.write(b);
    length--;
  }
  InternalStorage.close();
  client.stop();
  if (length > 0) {
    Serial.print("Timeout downloading update file at ");
    Serial.print(length);
    Serial.println(" bytes. Can't continue with update.");
    return;
  }

  Serial.println("Sketch update apply and reset.");
  Serial.flush();
  InternalStorage.apply(); // this doesn't return
}  

// on every reset save via FTP data and firmware version into
// srw/ftp/files/firmware/fw-changelog.log
void logTheFirmware(void)
{
  ftp.OpenConnection();
  ftp.ChangeWorkDir(fwChangelog);

  ftp.InitFile(COMMAND_XFER_TYPE_ASCII);
  ftp.AppendFile("fw-changelog.log");

  // Send date + time  
  sprintf(buff, "%u/%u/%u %u:%u:%u  ", rtc.getDay(), rtc.getMonth(), rtc.getYear(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds() );
  ftp.Write(buff);
  sprintf(buff, "firmware version: %u\n", VERSION );
  ftp.Write(buff);

  ftp.CloseFile();
  // Close ATM connection
  ftp.CloseConnection();

}
