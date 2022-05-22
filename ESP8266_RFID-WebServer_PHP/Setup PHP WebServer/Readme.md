### Note
- Tải và cài đặt tool [XAMPP](https://sourceforge.net/projects/xampp/files/latest/download)
- Sau khi đã cài đặt thì mình giải nén file **rfidattendance.rar**, và copy file đã giải nén đến đường dẫn thư mục:
`C:\>xampp/htdocs/`
- Tiếp đến là mở link: `http://localhost/phpmyadmin/` hoặc check ip của máy và thay thế *localhost* thành ip của laptop.
- Sau đó là add **rfidattendance.sql** theo hướng dẫn.

### Thiết lập
- Vào địa chỉ *http://localhost/rfidattendance/*
- Login:
```
email= admin@gmail.com
password= 123
```
- Sau đó **Add new device**, copy **DE.UID** vào code ESP8266.
- Ngoài ra nhớ nhập đúng đường link địa chỉ của *php webserver* này trong code.
- Nhấn **Attendance** để khởi động thực thi chương trình của Webserver. Lúc này khi mình tap thẻ RFID thì nó sẽ cho mình biết thời gian login và logout.




