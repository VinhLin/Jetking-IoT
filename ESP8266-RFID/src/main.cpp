#include <Arduino.h>

/*
 * Initial Author: ryand1011 (https://github.com/ryand1011)
 *
 * Reads data written by a program such as "rfid_write_personal_data.ino"
 *
 * See: https://github.com/miguelbalboa/rfid/tree/master/examples/rfid_write_personal_data
 *
 * Uses MIFARE RFID card using RFID-RC522 reader
 * Uses MFRC522 - Library
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

/*
    Typical pin layout used:
   ----------------------------------
               MFRC522      Node
               Reader/PCD   MCU
   Signal      Pin          Pin
   ----------------------------------
   RST/Reset   RST          D1 (GPIO5)
   SPI SS      SDA(SS)      D2 (GPIO4)

   SPI MISO    MISO         D6 (GPIO12)
   SPI MOSI    MOSI         D7 (GPIO13)
   SPI SCK     SCK          D5 (GPIO14)

   3.3V        3.3V         3.3V
   GND         GND          GND
*/

#define BLYNK_TEMPLATE_ID "TMPL9prfXMfd"
#define BLYNK_DEVICE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "FJzC-FEvbGfZ4s90znfMFaiG61Zk6OXj"

#include <SPI.h>
#include <MFRC522.h>
#include <BlynkSimpleEsp8266.h>

#define RST_PIN 5 // Configurable, see typical pin layout above
#define SS_PIN 4  // Configurable, see typical pin layout above

// Comment this out to disable prints and save space
// #define BLYNK_PRINT Serial_Bynk

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "FAI'S HUB L1";
char pass[] = "Fptacademy@2021";

String login_access = "a1300126";

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

void initWifi();
void blynkLed();
bool loginRFID(String login);

//*****************************************************************************************//
void setup()
{
  Serial.begin(9600); // Initialize serial communications with the PC

  // init wifi and connect blynk
  initWifi();
  Blynk.config(auth);

  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
}

//*****************************************************************************************//
void loop()
{
  bool Connected2Blynk = Blynk.connected();
  if (!Connected2Blynk)
  {
    Blynk.connect(3000);
    Blynk.virtualWrite(V4, 0);
    return;
  }

  bool login_success = loginRFID(login_access);
  if (login_success)
  {
    if (Connected2Blynk)
    {
      Serial.println("Connected to Blynk server");
      blynkLed();
    }
  }

  delay(1000); // change value if you want to read cards faster
}
//*****************************************************************************************//

void initWifi()
{
  // Connect to WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("*");
  }

  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP()); // Print the IP address
}

void blynkLed()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V4, millis() / 1000);
}

bool loginRFID(String login)
{
  String data_uid_rfid;

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return false;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return false;
  }

  Serial.println(F("**Card Detected:**"));
  Serial.println(F("Read personal data on a MIFARE PICC:")); // shows in serial that it is ready to read

  //-------------------------------------------
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); // dump some details about the card
  // mfrc522.PICC_DumpToSerial(&(mfrc522.uid));        // uncomment this to see all blocks in hex

  for (byte i = 0; i < 4; i++)
  {
    data_uid_rfid += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    data_uid_rfid += String(mfrc522.uid.uidByte[i], HEX);
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  data_uid_rfid.replace(" ", "");
  Serial.print("data_uid_rfid:");
  Serial.println(data_uid_rfid);

  if (data_uid_rfid == login)
  {
    Serial.println("RFID Access Success");

    return true;
  }

  return false;
}