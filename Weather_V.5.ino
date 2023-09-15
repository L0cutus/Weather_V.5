//#include <WiFiNINA.h>
#include <RTCZero.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BQ24195.h>
#include <ArduinoLowPower.h>
#include "arduino_secrets.h"
#include <ArduinoECCX08.h>
#include "defines.h"
#include <FTPClient_Generic.h>
#include "wiring_private.h"
//#include <WiFiUdp.h>
#include "PMS.h"
#include <utility/wifi_drv.h>
//#include <NTP.h>  // use it if you want time with DST
#include <ArduinoOTA.h>
#include <ArduinoHttpClient.h> // changed from HttpClient.h

const short MAJORVERSION = 5;
const short VERSION = 9;

// My #DEFINEs
#define USING_VSFTP_SERVER      true
#define RED     25
#define GREEN   26
#define BLUE    27

#if USING_VSFTP_SERVER

  // Change according to your FTP server
  char ftp_server[] = "192.168.1.55";

  char ftp_user[]   = SECRET_FTP_USER;
  char ftp_pass[]   = SECRET_FTP_PWD;

  char dirName[]    = "/files/";
  char newDirName[] = "/files/NewDir";

#else

  // Change according to your FTP server
  char ftp_server[] = "192.168.2.241";

  char ftp_user[]   = "teensy4x";
  char ftp_pass[]   = "ftp_test";

  char dirName[]    = "/";
  char newDirName[] = "/NewDir";

#endif

// FTPClient_Generic(char* _serverAdress, char* _userName, char* _passWord, uint16_t _timeout = 10000);
FTPClient_Generic ftp (ftp_server, ftp_user, ftp_pass, 10000);


// Add a new Serial interface
Uart mySerial (&sercom3, 1, 0, SERCOM_RX_PAD_1, UART_TX_PAD_0); // Create the new UART instance assigning it to pin 1 and 0


WiFiClient    wifiClient;  // HTTP


// DEBUG
// #define DEBUG3
// #define DEBUG2
// #define DEBUG1 
// #define LIVE

#ifdef DEBUG2 
  #define debug(x) Serial.print(x)
  #define debugln(x) Serial.println(x)
#else
  #define debug(x)
  #define debugln(x)
#endif

RTCZero rtc;

#define MAX_DATA_RECORDS 200 // 20 = 1 minuto -- 200 = 10 minuti

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

// My Variables
volatile bool alarm = true;     // used into alarm interrupt function
volatile int rainCounter = 0;
volatile int windIndex = 0;     // incrementato ogni 3 secondi
volatile byte pmsIndex = 0;     // incrementato ogni 5 minuti
volatile bool readPMS = false;  // true when we had to make a read operation on PMS5003 sensor
volatile bool sendData = false; // true when we had to send data to FTP server
volatile uint8_t timerCounter = 0;
volatile uint8_t wind[MAX_DATA_RECORDS];            // 200 * 1bytes = 200Bytes
volatile uint8_t windToSend[MAX_DATA_RECORDS];      // blocco dati da inviare copia di wind[]
volatile uint16_t windDir[MAX_DATA_RECORDS];        // 200 * 2bytes = 400Bytes
volatile uint16_t windDirToSend[MAX_DATA_RECORDS];  // blocco dati da inviare copia di windDir[]
volatile PMS::DATA pms5003[2];                      // 12 * 2bytes = 24Bytes
volatile PMS::DATA pms5003ToSend[2];                // 12 * 2bytes = 24Bytes
volatile uint32_t startingEpoch = 0;        // inizio campionamento vento
volatile uint32_t startingEpochToSend = 0;  // come sopra ma da inviare
uint32_t PMSRead0 = 0;
uint32_t PMSRead1 = 0;
char buff[100]; // Buffer for FTP operations
PMS pms(mySerial);
PMS::DATA data;
uint32_t lastTimeUpdate;
uint32_t intervalTimeUpdate = 172800;   // 48h
char firmwareDir[]= "/firmware/";       // used by download firmware updates
char fwChangelog[]="/files/firmware/";  // used to write firmware changes log


// PINs definitions
const int pinWind = 7;
const int pinRain = 5;
const int pinWindDir = A1;  // pin direzione del vento
const int pinPMS5003 = 2;   // pin to enable / disable PMS5003 sensor (HIGH = ENABLED)

// Server data
IPAddress serverIP(192,168,1,55);

Adafruit_BME280 bme; // I2C  

String stringToSend;

float temperature;
float humidity;
float pressure;
float batteryV;
float rain;

void setup() {
  Serial.begin(115200);
  
  mySerial.begin(9600);

  // Additional Serial interface  
  pinPeripheral(1, PIO_SERCOM); //Assign RX function to pin 1
  pinPeripheral(0, PIO_SERCOM); //Assign TX function to pin 0

  setupPins();
  setupWiFi();

  setupRTC();

  pms.passiveMode();

  debug(F("\nStarting FTPClient_ListFiles on "));
  debug(BOARD_NAME);
  debug(F(" with "));
  debugln(SHIELD_TYPE);
  debugln(FTPCLIENT_GENERIC_VERSION);

  pms.sleep();

  // on every reset save via FTP data and firmware version into
  // srw/ftp/files/firmware/fw-changelog.log
  logTheFirmware();

  WiFi.end();  // disattiva il wifi

  setupBME();
  setupBatteryCharger();
  
  analogReference(AR_DEFAULT); 
  analogReadResolution(12);

  startingEpoch = rtc.getEpoch();

  setupISRs();
  setAlarm();
  ECCX08.begin(); // usato perchè sembra stabilizzi l'interfaccia I2C 
  
  debug("Sketch version ");
  debugln(VERSION);

}

void loop() {
  checkSendData();

  LowPower.deepSleep();
}



