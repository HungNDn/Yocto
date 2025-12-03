#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define DEVICE_NAME "my_misc_dev"
#define BUFFER_SIZE 1024

static char* kernel_buffer = NULL;
static size_t buffer_size = 0;

static ssize_t my_read(struct file* file, char __user* user_buf, size_t count, loff_t *pos) 
{
    if(*pos >= buffer_size)
    {
        return 0;
    }

    if(count > buffer_size - *pos) 
    {
        count = buffer_size - *pos;
    }

    if(copy_to_user(user_buf, kernel_buffer + *pos, count))
        return -EFAULT;

        *pos += count;
    
    return count;
}

static ssize_t my_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    if (count > BUF_SIZE)
        count = BUF_SIZE;

    memset(kernel_buffer, 0, BUF_SIZE);
    buffer_size = count;

    if (copy_from_user(kernel_buffer, user_buf, count))
        return -EFAULT;

    return count;
}


struct file_operations my_fops =
{
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write
};

struct miscdevice my_misc_device = 
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = my_fops,
    .mode = 0666,
};

static int __init my_misc_init(void) 
{
    int ret = 0;

    kernel_buffer = kmalloc(BUF_SIZE, GFP_KERNEL);
    if(!kernel_buffer) 
        return -ENOMEM;
    
    ret = misc_register(&my_misc_device);

    if(ret) 
    {
        pr_err("Unable to register misc device\n");
        kfree(kernel_buffer);
        return ret;
    }  
    pr_info("misc device registered: %s\n", DEVICE_NAME);
    return ret;
}


static void __exit my_misc_exit(void)
{
    misc_deregister(&my_misc_device);
    kfree(kernel_buffer);
    pr_infor("misc device deregister\n";)
}

module_init(my_misc_init);
module_exit(my_misc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple misc device example");
