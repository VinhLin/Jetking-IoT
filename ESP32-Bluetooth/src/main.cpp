#include <Arduino.h>

#include "BluetoothSerial.h"

/* Check if Bluetooth configurations are enabled in the SDK */
/* If not, then you have to recompile the SDK */
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define ledPIN 2

BluetoothSerial SerialBT;

void setup()
{
  pinMode(ledPIN, OUTPUT);
  // digitalWrite(ledPIN, HIGH);

  Serial.begin(115200);
  /* If no name is given, default 'ESP32' is applied */
  /* If you want to give your own name to ESP32 Bluetooth device, then */
  /* specify the name as an argument SerialBT.begin("myESP32Bluetooth"); */
  SerialBT.begin("Test-ESP32_Bluetooth");
  Serial.println("Bluetooth Started! Ready to pair...");
}

void loop()
{
  String value_string;
  if (Serial.available())
  {
    SerialBT.write(Serial.read());
  }

  if (SerialBT.available())
  {
    Serial.write(SerialBT.read());

    value_string = SerialBT.readString();
    if (value_string == "1")
    {
      digitalWrite(ledPIN, HIGH);
    }
    else
    {
      digitalWrite(ledPIN, LOW);
    }
  }
  delay(20);
}
