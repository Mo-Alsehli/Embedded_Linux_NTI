# 🧩 Character Device Driver (Linux Kernel Module)

A **character device driver** allows user-space programs to communicate with kernel-space components through a special file in `/dev`.
This README provides a complete step-by-step guide to creating, building, inserting, testing, and removing a minimal Linux character device module.

---

## 🧱 Overview

In Linux, **character devices** transfer data as streams of bytes (unlike block devices which use buffers).
Each character device is represented by a **device node** in `/dev`, identified by a **major** and **minor** number.

Our example driver implements basic `read()` and `write()` operations between user space and kernel space.

---

## ⚙️ 1️⃣ How to Create a Character Device

### **Step 1: Allocate Memory for the Device Number**

We start by reserving a **major/minor number pair** using:

```c
alloc_chrdev_region(&device_number, 0, NUMBER_OF_DEVICES, "char_device");
```

* **Major number** → identifies the driver.
* **Minor number** → identifies the specific device instance.

You can list registered devices:

```bash
cat /proc/devices
```

---

### **Step 2: Create Data Structure for Character Device**

The kernel uses `struct cdev` to represent a character device.
We initialize and add it using:

```c
cdev_init(&_cdev, &fops);
cdev_add(&_cdev, device_number, NUMBER_OF_DEVICES);
```

This structure connects our driver’s file operations (`read`, `write`) to the kernel’s file interface.

---

### **Step 3: Create Device Node (`mknod`)**

After inserting the module, the kernel prints the assigned major/minor numbers (e.g. 240, 0).
You must create a device node manually to communicate with the driver.

Check numbers:

```bash
dmesg | tail
```

Example output:

```
char_device: Major=240, Minor=0
```

Then create the device file:

```bash
sudo mknod /dev/char_device c 240 0
sudo chmod 666 /dev/char_device
```

> 🔹 `c` = character device
> 🔹 `240` = major number
> 🔹 `0` = minor number
> 🔹 `chmod` allows user read/write access

Now the device is ready for testing via `/dev/char_device`.

---

### **Step 4: (Optional) Create Class and Device**

To automate `/dev` node creation without `mknod`, you can add:

```c
cls = class_create(THIS_MODULE, "char_class");
device_create(cls, NULL, device_number, NULL, "char_device");
```

And on cleanup:

```c
device_destroy(cls, device_number);
class_destroy(cls);
```

This will automatically generate `/dev/char_device` on module load.

---

### **Step 5: Remove Device on Exit**

When unloading the module, clean up resources:

```c
cdev_del(&_cdev);
unregister_chrdev_region(device_number, NUMBER_OF_DEVICES);
```

If a class/device was created:

```c
device_destroy(cls, device_number);
class_destroy(cls);
```

---

## 🧰 2️⃣ Building the Module

### **Makefile**

Create a `Makefile` alongside your source code:

```makefile
obj-m += char_device.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
```

This tells the kernel build system to compile your module against the running kernel headers.

---

### **Build Commands**

Run:

```bash
make
```

Expected output:

```
make -C /lib/modules/6.x.x/build M=/home/user/char_device modules
```

A new file will appear:

```
char_device.ko
```

---

## 🚀 3️⃣ Testing the Driver

### **Insert the Module**

```bash
sudo insmod char_device.ko
```

Check messages:

```bash
dmesg | tail
```

Output:

```
char_device: Major=240, Minor=0
```

---

### **Create the Node**

```bash
sudo mknod /dev/char_device c 240 0
sudo chmod 666 /dev/char_device
```

---

### **Write to the Device**

```bash
echo "Hello kernel" > /dev/char_device
```

Kernel log:

```bash
dmesg | tail
```

Output:

```
char_device: Received from user: Hello kernel
```

---

### **Read from the Device**

```bash
cat /dev/char_device
```

Output on terminal:

```
Hello from kernel space!
```

---

### **Unload the Module**

```bash
sudo rmmod char_device
dmesg | tail
```

Output:

```
char_device: Unregistered successfully
```

Clean the build:

```bash
make clean
```

---

## 🧪 Example Full Session

```bash
$ make
$ sudo insmod char_device.ko
$ dmesg | tail
char_device: Major=240, Minor=0

$ sudo mknod /dev/char_device c 240 0
$ sudo chmod 666 /dev/char_device

$ echo "Hi Kernel" > /dev/char_device
$ dmesg | tail
char_device: Received from user: Hi Kernel

$ cat /dev/char_device
Hello from kernel space!

$ sudo rmmod char_device
$ dmesg | tail
char_device: Unregistered successfully
```

---

## 📘 Summary Table

| Step | Purpose                        | Key Function                               |
| ---- | ------------------------------ | ------------------------------------------ |
| 1    | Allocate device number         | `alloc_chrdev_region()`                    |
| 2    | Initialize cdev                | `cdev_init()`, `cdev_add()`                |
| 3    | Create device node             | `mknod /dev/char_device c <major> <minor>` |
| 4    | (Optional) Create class/device | `class_create()`, `device_create()`        |
| 5    | Clean up                       | `cdev_del()`, `unregister_chrdev_region()` |

---

## 🧑‍💻 Notes

* You can check kernel logs anytime with `dmesg`.
* Make sure kernel headers are installed (`sudo apt install linux-headers-$(uname -r)`).
* Always remove the module before rebuilding it: `sudo rmmod char_device`.
* Use `lsmod | grep char_device` to check if the module is loaded.

---

**Author:** Mohamed Magdi
**License:** GPL
**Description:** Minimal Linux Character Device Driver Example

