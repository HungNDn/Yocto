#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "simple_char"
#define CLASS_NAME  "simple_class"
#define BUFFER_SIZE 1024

static dev_t dev_num;
static struct cdev simple_cdev;
static struct class *char_class;
static struct device *char_device;

static char kernel_buffer[BUFFER_SIZE];
static size_t buffer_size = 0;

static int simple_char_open(struct inode *inode, struct file *file)
{
    pr_info("simple_char: device opened\n");
    return 0;
}

static int simple_char_release(struct inode *inode, struct file *file)
{
    pr_info("simple_char: device closed\n");
    return 0;
}

static ssize_t simple_char_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    if (*offset >= buffer_size)
        return 0;

    if (len > buffer_size - *offset)
        len = buffer_size - *offset;

    if (copy_to_user(buf, kernel_buffer + *offset, len))
        return -EFAULT;

    *offset += len;
    return len;
}

static ssize_t simple_char_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    if (len > BUFFER_SIZE - 1)
        len = BUFFER_SIZE - 1;

    // copy_from_user returns the number of bytes that were NOT copied.
    // copy_from_user(kernel_buffer, buf, len) != 0 ~ copy_from_user(kernel_buffer, buf, len)
    if (copy_from_user(kernel_buffer, buf, len)) 
        return -EFAULT;

    kernel_buffer[len] = '\0';
    buffer_size = len;

    pr_info("simple_char: stored string: %s\n", kernel_buffer);
    return len;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = simple_char_open,
    .release = simple_char_release,
    .read    = simple_char_read,
    .write   = simple_char_write,
};

static int __init simple_char_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("Failed to allocate device number\n");
        return ret;
    }

    pr_info("simple_char: registered with major %d\n", MAJOR(dev_num));

    char_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(char_class)) {
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(char_class);
    }

    char_device = device_create(char_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(char_device)) {
        class_destroy(char_class);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(char_device);
    }

    cdev_init(&simple_cdev, &fops);
    ret = cdev_add(&simple_cdev, dev_num, 1);
    if (ret < 0) {
        device_destroy(char_class, dev_num);
        class_destroy(char_class);
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    pr_info("simple_char: driver initialized\n");
    return 0;
}

static void __exit simple_char_exit(void)
{
    cdev_del(&simple_cdev);
    device_destroy(char_class, dev_num);
    class_destroy(char_class);
    unregister_chrdev_region(dev_num, 1);
    pr_info("simple_char: driver removed\n");
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hung Nghiem Dinh");
MODULE_DESCRIPTION("Simple char device with read/write support");
