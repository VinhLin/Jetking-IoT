## Các library được dùng trong code
```
#include <SoftwareSerial.h>
#include <esp_task_wdt.h>
#include "ThingsBoard.h"
#include <TinyGsmClient.h>
#include <TinyGPSPlus.h>
#include <TaskScheduler.h>
#include <TimeLib.h>
#include "SPIFFS.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
```

### SoftwareSerial.h
- Thư viện này có chức năng **giả lập** tín hiệu UART.

### esp_task_wdt.h
- Thư viện Watchdog Timer (WDT).
- Có chức năng giúp thiết bị restart lại khi bị treo.

### ThingsBoard.h
- Thư viện dùng để truyền nhận data với Platform ThingsBoard.

### TinyGsmClient.h
- Thư viện giúp giao tiếp và khởi động module sim800.

### TinyGPSPlus.h
- Thư viện giúp xử lý data GPS.

### TaskScheduler.h
- Thư viện giúp định thời chạy function.

### TimeLib.h
- Thư viện giúp cập nhật thời gian cho ESP32

### SPIFFS.h
- Thư viện khởi tạo chức năng SPIFFS, mục đích là dùng để ghi lại **LOG**.

### WiFi.h
- Thư viện khởi tạo chức năng Wifi cho ESP32.

### AsyncTCP.h và ESPAsyncWebServer.h
- Khởi tạo webserver cho ESP32.