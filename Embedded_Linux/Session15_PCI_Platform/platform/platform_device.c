#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>      // For ioremap/iounmap if needed
#include <linux/kernel.h>  // For printk
#include <linux/module.h>
#include <linux/platform_device.h>  // Added for platform_device structure

#include "RPI_GPIO_Registers.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Minimal platform device example");

/* -------------------------------------------------
 * Release Function
 * -------------------------------------------------*/
static void mydevice_release(struct device *dev) { printk(KERN_INFO "Safe shutdown for the device\n"); }

/* -------------------------------------------------
 * Private Device Data
 * -------------------------------------------------*/
struct private_device_data {
    uint32_t baseAddress;
    size_t regSize;
};

static struct private_device_data _mydevice = {.baseAddress = BCM2837_GPIO_BASE, .regSize = BCM2837_GPIO_SIZE};

/* -------------------------------------------------
 * Platform Device Declaration
 * -------------------------------------------------*/
static struct platform_device pltfrmDev[] = {
    {
        .name = "mydevice_synopsis",
        .id = 0,
        .dev =
            {
                .release = mydevice_release,
                .platform_data = &_mydevice,
            },
    },
    {} /* Terminator */
};

/* -------------------------------------------------
 * Init and Exit Functions
 * -------------------------------------------------*/
static int __init mydeviceInit(void) {
    int ret;
    printk(KERN_INFO "Registering mydevice platform device\n");

    ret = platform_device_register(&pltfrmDev[0]);
    if (ret) printk(KERN_ERR "Failed to register platform device: %d\n", ret);

    return ret;
}

static void __exit mydeviceExit(void) {
    printk(KERN_INFO "Unregistering mydevice platform device\n");
    platform_device_unregister(&pltfrmDev[0]);
}

/* -------------------------------------------------
 * Module Declarations
 * -------------------------------------------------*/
module_init(mydeviceInit);
module_exit(mydeviceExit);
