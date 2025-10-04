#include <linux/io.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#include "RPI_GPIO_Registers.h"

/**
 * @brief Preprocessor Section
 */

typedef enum { INPUT = 0u, OUTPUT = 1u } direction_enum;
#define STD_HIGH 1
#define STD_LOW 0

/**
 * @brief Function prototypes
 */
static int gpio_open(struct inode *ino, struct file *filp);
static ssize_t gpio_write(struct file *filp, const char __user *buf, size_t size, loff_t *off);
static ssize_t gpio_read(struct file *filp, char __user *buf, size_t size, loff_t *off);
static inline void gpio_set_pin_direction(int channel, direction_enum direction);
static inline void gpio_set_pin_value(int channel, int value);

/**
 * @brief  Variables
 */
static struct proc_ops pops = {
    .proc_open = gpio_open,
    .proc_write = gpio_write,
    .proc_read = gpio_read,
};

static struct proc_dir_entry *pdir = NULL;
void __iomem *gpio_base = NULL;

/**
 * @brief  Functions
 */

static inline void gpio_set_pin_direction(int channel, direction_enum direction) {
    if (channel == 27u) {
        volatile unsigned int *gpioDirection = (unsigned int *) (gpio_base + 0x08);  // GPFSEL2
        unsigned int shift = (channel % 10) * 3;
        unsigned int val = *gpioDirection;
        val &= ~(0x7 << shift);
        if (direction == OUTPUT) val |= (0x1 << shift);
        *gpioDirection = val;
    }
}

static inline void gpio_set_pin_value(int channel, int value) {
    if (channel == 27u) {
        if (value == STD_HIGH) {
            volatile unsigned int *gpioSet = (unsigned int *) (gpio_base + 0x1C);
            *gpioSet = (1 << channel);
        } else if (value == STD_LOW) {
            volatile unsigned int *gpioClear = (unsigned int *) (gpio_base + 0x28);
            *gpioClear = (1 << channel);
        }
    }
}

static int gpio_open(struct inode *ino, struct file *filp) { return 0; }

static ssize_t gpio_write(struct file *filp, const char __user *user, size_t size, loff_t *off) {
    char buf[10] = {0};
    int dir, val;

    if (copy_from_user(buf, user, size)) return -EFAULT;

    if (sscanf(buf, "%d %d", &dir, &val) != 2) {
        if (sscanf(buf, "%1d%1d", &dir, &val) != 2) return -EINVAL;
    }

    if (dir == OUTPUT)
        gpio_set_pin_direction(27, OUTPUT);
    else if (dir == INPUT)
        gpio_set_pin_direction(27, INPUT);

    if (val == STD_HIGH)
        gpio_set_pin_value(27, STD_HIGH);
    else if (val == STD_LOW)
        gpio_set_pin_value(27, STD_LOW);

    return size;
}

static ssize_t gpio_read(struct file *filp, char __user *user, size_t size, loff_t *off) { return 0; }

static int __init gpio_module_init_func(void) {
    gpio_base = ioremap(BCM2837_GPIO_BASE, BCM2837_GPIO_SIZE);
    if (!gpio_base) return -ENOMEM;

    pdir = proc_create("gpio27", 0666, NULL, &pops);
    if (!pdir) {
        iounmap(gpio_base);
        return -ENOMEM;
    }

    return 0;
}

static void __exit gpio_module_deinit_func(void) {
    proc_remove(pdir);
    if (gpio_base) iounmap(gpio_base);
}

module_init(gpio_module_init_func);
module_exit(gpio_module_deinit_func);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("magdi");
