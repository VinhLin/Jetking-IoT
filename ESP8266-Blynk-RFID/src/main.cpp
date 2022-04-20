#include <Arduino.h>

#define BLYNK_TEMPLATE_ID "TMPL5f25MxRB"
#define BLYNK_DEVICE_NAME "ESP8266Led1"
#define BLYNK_AUTH_TOKEN "FJzC-FEvbGfZ4s90znfMFaiG61Zk6OXj"

#define BLYNK_FIRMWARE_VERSION "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// #include "BlynkEdgent.h"
#include <BlynkSimpleEsp8266.h>

#include <SPI.h>
#include <MFRC522.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "FAI'S HUB L1";
char pass[] = "Fptacademy@2021";

#define RST_PIN 5 // D4 Configurable, see typical pin layout above
#define SS_PIN 4  //  D0 Configurable, see typical pin layout above
// MFRC522 mfrc522(RST_PIN, SS_PIN);        // Create MFRC522 instance.
MFRC522 mfrc522(SS_PIN, RST_PIN);

String cards[20];                   // Lưu 20 UID thẻ được phép vào
String users[20];                   //  Lưu số thứ tự user tương ứng
int virtualPin[] = {0, 0, 0, 0, 0}; // Trạng thái các nút V21, V22, V23, V24

int elock = D1;
int motionSensor = D2;
int Buzzer = D8;

int accessFailed = 0;
int sizeCards = 2; // khai báo số lượng card

WidgetTerminal terminal(V3); // Chân log hiện thị trên app
BlynkTimer timer;

void rfid();
void openOnApp();
void clearTag();
void motion();
BLYNK_WRITE(V21);
BLYNK_WRITE(V22);
BLYNK_WRITE(V23);
BLYNK_WRITE(V24);
BLYNK_WRITE(V25);
void initWifi();

void setup()
{

  Serial.begin(9600);

  initWifi();
  // BlynkEdgent.begin();
  Blynk.config(auth);

  pinMode(elock, OUTPUT);
  digitalWrite(elock, LOW);
  SPI.begin();                       // Init SPI bus
  mfrc522.PCD_Init();                // Init MFRC522 card
  mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  timer.setInterval(500L, rfid);
  timer.setInterval(200L, motion);
  pinMode(motionSensor, INPUT);
}

void loop()
{
  bool Connected2Blynk = Blynk.connected();
  if (!Connected2Blynk)
  {
    Blynk.connect(3000);
    return;
  }

  timer.run(); // Initiates SimpleTimer
  Blynk.run();
}

void rfid()
{

  boolean doorOpen = false;           // Trạng thái cửa
  String content = "";                // UID card quẹt vào
  boolean checkExistingCards = false; // Kiểm tra sự tồn tại của card
  clearTag();
  openOnApp();
  if (mfrc522.PICC_IsNewCardPresent())
  {
    if (mfrc522.PICC_ReadCardSerial())
    {
      for (byte i = 0; i < mfrc522.uid.size; ++i)
      {
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], DEC);
      }
      content.toUpperCase();

      for (int i = 0; i < sizeof(cards) / sizeof(cards[0]); i++)
      {
        if (content == cards[i] && virtualPin[0] == 0 && virtualPin[1] == 0) // So sánh thẻ nhập vào với thẻ được lưu và kiểm tra có đang khóa thẻ hay không
        {
          terminal.println(users[i] + " accessed");
          terminal.flush();
          tone(Buzzer, 2000, 200);
          digitalWrite(elock, HIGH);
          timer.setTimeout(5000L, []()
                           { digitalWrite(elock, LOW); });
          // delay(5000);
          // digitalWrite(elock, LOW);
          doorOpen = true;
          accessFailed = 0;
        }
        if (content == cards[i] && !checkExistingCards && virtualPin[1] == 1)
        { // Kiểm tra thẻ nhập vào có đang tồn tại hay không và kiểm tra chức năng thêm thẻ có mở không
          checkExistingCards = true;
          terminal.println("Card exist - " + users[i]);
          terminal.flush();
          tone(Buzzer, 2000, 600);
        }
      }

      if (content != 0 && virtualPin[1] == 1 && !checkExistingCards)
      { // nếu có thẻ nhập vào, chức năng thêm thẻ mở và thẻ không tồn tại thì thực hiện thêm thẻ
        if (sizeCards == 0)
        {
          sizeCards = 0;
        }

        cards[sizeCards] = content;
        users[sizeCards] = "User" + String(sizeCards);
        terminal.println("Register successed");
        terminal.flush();
        tone(Buzzer, 2000, 800);
        content = "";
        sizeCards++;
      }

      if (!doorOpen && content != 0 && virtualPin[0] == 0 && virtualPin[1] == 0)
      { // Nếu thẻ không khớp với các thẻ được lưu thực hiện lệnh
        terminal.println("access denied:" + content);
        terminal.flush();
        tone(Buzzer, 2000, 1000);
        accessFailed += 1;
        if (accessFailed > 3)
        { // Mở sai trên 3 lần tự động khóa & thông báo trên điện thoại
          virtualPin[0] = 1;
          Blynk.virtualWrite(V21, 1);
          Blynk.notify("Someones tried to access your home!!");
          tone(Buzzer, 2000, 5000);
        }
        doorOpen = !doorOpen;
      }
    }
  }
}

void openOnApp()
{
  if (virtualPin[3] == 1) // Mở khóa bằng app
  {
    terminal.println("Open by app");
    terminal.flush();
    tone(Buzzer, 2000, 200);
    digitalWrite(elock, HIGH);
    timer.setTimeout(5000L, []()
                     { digitalWrite(elock, LOW); });
    // delay(100000L);
    // digitalWrite(elock, LOW);
    virtualPin[3] = 0;
    Blynk.virtualWrite(V24, 0);
  }
}

void clearTag()
{ // Xóa tất cả các thẻ
  if (virtualPin[2] == 1)
  {
    tone(Buzzer, 2000, 200);
    memset(cards, 0, sizeof(cards)); // Xóa tất cả các UID
    memset(users, 0, sizeof(users)); // Xóa tất cả các user
    terminal.println("Clear successed");
    terminal.flush();
    sizeCards = 0;
    virtualPin[2] = 0;
    Blynk.virtualWrite(V23, 0);
  }
}

void motion()
{
  int checkAlarm = digitalRead(motionSensor);
  Serial.print(checkAlarm);
  if (checkAlarm == 1 && virtualPin[4] == 1)
  {
    Blynk.notify("Someones accessed your home!!");
    tone(Buzzer, 2000, 2000);
  }
  checkAlarm = 0;
}

BLYNK_WRITE(V21)
{
  virtualPin[0] = param.asInt();
}

BLYNK_WRITE(V22)
{
  virtualPin[1] = param.asInt();
}

BLYNK_WRITE(V23)
{
  virtualPin[2] = param.asInt();
}

BLYNK_WRITE(V24)
{
  virtualPin[3] = param.asInt();
}
BLYNK_WRITE(V25)
{
  virtualPin[4] = param.asInt();
}

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