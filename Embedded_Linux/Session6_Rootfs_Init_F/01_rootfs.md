# 📂 Root File System Setup for Raspberry Pi (with BusyBox)

This guide explains how to create a minimal Linux root filesystem (rootfs) for Raspberry Pi (or any embedded Linux system) using **BusyBox**, link the required libraries, and run the new image.

---

## 🎯 Goal

We want our image to support **all common Linux commands**.
Instead of manually downloading, compiling, and installing every single command from the **GNU project**, we will use **BusyBox**.

---

## 🧰 Why BusyBox?

Normally, each command (like `ls`, `cp`, `cat`, etc.) comes from separate GNU packages. Installing them one by one for an embedded system is inefficient.
BusyBox solves this problem by providing **all essential UNIX utilities** in a single small executable, perfect for embedded systems.

✅ **Benefits of BusyBox:**

* Lightweight (small binary size)
* Includes most commonly used Linux commands
* Fully configurable — you can enable only the commands you need
* Easier to cross-compile for ARM / embedded targets

---

## 📥 Clone & Build BusyBox

Follow these steps to build BusyBox for your target system:

```bash
# Clone BusyBox (using the stable branch)
git clone -b 1_36_stable https://github.com/mirror/busybox.git --depth=1

# Install required libraries (needed for menuconfig and building)
sudo apt install libtirpc-dev
sudo apt install libncurses5-dev libncursesw5-dev ncurses-dev libncurses-dev

cd busybox

# Set your cross-compiler for Raspberry Pi (example)
export CROSS_COMPILE=<rpi-cross-compiler-path>/bin/aarch64-linux-gnu-

# Configure BusyBox
make defconfig       # Use default configuration
make menuconfig      # Optional: customize which commands/utilities you want to include

# Build BusyBox
make

# Install BusyBox into a temporary directory (_install)
make install
```

After running `make install`, a folder named **`_install/`** will be created.
This folder represents the **minimal root filesystem structure** containing BusyBox and its symlinks to commands like `ls`, `cat`, etc.

---

## 📂 Copy BusyBox to Your Root Filesystem

Use `rsync` to copy everything inside `_install` to your actual rootfs:

```bash
sudo rsync -avr _install/ /<path-to-rootfs>/
```

> ⚠ **Note:** Make sure your rootfs folder is mounted correctly (for example, the second partition of the SD card for Raspberry Pi).

---

## 🏗 Adding Required Libraries

BusyBox commands are **dynamically linked** (they rely on shared libraries such as `libc.so`, `ld-linux-aarch64.so.1`, etc.).

You must copy the required libraries from your **cross-compiler sysroot** into your rootfs.

Typical location of libraries:

```bash
<cross-compiler>/aarch64-linux-gnu/libc/lib
<cross-compiler>/aarch64-linux-gnu/libc/lib64
```

Copy them into `/lib` and `/lib64` in your rootfs:

```bash
sudo rsync -avr <cross-compiler>/aarch64-linux-gnu/libc/lib/  <path-to-rootfs>/lib/
sudo rsync -avr <cross-compiler>/aarch64-linux-gnu/libc/lib64/ <path-to-rootfs>/lib64/
```

> 📌 **Tip:** Run `file busybox` inside `_install/bin/` to confirm which architecture and loader it uses.
> You can also use `readelf -d busybox` to see which shared libraries are required.

---

## 🚀 Running the New Image

Once BusyBox and libraries are copied, you can boot your board with this rootfs.
The **`init` process** (PID 1) will now be BusyBox's shell (`/bin/ash`).

That means you should have:

```
/sbin/init  --> symlink or binary pointing to busybox
/bin/ash    --> shell provided by busybox
```

---

## 📊 Enabling `/proc` for Process Information

The `/proc` filesystem is a virtual filesystem that shows running processes and kernel information.

Steps to enable it:

1. Create a `/proc` directory in your rootfs:

```bash
mkdir -p <path-to-rootfs>/proc
```

2. Once the system boots, mount it manually:

```bash
mount -t proc none /proc
```

Now you can use commands like `ps`, `top`, and inspect `/proc/cpuinfo`, `/proc/meminfo`, etc.

> 🔧 To make this automatic, add the following line to `/etc/fstab` in your rootfs:

```
proc    /proc   proc    defaults    0   0
```

---

## ✅ Summary

* **BusyBox** is used to provide core Linux commands in a small binary.
* Build it using your cross-compiler, install it to `_install/`, and copy to rootfs.
* Copy required **shared libraries** from cross-toolchain sysroot.
* Set `/bin/ash` as your `init` process.
* Create and mount `/proc` for process visibility.

With this, you have a **functional root filesystem** that can boot and provide a working shell environment.


load mmc 0:1 $kernel_addr_r Image
load mmc 0:1 $fdt_addr_r bcm2710-rpi-3-b.dtb
booti $kernel_addr_r - $fdt_addr_r