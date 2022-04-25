#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = "FAI'S HUB L1";
const char *password = "Fptacademy@2021";
const char *mqtt_server = "172.16.2.61";

WiFiClient espClient;
PubSubClient client(espClient);

void initWifi();
void callback(char *topic, byte *payload, unsigned int length);
void reconnect();

void setup()
{
  digitalWrite(LED_BUILTIN, HIGH);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  initWifi();
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

void initWifi()
{
  // Connect to WiFi
  WiFi.begin(ssid, password);
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

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1')
  {
    digitalWrite(BUILTIN_LED, LOW);
  }
  else
  {
    digitalWrite(BUILTIN_LED, HIGH);
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");

    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");

      // client.publish("VinhLD/outTopic", "hello world");
      // client.subscribe("VinhLD/inTopic");

      client.publish("Vinhld", "hello world");
      client.subscribe("Vinhld");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}