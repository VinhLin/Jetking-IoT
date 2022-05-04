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
**TODO**


