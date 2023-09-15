void setupWiFi() {

  Serial.print("Sketch version ");
  Serial.println(VERSION);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  uint8_t tt = 0;
  while( (status != WL_CONNECTED) || (WiFi.RSSI() <= -90) || (WiFi.RSSI() == 0)) {
    debug("Attempting to connect to SSID: ");
    debugln(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(7000);

    if (tt++ > 5) // Try 5 times then STOP
      while(1);

  };
  WiFi.lowPowerMode();
  
  #ifdef DEBUG1
    printWiFiStatus();
  #endif
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:

  debug("SSID: ");

  debugln(WiFi.SSID());
	
  // print your WiFi shield's IP address:
	
  IPAddress ip = WiFi.localIP();
	
  debug("IP Address: ");
	
  debugln(ip);
	
  // print the received signal strength:
	
  long rssi = WiFi.RSSI();
	
  debug("signal strength (RSSI):");
	
  debug(rssi);
	
  debugln(" dBm");
}
