#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = "FAI'S HUB L1";
const char *password = "Fptacademy@2021";
const char *mqtt_server = "172.16.2.61";

String clientId = "ClientESP8266";  // Client ID của mạch
const char *relay_topic = "Vinhld"; // Topic của relay

#define SW_1 D1
#define SW_2 D2
#define SW_3 D3
#define SW_4 D4

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi();
void callback(char *topic, byte *payload, unsigned int length);
void handlingData_Relay(byte *data);
void reconnect();
void pinModes();

void setup()
{
  pinMode(D0, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  /* Hàm start - read Callback client */
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}

void setup_wifi()
{
  delay(10);
  pinModes();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
/* Ham call back nhan lai du lieu */
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message read [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (strcmp(topic, relay_topic) == 0)
  {
    handlingData_Relay(payload);
  }
}
void handlingData_Relay(byte *data)
{
  switch ((char)data[0])
  {
  case '0':
    digitalWrite(SW_1, LOW);
    client.publish(relay_topic, "Relay 1 is ON");
    break;
  case '1':
    digitalWrite(SW_1, HIGH);
    client.publish(relay_topic, "Relay 1 is OFF");
    break;
  case '2':
    digitalWrite(SW_2, LOW);
    client.publish(relay_topic, "Relay 2 is ON");
    break;
  case '3':
    digitalWrite(SW_2, HIGH);
    client.publish(relay_topic, "Relay 2 is OFF");
    break;
  case '4':
    digitalWrite(SW_3, LOW);
    client.publish(relay_topic, "Relay 3 is ON");
    break;
  case '5':
    digitalWrite(SW_3, HIGH);
    client.publish(relay_topic, "Relay 3 is OFF");
    break;
  case '6':
    digitalWrite(SW_4, LOW);
    client.publish(relay_topic, "Relay 4 is ON");
    break;
  case '7':
    digitalWrite(SW_4, HIGH);
    client.publish(relay_topic, "Relay 4 is OFF");
    break;
  }
}
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      client.publish(relay_topic, "Reconnect"); // Gửi dữ liệu
      client.subscribe(relay_topic);            // Theo dõi dữ liệu
    }
    else
    {
      Serial.println("failed, rc=");
      Serial.print(client.state());
      // Doi 1s
      delay(1000);
    }
  }
}

void pinModes()
{
  pinMode(SW_1, OUTPUT);
  pinMode(SW_2, OUTPUT);
  pinMode(SW_3, OUTPUT);
  pinMode(SW_4, OUTPUT);

  digitalWrite(SW_1, HIGH);
  digitalWrite(SW_2, HIGH);
  digitalWrite(SW_3, HIGH);
  digitalWrite(SW_4, HIGH);
}