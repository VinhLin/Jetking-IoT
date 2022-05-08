#include <Arduino.h>

// Select your modem
#define TINY_GSM_MODEM_SIM800

// #define _TASK_TIMECRITICAL      // Enable monitoring scheduling overruns
#define _TASK_SLEEP_ON_IDLE_RUN // Enable 1 ms SLEEP_IDLE powerdowns between tasks if no callback methods were invoked during the pass
//#define _TASK_STATUS_REQUEST    // Compile with support for StatusRequest functionality - triggering tasks on status change events in addition to time only
// #define _TASK_WDT_IDS           // Compile with support for wdt control points and task ids
// #define _TASK_LTS_POINTER       // Compile with support for local task storage pointer
// #define _TASK_PRIORITY          // Support for layered scheduling priority
// #define _TASK_MICRO_RES         // Support for microsecond resolution
// #define _TASK_STD_FUNCTION      // Support for std::function (ESP8266 ONLY)
// #define _TASK_DEBUG             // Make all methods and variables public for debug purposes
// #define _TASK_INLINE         // Make all methods "inline" - needed to support some multi-tab, multi-file implementations
#define _TASK_TIMEOUT

//***************************************Libraries***********************************************
#include <SoftwareSerial.h>
#include <esp_task_wdt.h>
#include "ThingsBoard.h"
#include <TinyGsmClient.h>
#include <TinyGPSPlus.h>
#include <TaskScheduler.h>
#include <TimeLib.h>
#include "SPIFFS.h"

struct Data_Send_PlatformIoT
{
  float GPS_Lat;
  float GPS_Long;
  float GPS_Speed;
  String Date_Time;
};

// See https://thingsboard.io/docs/getting-started-guides/helloworld/
// to understand how to obtain an access token
#define THINGSBOARD_SERVER "demo.thingsboard.io"
#define TOKEN "lDwJnG8ZEmCTJ3xHnkvR"
int count_connect_platformIoT;

// 45 seconds WDT
#define WDT_TIMEOUT 45

// define status gps and gsm
bool status_gps = false;
bool status_gsm = false;

// storage path
#define log_path "/log/log_data.txt"

//*******************************Module GSM Sim800************************************
// Define baudrate for module GSM
static const uint32_t GSMBaud = 9600;

// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[] = "v-internet";
const char user[] = "";
const char pass[] = "";

// SIM card PIN (leave empty, if not defined)
const char simPIN[] = "";

// Initialize GSM modem
TinyGsm modem(Serial2);

// Initialize GSM client
TinyGsmClient client(modem);

// Initialize ThingsBoard instance
ThingsBoard tb(client);

//***************************************Module GPS***********************************
// Define baudrate for module GPS
static const uint32_t GPSBaud = 9600;

// Serial port for GPS shield
#define RX_MODEM (14)
#define TX_MODEM (27)
SoftwareSerial serialGPS(RX_MODEM, TX_MODEM);

//****************************************Define***************************************
auto &debugSerial = Serial;

// Version FW
const char *VER = "1.0.2-Beta";

// Pin out status led
#define GSM_Led (13)
#define GPS_Led (12)
#define PWR_Led (5)

// The TinyGPSPlus object
TinyGPSPlus gps;

// Init struct Data_Send_PlatformIoT
Data_Send_PlatformIoT Data_Platform;

//*************************************Functions*******************************************
void processDataGPS();
void platformThingsBoard();
void sendDataToThingsBoard();
void initModuleGSM800();
void startModuleGSM();
void thingsBoardCallback();
void timeDevice();
void clearDataStruct();
void statusLed(String module, bool status);
void listDir(char *dir);
void writeCreateLog(char *dir, String log);
void logCallback();
void readLog(char *dir);
bool checkLogFile(char *dir);
void initSPIFFS();
void appendContentFile(char *dir, String data);

//***********************************Tasks************************************************
Scheduler ts;

// Task t1(1 * TASK_SECOND, TASK_FOREVER, &task1Callback, &ts, false, NULL, &initModuleGSM800);
// Task t2(5 * TASK_SECOND, TASK_FOREVER, &task2Callback, &ts, false, NULL, &sendDataToThingsBoard);
// Task t1(2000, 10, &t1Callback, &ts, true);  //adding task to the chain on creation
// Task t2(3000, TASK_FOREVER, &t2Callback, &ts, true);  //adding task to the chain on creation
// Task t3(5000, TASK_FOREVER, &t3Callback);
Task taskThingsBoard(30 * TASK_SECOND, TASK_FOREVER, &thingsBoardCallback, &ts, true);
Task taskProcessGPS(1 * TASK_SECOND, TASK_FOREVER, &processDataGPS, &ts, true);
Task taskLog(30 * TASK_SECOND, TASK_FOREVER, &logCallback, &ts, true);

void setup()
{
  // Enable WDT
  esp_task_wdt_init(WDT_TIMEOUT, true); // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);               // add current thread to WDT watch

  debugSerial.begin(115200);

  // Init serial connect with Module GPS
  serialGPS.begin(GPSBaud);

  // Init module GSM sim800
  initModuleGSM800();

  // init SPIFFS on ESP32
  initSPIFFS();

  // Init led status
  pinMode(GSM_Led, OUTPUT);
  pinMode(GPS_Led, OUTPUT);
  pinMode(PWR_Led, OUTPUT);

  // ON led Power
  statusLed("Power", true);

  debugSerial.print(F("\r\nTinyGPSPlus library v. "));
  debugSerial.println(TinyGPSPlus::libraryVersion());

  // start FW
  debugSerial.printf("FW Version: %s\r\n", VER);

  // set point-in-time for scheduling start
  ts.startNow();
}

void loop()
{
  // Always revice data GPS from Serial
  while (serialGPS.available() > 0)
  {
    gps.encode(serialGPS.read());
  }

  // init TaskScheduler
  ts.execute();

  // Reset WDT
  esp_task_wdt_reset();
}

void processDataGPS()
{
  float value_Lat, value_Long, value_speed;

  debugSerial.print(F("Location: "));
  if (gps.location.isValid())
  {
    value_Lat = gps.location.lat();
    value_Long = gps.location.lng();

    // show debug data location
    debugSerial.print(value_Lat, 6);
    debugSerial.print(F(","));
    debugSerial.println(value_Long, 6);

    // add value lat, long to struct.
    Data_Platform.GPS_Lat = value_Lat;
    Data_Platform.GPS_Long = value_Long;

    // status led GPS
    statusLed("GPS", true);
    status_gps = true;
  }
  else
  {
    // status led GPS
    statusLed("GPS", false);
    status_gps = false;
    debugSerial.println(F("INVALID"));
  }

  debugSerial.print("\nSpeed: ");
  if (gps.speed.isValid())
  {
    // get speed km/h from gps data
    value_speed = gps.speed.kmph();

    // show debug value speed
    debugSerial.println(value_speed);

    // add value speed to struct
    Data_Platform.GPS_Speed = value_speed;
  }
  else
  {
    debugSerial.println(F("INVALID"));
  }

  if (year() < 2020)
  {
    timeDevice();
  }
  else
  {
    String Data_Time = String(String(hour()) + ":" + String(minute()) + ":" + String(second()) + " " + String(day()) + "/" + String(month()) + "/" + String(year()));
    Data_Platform.Date_Time = Data_Time;

    debugSerial.print("Date_Time: ");
    debugSerial.println(Data_Time);
  }
}

// platformThingsBoard platformIoT ThingsBoard
void platformThingsBoard()
{
  if (!tb.connected())
  {
    // Connect to the ThingsBoard
    debugSerial.print("Connecting to: ");
    debugSerial.print(THINGSBOARD_SERVER);
    debugSerial.print(" with token ");
    debugSerial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN))
    {
      debugSerial.println("Failed to connect ThingsBoard");
      count_connect_platformIoT++;
      return;
    }
  }

  // status GSM sending data
  statusLed("GSM", true);

  // send data to ThingsBoard
  sendDataToThingsBoard();

  // clear data after send data to ThingsBoard
  clearDataStruct();

  // status GSM finish send data
  statusLed("GSM", false);

  tb.loop();
}

// sendDataToThingsBoard send data to ThingsBoard Platform
// URL: https://demo.thingsboard.io/dashboards/b21a4c60-a264-11ec-83af-999932045501
void sendDataToThingsBoard()
{
  debugSerial.println("Sending data...");

  // Uploads new telemetry to ThingsBoard using MQTT.
  // See https://thingsboard.io/docs/reference/mqtt-api/#telemetry-upload-api
  // for more details
  if (Data_Platform.Date_Time != "")
  {
    tb.sendTelemetryString("DateTime", Data_Platform.Date_Time.c_str());
  }

  if (Data_Platform.GPS_Lat != 0 || Data_Platform.GPS_Long != 0 || Data_Platform.GPS_Speed != 0)
  {
    tb.sendTelemetryFloat("GPS_Lat", Data_Platform.GPS_Lat);
    tb.sendTelemetryFloat("GPS_Long", Data_Platform.GPS_Long);
    tb.sendTelemetryFloat("GPS_Speed", Data_Platform.GPS_Speed);
  }
}

// initModuleGSM800 init module GSM sim800
void initModuleGSM800()
{
  // Set GSM module baud rate
  // serialGsm.begin(115200);
  // delay(3000);

  // Lower baud rate of the modem.
  // This is highly practical for Uno board, since SoftwareSerial there
  // works too slow to receive a modem data.
  // serialGsm.write("AT+IPR=9600\r\n");
  // serialGsm.end();
  Serial2.begin(9600);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  debugSerial.println(F("Initializing modem..."));
  modem.restart();

  // Unlock your SIM card with a PIN if needed
  if (strlen(simPIN) && modem.getSimStatus() != 3)
  {
    modem.simUnlock(simPIN);
  }

  String modemInfo = modem.getModemInfo();
  debugSerial.print(F("Modem: "));
  debugSerial.println(modemInfo);
}

// startModuleGSM start module GSM
void startModuleGSM()
{
  debugSerial.print(F("Waiting for network..."));
  if (!modem.waitForNetwork(10000))
  {
    debugSerial.println(" fail");
    return;
  }
  debugSerial.println(" OK");

  debugSerial.print(F("Connecting to "));
  debugSerial.print(apn);

  if (!modem.gprsConnect(apn, user, pass))
  {
    debugSerial.println(" fail");
    return;
  }

  debugSerial.println(" OK");
}

// thingsBoardCallback ThingsBoard Callback
void thingsBoardCallback()
{
  if (modem.isNetworkConnected())
  {
    debugSerial.println("Network connected");
    status_gsm = true;

    if (count_connect_platformIoT == 5)
    {
      // restart module sim
      count_connect_platformIoT = 0;
      startModuleGSM();
    }
  }
  else
  {
    // Lost connect gsm
    status_gsm = false;

    // start and connect to GPRS
    startModuleGSM();
  }

  // send information to PlatformIoT
  platformThingsBoard();
}

// timeDevice use time data GPS setup for ESP
void timeDevice()
{
  bool date_valid = false;
  bool time_valid = false;

  debugSerial.print(F("Date/Time: "));
  if (gps.date.isValid())
  {
    debugSerial.print(gps.date.month());
    debugSerial.print(F("/"));
    debugSerial.print(gps.date.day());
    debugSerial.print(F("/"));
    debugSerial.print(gps.date.year());
    debugSerial.print(" ");

    date_valid = true;
  }
  else
  {
    debugSerial.println(F("INVALID"));
  }

  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10)
    {
      debugSerial.print(F("0"));
    }
    debugSerial.print(gps.time.hour());
    debugSerial.print(F(":"));

    if (gps.time.minute() < 10)
    {
      debugSerial.print(F("0"));
    }
    debugSerial.print(gps.time.minute());
    debugSerial.print(F(":"));

    if (gps.time.second() < 10)
    {
      debugSerial.print(F("0"));
    }
    debugSerial.print(gps.time.second());
    debugSerial.println(F("."));

    time_valid = true;
  }
  else
  {
    debugSerial.println(F("INVALID"));
  }

  // settime for ESP32 device
  if (date_valid && time_valid)
  {
    setTime(gps.time.hour() + 7, gps.time.minute(), gps.time.second(), gps.date.day(), gps.date.month(), gps.date.year());
  }
}

// clearDataStruct clear all data on struct
void clearDataStruct()
{
  Data_Platform.Date_Time = "";
  Data_Platform.GPS_Lat = 0;
  Data_Platform.GPS_Long = 0;
  Data_Platform.GPS_Speed = 0;
}

// statusLed led status
void statusLed(String module, bool status)
{
  if (module == "Power")
  {
    digitalWrite(PWR_Led, HIGH);
  }

  if (module == "GSM" && status)
  {
    digitalWrite(GSM_Led, HIGH);
  }
  else
  {
    digitalWrite(GSM_Led, LOW);
  }

  if (module == "GPS" && status)
  {
    digitalWrite(GPS_Led, HIGH);
  }
  else
  {
    digitalWrite(GPS_Led, LOW);
  }
}

// listDir list dir in SPIFFS
//  listDir("/") -> list all dir in SPIFFS
void listDir(char *dir)
{
  File root = SPIFFS.open(dir);

  File file = root.openNextFile();

  while (file)
  {
    debugSerial.print("FILE: ");
    debugSerial.println(file.name());

    file = root.openNextFile();
  }
}

// writeCreateLog create and write log file
void writeCreateLog(char *dir, String log)
{
  File file = SPIFFS.open(dir, FILE_WRITE);

  if (!file)
  {
    debugSerial.println("There was an error opening the file for writing");
    return;
  }
  if (file.print(log))
  {
    debugSerial.print("File was written: ");
    debugSerial.println(log);
  }
  else
  {
    debugSerial.println("File write failed");
    debugSerial.println(log);
  }

  file.close();
}

// logCallback log callback
void logCallback()
{
  String log_data;

  if (!status_gps || !status_gsm)
  {
    log_data = String(Data_Platform.Date_Time + "," + String(status_gps) + "," + String(Data_Platform.GPS_Lat, 2) + "," + String(Data_Platform.GPS_Long, 2) + "," + String(Data_Platform.GPS_Speed, 2) + "," + String(status_gsm));
    // debugSerial.print("log_data: ");
    // debugSerial.println(log_data);
  }
  else
  {
    return;
  }

  // write to log
  appendContentFile(log_path, log_data);
}

// readLog read log
void readLog(char *dir)
{
  File file = SPIFFS.open(dir);

  if (!file)
  {
    debugSerial.println("Failed to open file for reading");
    return;
  }

  debugSerial.println("File Content:");

  while (file.available())
  {
    debugSerial.write(file.read());
  }

  file.close();
}

// checkLogFile check file is exit
bool checkLogFile(char *dir)
{
  if (!SPIFFS.exists(dir))
  {
    debugSerial.println("File not exit.");
    return false;
  }
  debugSerial.println("File exit.");

  return true;
}

// initSPIFFS init, check, make and read file log
void initSPIFFS()
{
  long file_size;

  // init SPIFFS on ESP32
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  debugSerial.print("Total Bytes of SPIFFS: ");
  debugSerial.println(SPIFFS.totalBytes());

  // list all dir in SPIFFS
  listDir("/");

  // check file size
  file_size = SPIFFS.usedBytes();
  debugSerial.print("File Size: ");
  debugSerial.println(file_size);
  if (file_size >= 1000000)
  {
    // del file if file_size >= 1M (1 000 000 Bytes)
    debugSerial.println("Remove log file.");
    SPIFFS.remove(log_path);
  }

  // check file
  if (!checkLogFile(log_path))
  {
    // make log file
    writeCreateLog(log_path, "# Date, GPS_status, Lat, Long, Speed, GSM_status\r\n");
  }
  else
  {
    // read file if log file exit
    readLog(log_path);
  }
}

// appendContentFile append-content-to-file
void appendContentFile(char *dir, String data)
{
  File fileToAppend = SPIFFS.open(dir, FILE_APPEND);

  if (!fileToAppend)
  {
    debugSerial.println("There was an error opening the file for appending");
    return;
  }

  if (fileToAppend.println(data))
  {
    debugSerial.print("File content was appended: ");
    debugSerial.println(data);
  }
  else
  {
    debugSerial.println("File append failed");
    debugSerial.println(data);
  }

  fileToAppend.close();
}
