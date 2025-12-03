#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>
#include <linux/uaccess.h>

#define PROC_NAME "my_driver_info"

static int access_count = 0;
static unsigned long last_jiffies = 0;

static int my_proc_show(struct seq_file *m, void *v)
{
    access_count++;
    last_jiffies = jiffies;

    seq_printf(m, "=== My Driver Debug Info ===\n");
    seq_printf(m, "Access count   : %d\n", access_count);
    seq_printf(m, "Last accessed  : %lu jiffies\n", last_jiffies);
    seq_printf(m, "Uptime (secs)  : %lu\n", jiffies_to_msecs(jiffies) / 1000);

    return 0;
}

static int my_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, my_proc_show, NULL);
}

static const struct proc_ops my_proc_ops = {
    .proc_open    = my_proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static int __init my_proc_init(void)
{
    proc_create(PROC_NAME, 0, NULL, &my_proc_ops);
    pr_info("/proc/%s created\n", PROC_NAME);
    return 0;
}

static void __exit my_proc_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    pr_info("/proc/%s removed\n", PROC_NAME);
}

module_init(my_proc_init);
module_exit(my_proc_exit);
MODULE_LICENSE("GPL");
