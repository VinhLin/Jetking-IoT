## Hardware
- Thiết bị sử dụng nguồn đầu vào acquy 12VDC.
- Khi cấp nguồn 12VDC, thiết bị sẽ sạc cho các **cell pin Lipo** bên trong.
- Đồng thời output ra trong khoảng 9 -> 12VDC.
- Áp output sẽ cung cấp được dùng cho:
	+ Module Uno ESP32
	+ Module Sim800
- Còn 2 module GPS và module Led sẽ dùng nguồn 5VDC từ **uno ESP32**.
- Các trạng thái LED:

LED	|	Mô tả		|
--------|-----------------------|
Đỏ	| Cho biết thiết bị đang hoạt động |
Vàng	| Mỗi khi thiết bị **gửi** data, thì led Vàng sẽ **chớp** một cái |
xanh	| Khi thiết bị đã **bắt được GPS** thì sẽ luôn sáng led xanh | 

- Khi ngắt điện áp đầu vào, thiết bị sẽ lấy nguồn từ các *cell pin* để tiếp tục hoạt động.

### Cách đấu chân tín hiệu của Module

Module LED	|	Uno ESP32	|
----------------|-----------------------|
Led Đỏ (R)	|	GPIO 5	|
Led Vàng (Y)	|	GPIO 13	|
Led Xanh (G)	|	GPIO 12	|

Module GPS	|	Uno ESP32	|
----------------|-----------------------|
RX	|	GPIO 27	|
TX	|	GPIO 14	|
GND	|	GND	|

Module sim800	|	Uno ESP32	|
----------------|-----------------------|
RXD	|	GPIO 17	|
TXD	|	GPIO 16	|
GND	|	GND	|

------------------------------------------------------------------------------------------------
## Software
### Giai đoạn 1: 
- Module Uno ESP32 sẽ liên tục 1s lấy data GPS.
- Các thông tin GPS sẽ lấy bao gồm:
	+ Latitude 
	+ Longitude
	+ Date - Time
	+ Speed
- Khởi tạo module GSM (sim800) và connect với **ThingsBoard**.
- Cứ mỗi **30s** thì sẽ gửi data lên *ThingsBoard*.
- Đồng thời show ra các trạng thái led.
### Giai đoạn 2: LOG
- Sau mỗi **30s** thì sẽ check xem nếu bị mất connect **GPS** hoặc **GSM** thì sẽ ghi lại **log**.
- Format khi ghi log sẽ là: `# Date, GPS_status, Lat, Long, Speed, GSM_status`
- Khi **size của file lớn hơn 1MB** thì sẽ tự động xóa và tạo lại file mới.
- Thiết bị ESP32 sẽ phát ra wifi Access Point và khi connect sẽ là: 
```
 SSID: WifiAP_Log
 Pass: 123456789
```
- Ví dụ IP khi connect với *WifiAP_Log* là **192.168.4.1**. 
- Để thấy được log đã lưu trên ESP32 thì vào link: `http://192.168.4.1/log`.
