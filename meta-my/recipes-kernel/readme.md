Điều khiển đơn giản, trạng thái	                    sysfs
Thông tin hệ thống, debug	                        procfs
Giao tiếp dữ liệu, buffer, read/write,              miscdevice hoặc cdev
Giao tiếp dạng stream, buffer, binary data	        miscdevice hoặc cdev
Nhiều thiết bị char phức tạp	                    cdev chuẩn
Một thiết bị char duy nhất	                        miscdevice


Điều khiển LED, Thay đổi ngưỡng/config, Bật/tắt thiết bị bằng sysfs (không dùng cho truyền dữ liệu lớn, thời gian thực, streaming, video)
echo 1 > /sys/class/led_ctrl/led0/state  # Bật LED
echo 0 > /sys/class/led_ctrl/led0/state  # Tắt LED
cat /sys/class/led_ctrl/led0/state       # Xem trạng thái hiện tại


Xuất thông tin thống kê/đếm/debug, Đọc trạng thái hệ thống bằng procfs (không dùng cho cấu hình hoặc điều khiển, Giao tiếp user-space phức tạp)
cat /proc/my_driver_info
=== My Driver Debug Info ===
Access count   : 1
Last accessed  : 4292812334 jiffies
Uptime (secs)  : 123
/proc/meminfo, /proc/cpuinfo, /proc/driver/nvidia/version.

1. ioctl – Gửi lệnh điều khiển đến driver
Đổi chế độ thiết bị (on/off)
Đặt tốc độ truyền
Lấy thông tin trạng thái cụ thể

Khi có nhiều loại lệnh cần truyền, không thể nhét hết vào read/write
Khi truyền struct, flag, option, enum

2. poll / select – Chờ dữ liệu hoặc sự kiện từ driver
Cho phép user-space đợi (non-blocking) cho đến khi:
- Thiết bị có dữ liệu mới
- Có thể read() hoặc write() mà không bị block
Dùng khi:
Bạn muốn event-driven thay vì vòng lặp read() tốn CPU
Phù hợp với driver sensor, thiết bị nhập (input), hoặc bất kỳ dữ liệu nào không liên tục

3. mmap() – Chia sẻ vùng nhớ giữa kernel và user-space
Dùng khi:
Bạn cần chia sẻ buffer lớn
Không muốn copy dữ liệu qua read()/write() (hiệu năng kém)
Dùng trong thiết bị camera, video, DMA, hay shared memory


