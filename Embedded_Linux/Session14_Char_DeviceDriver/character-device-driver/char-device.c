#include <linux/cdev.h>    // for creating a character device
#include <linux/device.h>  // correct header for class_create/device_create
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/string.h>   // for strlen
#include <linux/uaccess.h>  // for copy_to_user, copy_from_user

// https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html

/** Preprocessor region */

/** @brief Base number for the device */
#define DEVICE_IO_BASE_NUMBER 0u

/** @brief Number of devices with the same specs */
#define NUMBER_OF_DEVICES 1u

/** Prototypes for file operations */
ssize_t char_device_write(struct file *, const char __user *, size_t, loff_t *);
ssize_t char_device_read(struct file *, char __user *, size_t, loff_t *);

/** @brief Device number */
static dev_t device_number;

/** @brief Character device structure */
static struct cdev _cdev;

/** @brief Pointer to class structure for /dev auto creation */
static struct class *_class = NULL;

/** @brief File operations structure */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = char_device_read,
    .write = char_device_write,
};

/** @brief Write function implementation */
ssize_t char_device_write(struct file *file, const char __user *user_buf, size_t len, loff_t *offset) {
    char kbuf[64];
    if (len > sizeof(kbuf) - 1) len = sizeof(kbuf) - 1;

    if (copy_from_user(kbuf, user_buf, len)) return -EFAULT;

    kbuf[len] = '\0';
    printk(KERN_INFO "char_device: Received from user: %s\n", kbuf);
    return len;
}

/** @brief Read function implementation */
ssize_t char_device_read(struct file *file, char __user *user_buf, size_t len, loff_t *offset) {
    const char msg[] = "Hello from kernel space!\n";
    size_t msg_len = strlen(msg);

    if (*offset >= msg_len) return 0;

    if (len > msg_len - *offset) len = msg_len - *offset;

    if (copy_to_user(user_buf, msg + *offset, len)) return -EFAULT;

    *offset += len;
    return len;
}

/** @brief Module init function */
static int __init char_device_init(void) {
    int ret;

    ret = alloc_chrdev_region(&device_number, DEVICE_IO_BASE_NUMBER, NUMBER_OF_DEVICES, "char_device");
    if (ret < 0) {
        printk(KERN_ERR "Failed to allocate char device region\n");
        return ret;
    }

    cdev_init(&_cdev, &fops);
    _cdev.owner = THIS_MODULE;

    ret = cdev_add(&_cdev, device_number, NUMBER_OF_DEVICES);

    /** Create class under /sys/class/ */
    _class = class_create("char_device");

    /** Create device node under /dev/ */
    device_create(_class, NULL, device_number, NULL, "char_device");

    printk(KERN_INFO "char_device: Major=%d, Minor=%d\n", MAJOR(device_number), MINOR(device_number));

    return 0;
}

/** @brief Module exit function */
static void __exit char_device_exit(void) {
    device_destroy(_class, device_number);
    class_destroy(_class);
    cdev_del(&_cdev);
    unregister_chrdev_region(device_number, NUMBER_OF_DEVICES);
    printk(KERN_INFO "char_device: Unregistered successfully\n");
}

module_init(char_device_init);
module_exit(char_device_exit);

MODULE_LICENSE("GPL");  // Must be GPL if using classes
MODULE_AUTHOR("Mohamed Magdi");
MODULE_DESCRIPTION("Minimal character device driver example");
