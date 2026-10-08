### Bài 1. Hoàn thiện mã nguồn
Hoàn thành tất cả các phần được đánh dấu TODO trong `week1\incompleted\scanner.c`

### Bài 2. Mở rộng bộ từ vựng của KPL
Dựa trên kết quả Bài 1, chỉnh sửa mã nguồn để bộ phân tích từ vựng nhận biết thêm các thành phần sau:
1.	Kiểu dữ liệu string: thêm từ khóa STRING và hằng xâu ký tự.
2.	Phép toán lấy phần dư: thêm toán tử %.
3.	Comment một dòng: chuỗi // bắt đầu một comment kéo dài đến hết dòng, giống C++.
4.	Kiểu dữ liệu bytes: thêm từ khóa BYTES.
5.	Phép toán lũy thừa **: ví dụ, 2**10 biểu diễn 210. Lưu ý: cần phân biệt ** với toán tử nhân *.
6.	Cấu trúc lặp REPEAT … UNTIL: thêm hai từ khóa REPEAT và UNTIL cho câu lệnh có dạng:
REPEAT <statement> UNTIL <condition>;
Yêu cầu chung: với mỗi thay đổi, cập nhật đủ các phần liên quan (khai báo token, bảng từ khóa, hàm in token) và viết chương trình KPL mẫu để kiểm thử
