### Tham khao
- http://arduino.vn/bai-viet/833-lap-trinh-va-su-dung-modul-doc-rfid-rc522
- https://examples.blynk.cc/?board=ESP8266&shield=ESP8266%20WiFi&example=GettingStarted%2FVirtualPinWrite

### Library
- [RFID](https://github.com/miguelbalboa/rfid)
- [Blynk](https://github.com/blynkkk/blynk-library)

### Kết nối chân
MFRC522 Reader/PCD  |  Node MCU |
--------------------|-----------|
RST | D1 (GPIO5)
SDA(SS) | D2 (GPIO4)
MISO  | D6 (GPIO12)
MOSI | D7 (GPIO13)
SCK | D5 (GPIO14)
3.3V  | 3.3V
GND | GND
                 
### Mô tả
- Khi tap **đúng** thẻ RFID thì sẽ bật led ảo trên Blynk.
