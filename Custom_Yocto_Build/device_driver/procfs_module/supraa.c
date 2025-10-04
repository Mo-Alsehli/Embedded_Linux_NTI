#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

/**
 * @brief Function prototypes
 */
static int supraa_open(struct inode *ino, struct file *filp);
static ssize_t supraa_write(struct file *filp, const char __user *buf, size_t size, loff_t *off);
static ssize_t supraa_read(struct file *filp, char __user *buf, size_t size, loff_t *off);

/**
 * @brief  Variables
 */
static struct proc_ops pops = {
    .proc_open = supraa_open,
    .proc_write = supraa_write,
    .proc_read = supraa_read,
};

static struct proc_dir_entry *pdir;
static char data[4] = {0};

/**
 * @brief  Functions
 */
static int supraa_open(struct inode *ino, struct file *filp) {
    printk("Supraaa on fire\n");
    return 0;
}

static ssize_t supraa_write(struct file *filp, const char __user *user, size_t size, loff_t *off) {
    if (copy_from_user(data, user, 4)) return -EFAULT;

    printk("Supraaa drifts: %c\n", data[0]);
    return 4;
}

static ssize_t supraa_read(struct file *filp, char __user *user, size_t size, loff_t *off) {
    if (copy_to_user(user, data, 4)) return -EFAULT;

    printk("It's A SUPRAAAAAAA\n");
    return 4;
}

static int __init supraa_module_init_func(void) {
    printk("Hello from the Supraa init module\n");
    pdir = proc_create("supraa", 0666, NULL, &pops);
    return 0;
}

static void __exit supraa_module_deinit_func(void) {
    printk("Hello from Supraa deinit module\n");
    proc_remove(pdir);
}

module_init(supraa_module_init_func);
module_exit(supraa_module_deinit_func);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("magdi");
