### Note
- Đồ án kết thúc học kì IoT Jetking.
- Đồ án sẽ là: **Hộp đen đơn giản cho xe máy**
```
 - Sử dụng module ESP32 làm MCU chính.
 - Sử dụng module GPS U-Blox NEO-7N để bắt data GPS.
 - Sử dụng module GSM GPRS SIM800A để connect và gửi data len ThingsBoard.
```

-------------------------------------------------------------------------------------------
### Link sản phẩm module tham khảo
- [Module ESP32](https://hshop.vn/products/kit-arduino-wifi-ble-soc-esp32-wemos-d1-r32)
- [Module GPS U-Blox NEO-7N](https://hshop.vn/products/mach-gps-ublox-neo-6m-v2)
- [Module GSM GPRS SIM800A](https://hshop.vn/products/mach-gsm-gprs-sim800a-tich-hop-nguon-xung-va-chuyen-muc-tin-hieu)

### Tài liệu GPS và chuẩn NMEA
- [Document NMEA](http://aprs.gids.nl/nmea/#rmc)
- [Thư viện Github đọc và xử lý data GPS](https://github.com/mikalhart/TinyGPSPlus)
- Sample code:
```
https://github.com/mikalhart/TinyGPSPlus/blob/master/examples/FullExample/FullExample.ino
https://github.com/mikalhart/TinyGPSPlus/blob/master/examples/UsingCustomFields/UsingCustomFields.ino
https://github.com/mikalhart/TinyGPSPlus/blob/master/examples/BasicExample/BasicExample.ino
```

### Tài liệu về GSM sim800
- [Datasheet Sim800](https://datasheetspdf.com/datasheet/SIM800.html)
- [AT command Sim800](https://cdn-shop.adafruit.com/datasheets/sim800_series_at_command_manual_v1.01.pdf)
- [Hướng dẫn sử dụng](https://vidieukhien.xyz/2018/03/24/sim-com-command-tut/)
- [Thư viện sử dụng cho Arduino (hoặc ESP32)](https://github.com/vshymanskyy/TinyGSM)

### ThingsBoard
- **ThingsBoard** nằm trong top các platform dành cho IoT có [độ phổ biến nhất](https://movan.vn/top-10-iot-platform-ma-nguon-mo-2020/), ngoài ra đây cũng là một platform có mã nguồn mở.
- [Docs Guides](https://thingsboard.io/docs/guides/)
- [Tài liệu Tiếng Việt](https://lophocvui.com/category/iot-internet-of-things/)

### ESPAsyncWebServer
- [Tham khao](https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library/)
- [Github library](https://github.com/esphome/ESPAsyncWebServer)
- https://github.com/esphome/ESPAsyncWebServer/blob/master/examples/simple_server/simple_server.ino
