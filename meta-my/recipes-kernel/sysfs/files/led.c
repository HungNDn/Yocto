#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/device.h>

#define LED_GPIO 21  // giả sử GPIO21 điều khiển LED

static struct class *led_class;
static struct device *led_device;

static int led_state = 0;


/* 
User-space:
   cat /sys/.../state
      ↓
   read(fd, user_buf, size)
      ↓

Kernel:
   Gọi state_show(..., buf)
      → bạn ghi: sprintf(buf, "%d\n", led_state);
   Kernel copy buf → user_buf

   sysfs
Bạn không thấy copy_to_user() hoặc copy_from_user(), vì:
Kernel là người quản lý toàn bộ buffer giữa kernel ↔ user
Bạn chỉ ghi/đọc từ char *buf được truyền vào
Kernel sẽ tự lo việc copy từ/to user-space sau đó
Bạn chỉ cần ghi nội dung là xong.

*/
static ssize_t state_show(struct device *dev,
                          struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", led_state);
}

static ssize_t state_store(struct device *dev,
                           struct device_attribute *attr,
                           const char *buf, size_t count)
{
    int val;
    if (kstrtoint(buf, 10, &val) < 0)
        return -EINVAL;

    led_state = !!val;  // chỉ lấy 0 hoặc 1
    gpio_set_value(LED_GPIO, led_state);

    return count;
}

// Tạo attribute "state"
static DEVICE_ATTR_RW(state);

/*
#define __ATTR_RW(_name) \
    __ATTR(_name, 0664, _name##_show, _name##_store)

static DEVICE_ATTR_RW(state);
=
struct device_attribute dev_attr_state = {
    .attr = {
        .name = "state",
        .mode = 0664,
    },
    .show  = state_show,
    .store = state_store,
};
/sys/class/led_ctrl/led0/mode  
/sys/class/led_ctrl/led0/brightness
mode_show, mode_store, rồi gọi DEVICE_ATTR_RW(mode)

*/ 

static int __init led_init(void)
{
    int ret;

    // Xuất GPIO
    ret = gpio_request(LED_GPIO, "led_gpio");
    if (ret)
        return ret;

    gpio_direction_output(LED_GPIO, 0);

    // Tạo class
    led_class = class_create(THIS_MODULE, "led_ctrl");
    if (IS_ERR(led_class))
        return PTR_ERR(led_class);

    // Tạo device /sys/class/led_ctrl/led0
    led_device = device_create(led_class, NULL, 0, NULL, "led0");
    if (IS_ERR(led_device))
        return PTR_ERR(led_device);

    // Gắn attribute vào sysfs
    ret = device_create_file(led_device, &dev_attr_state);
    if (ret)
        return ret;

    pr_info("LED driver with sysfs loaded\n");
    return 0;
}

static void __exit led_exit(void)
{
    device_remove_file(led_device, &dev_attr_state);
    device_destroy(led_class, 0);
    class_destroy(led_class);
    gpio_free(LED_GPIO);
    pr_info("LED driver removed\n");
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
