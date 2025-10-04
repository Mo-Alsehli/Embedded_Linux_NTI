# Device Driver

## Overview

A **device driver** is a special type of kernel module that enables the Linux kernel to communicate with and control hardware peripherals.
It serves as a **bridge between hardware and software**, translating user-space requests into hardware-specific operations.

In Linux, unlike traditional embedded systems, drivers are designed to be **generic** and **data-structure based**, allowing them to handle many similar devices through standardized interfaces.

---

## 1. What Is a Driver?

* A **driver** is **software that controls hardware** (a peripheral) through well-defined interfaces.
* In **bare-metal embedded systems**, a driver is **specific to one device** — it directly manipulates hardware registers.
* In **Linux**, a driver is **generic** — it uses **data structures** to describe the hardware and communicates with it via **kernel frameworks**.

### Example

* Bare-metal: One driver for one sensor, tightly coupled to the MCU.
* Linux: One driver for many sensors of the same type (e.g., GPIO, I²C, SPI) — each sensor instance is defined via a **device tree entry** or **platform data**.

---

## 2. What Is a Device (Peripheral)?

* A **device** represents the **physical hardware component** — like a GPIO controller, UART, SPI device, etc.
* The **device driver** is the **software** that makes this hardware usable by the OS.

**Example Flow**

1. The hardware vendor provides the **datasheet** (register map, addresses, control bits).
2. The kernel developer writes a driver that uses those registers.
3. The driver is compiled as a **kernel module** (`.ko` file).
4. When inserted, it registers itself with the kernel as the controller for that device.

---

## 3. Creating a Device Driver

### Step 1 — Create the Source File

Start with a simple driver file, e.g.:

```
device.c
```

### Step 2 — Include Kernel Headers

You need to include the kernel headers that define module behavior and hardware access:

```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>          // For ioremap()
#include <linux/proc_fs.h>     // For /proc filesystem
```

### Step 3 — Define Register Base Addresses

Each peripheral in SoC (System-on-Chip) has specific **register addresses**.
For Raspberry Pi, GPIO registers are memory-mapped and accessed through physical addresses.

---

## 4. Writing a GPIO Driver Using `/proc` in Raspberry Pi

### Objective

We will write a driver that:

* Maps GPIO memory addresses into kernel space.
* Creates an entry in `/proc` (e.g. `/proc/gpio27`).
* Allows writing to that file to toggle the GPIO pin.

---

### Step-by-Step Implementation

#### 1. Define GPIO Base Address

```c
#define GPIO_BASE_ADDR 0x3F200000  // Base for RPi3 GPIO
#define GPIO_SIZE      0xB4        // Size of GPIO register block
```

#### 2. Declare Pointers for Registers

```c
static void __iomem *gpio_base; // Kernel-space pointer to GPIO registers
```

#### 3. Initialize with `ioremap()`

`ioremap()` tells the kernel that this physical address belongs to hardware I/O and maps it into the kernel's virtual address space.

```c
gpio_base = ioremap(GPIO_BASE_ADDR, GPIO_SIZE);
if (!gpio_base) {
    printk(KERN_ALERT "Failed to remap GPIO base\n");
    return -ENOMEM;
}
```

This ensures safe access to the GPIO hardware from within the kernel.

---

#### 4. Creating a `/proc` Entry

We use the `/proc` filesystem to communicate between user space and the kernel module.

```c
struct proc_dir_entry *proc_file;

proc_file = proc_create("gpio27", 0666, NULL, &gpio_fops);
if (!proc_file) {
    printk(KERN_ALERT "Error creating /proc entry\n");
    return -ENOMEM;
}
```

---

#### 5. Define File Operations

We define read/write callbacks for user interaction:

```c
static ssize_t gpio_write(struct file *file, const char __user *buf,
                          size_t count, loff_t *ppos)
{
    char cmd;
    if (copy_from_user(&cmd, buf, 1))
        return -EFAULT;

    if (cmd == '1') {
        // Set GPIO pin ON
    } else if (cmd == '0') {
        // Set GPIO pin OFF
    }

    return count;
}

static const struct proc_ops gpio_fops = {
    .proc_write = gpio_write,
};
```

Now, from user space:

```bash
echo "1" > /proc/gpio27   # Turn ON LED
echo "0" > /proc/gpio27   # Turn OFF LED
```

---

#### 6. Module Init and Exit Functions

```c
static int __init gpio_init(void)
{
    gpio_base = ioremap(GPIO_BASE_ADDR, GPIO_SIZE);
    if (!gpio_base) return -ENOMEM;

    proc_create("gpio27", 0666, NULL, &gpio_fops);
    printk(KERN_INFO "GPIO Driver loaded\n");
    return 0;
}

static void __exit gpio_exit(void)
{
    remove_proc_entry("gpio27", NULL);
    iounmap(gpio_base);
    printk(KERN_INFO "GPIO Driver removed\n");
}

module_init(gpio_init);
module_exit(gpio_exit);
MODULE_LICENSE("GPL");
```

---

## 5. Summary

| Concept               | Description                                                       |
| --------------------- | ----------------------------------------------------------------- |
| **Driver**            | Software that interfaces with hardware peripherals.               |
| **Device**            | The actual hardware component (chip or module).                   |
| **Generic Driver**    | Driver written to handle multiple devices using structured data.  |
| **`ioremap()`**       | Maps physical hardware registers to kernel virtual address space. |
| **`/proc` Interface** | Provides a simple way for user space to read/write kernel data.   |
| **Static Functions**  | Used for encapsulation — only the kernel calls them.              |

---

## Final Notes

* In **bare-metal**, the driver directly manipulates registers.
* In **Linux**, the driver **abstracts hardware** and exposes it through **kernel APIs** or **/proc/sysfs interfaces**.
* `ioremap()` is critical for mapping hardware safely into kernel space.
* Each Linux driver follows the same pattern:
  **Header Includes → Register Mapping → File Interface → Init/Exit Functions.**
