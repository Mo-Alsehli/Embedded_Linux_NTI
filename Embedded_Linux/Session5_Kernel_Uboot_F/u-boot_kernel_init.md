# 🖥️ Booting Linux Kernel with U-Boot

This document explains how U-Boot loads and starts the Linux kernel, the role of the `init` process, and how to automate the boot process with environment variables.

---

## 🔑 Understanding the `root` User in Linux

* **Root User = User-Space Superuser**
  The `root` user is the highest-privileged user **inside user-space**.
* **Kernel is Still the Boss**
  Even `root` cannot bypass restrictions enforced by the kernel.
  For example, if the kernel denies access to hardware or a system call, `root` cannot override it.
* **Root’s Existence**

  * The kernel is responsible for creating the `root` user during system initialization.
  * Every Linux distribution **must have a `root` user**, with a **UID of 0**.

> 💡 **Key Takeaway:**
> `root` is *not* the "true god" of the system — the kernel is. The kernel enforces rules, and even root is bound by them.

---

## ⚙️ The `init` Process

The `init` process is the very first program executed by the kernel after it switches from **kernel-space** to **user-space**.

* **Purpose:**
  It sets up and runs everything else in user-space (services, daemons, shells, etc.).
* **Created by the Kernel:**
  The kernel itself spawns the `init` process, either from a hardcoded default or from what you specify in the `bootargs`.
* **Configurable:**
  You can tell the kernel which program to run as `init` using the `init=` kernel parameter.

Example:

```bash
init=/sbin/init
```

or even:

```bash
init=/bin/sh
```

(to directly boot into a shell).

---

## 🖧 Kernel Boot Arguments (bootargs)

When U-Boot loads the kernel, it can pass **kernel command line arguments** via the `bootargs` environment variable.

Common `bootargs` keywords:

* **`init=...`** → Specifies which program will run as PID 1 (the init process).
* **`root=...`** → Specifies the root filesystem device (e.g., `/dev/mmcblk0p2`).
* **`rootfstype=...`** → Specifies the filesystem type (e.g., `ext4`).
* **`rw`** → Mounts the root filesystem in read-write mode.
* **`rootwait`** -> Wait till file system is mounted.
* `8250.nr_uarts=1 console=ttyS0,115200n8` -> uart options to show the shell.

Example bootargs:

```bash
setenv bootargs "root=/dev/mmcblk0p2 rw rootfstype=ext4 8250.nr_uarts=1 console=ttyS0,115200n8 init=/dash rootwait"
```

---

## 📜 Kernel Boot Sequence with U-Boot

Here’s the **step-by-step process** of loading and running the kernel from U-Boot:

1. **Load Kernel Image**

   ```bash
   load mmc 0:1 $kernel_addr_r Image
   ```

   Loads the Linux kernel (`Image`) from the first partition of the SD card into RAM.

2. **Load Device Tree Blob (DTB)**

   ```bash
   load mmc 0:1 $fdt_addr_r my_device_tree.dtb
   ```

   Loads the DTB, which contains hardware configuration.

3. **Set Kernel Boot Arguments**

```bash
  setenv bootargs "root=/dev/mmcblk0p2 rw rootfstype=ext4 8250.nr_uarts=1 console=ttyS0,115200n8 init=/dash rootwait"
```

   Passes arguments to the kernel to tell it which init process to run and where the root filesystem is.

4. **Boot the Kernel**

   ```bash
   booti $kernel_addr_r - $fdt_addr_r
   ```

   Boots the kernel using the loaded image and device tree.

---

## 🏗️ Automating the Boot Process in U-Boot

Manually typing all commands each time can be tedious. U-Boot allows automation through environment variables:

```bash
# 1. Define commands
setenv loadkernel "load mmc 0:1 $kernel_addr_r Image"
setenv loaddtb "load mmc 0:1 $fdt_addr_r my_device_tree.dtb"
setenv bootargs "root=/dev/mmcblk0p2 rw rootfstype=ext4 8250.nr_uarts=1 console=ttyS0,115200n8 init=/dash rootwait"
setenv bootimage "booti $kernel_addr_r - $fdt_addr_r"

# 2. Combine them into a single script
setenv bootscript "run loadkernel; run loaddtb; run setbootargs; run bootimage"

# 3. Make it run automatically at power-on
setenv bootcmd "run bootscript"

# 4. Save environment to make changes persistent
saveenv
```

> 📝 **Note:**
> `bootcmd` is a **special U-Boot variable** that executes automatically at boot. By setting it to run our script, we achieve fully automated kernel booting.

---

## 🎯 Task: Use `dash` as Your Init Process

We want to make the **init process** a simple POSIX shell using [dash](https://github.com/danishprakash/dash).

Steps:
Here’s a cleanly regenerated **Markdown version** of your text, with proper formatting and clear structure:

## 2️⃣ Set U-Boot Environment Variables

Boot the Raspberry Pi and stop at the **U-Boot prompt**.  
Then configure `bootargs` to run `dash` as `init`:

```bash
setenv bootargs "root=/dev/mmcblk0p2 rw rootfstype=ext4 8250.nr_uarts=1 console=ttyS0,115200n8 init=/dash rootwait"
```

**Explanation of `bootargs`:**

* `console=serial0,115200` → Enables console on Raspberry Pi UART (serial0) at 115200 baud.
* `root=/dev/mmcblk0p2` → Uses the second SD card partition as the root filesystem.
* `rw rootfstype=ext4` → Mounts the root partition as read/write using `ext4`.
* `init=/bin/dash` → Replaces the default `init` process with `dash`.

---

## 3️⃣ Load Kernel and Device Tree

Still at the U-Boot prompt, load the kernel and device tree from the SD card:

```bash
load mmc 0:1 ${kernel_addr_r} Image
load mmc 0:1 ${fdt_addr_r} bcm2710-rpi-3-b.dtb
```

---

## 4️⃣ Boot the Kernel

Boot the system with:

```bash
booti ${kernel_addr_r} - ${fdt_addr_r}
```

---

## 5️⃣ (Optional) Automate with `bootcmd`

To avoid typing the commands manually on every boot, make them persistent:

```bash
setenv bootcmd 'load mmc 0:1 ${kernel_addr_r} Image; load mmc 0:1 ${fdt_addr_r} bcm2710-rpi-3-b.dtb; booti ${kernel_addr_r} - ${fdt_addr_r}'
saveenv
```

This saves the `bootcmd` to U-Boot’s environment, so it runs automatically on every reset.

---

## ✅ Result

When the Raspberry Pi boots:

* The kernel mounts `/dev/mmcblk0p2` as the root filesystem.
* It directly runs `/bin/dash` as PID 1 (instead of `init` or `systemd`).

You will drop straight into a **minimal `dash` shell** — no `systemd`, no `init`, just a raw shell environment.



This will boot you **directly into the dash shell** as PID 1.
