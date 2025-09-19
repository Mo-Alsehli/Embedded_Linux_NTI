# 🚀 U-Boot (Unified Bootloader)

U-Boot is a **powerful and flexible bootloader** used in embedded systems. It is responsible for **initializing hardware**, **loading images** (kernel, applications, bare-metal programs), and **transferring control** to them.

---

## 🔑 Key Features of U-Boot

* **Boots Applications** → Can boot Linux kernels, bare-metal programs, or other images.
* **Multi-Platform Support** → Works on many architectures (ARM, RISC-V, x86, PowerPC, etc.).
* **Privileged Execution** → U-Boot and the images it runs execute in **privileged (supervisor) mode**, giving full access to hardware.
* **RAM Partitioning** → When loading an image, RAM is conceptually divided into:

  * **BootROM region** → Used by U-Boot itself.
  * **Image region** → Where the kernel, application, or bare-metal program is loaded.

---

## 🖥️ Interacting with U-Boot

U-Boot provides a **command-line interface** (CLI) to configure and control the boot process. You can extend or reduce available commands based on your project needs.

### 🔹 Common Commands

* **Customizing Commands**
    - In Linux Repo on host

  * Run:

    ```bash
    make menuconfig
    ```
  * Navigate to: *Command line interface → Commands*.
  * You can **enable/disable commands** to reduce memory footprint or add functionality.

* **Help System (u-boot shell)**

  ```bash
  help
  ```

  Lists all supported commands available in the current U-Boot build.

* **System Information**

  ```bash
  bdinfo
  ```

  Displays board details such as:

  * IP address
  * MAC address
  * DDR RAM base address

* **Booting Images**

  * `bootz` → Boots a **zImage** (compressed Linux kernel).
  * `booti` → Boots an **Image** (uncompressed Linux kernel).

  > ⚠️ These commands must be enabled in `menuconfig`.

* **Bootflow System**

  ```bash
  bootflow
  ```

  Boots using `extlinux.conf` or other supported boot configuration files.

* **Running Applications**

  ```bash
  go <address>
  ```

  Jumps to a memory address and starts executing the code (bare-metal or kernel entry).

* **Environment Variables**

  * Temporary:

    ```bash
    setenv <var> <value>
    ```

    Sets an environment variable for the current session.
  * Permanent:

    ```bash
    saveenv
    ```

    Saves environment variables to persistent storage (flash/NVRAM).

* **Loading Images into Memory**

  ```bash
  load <interface> <device>:<partition> [address] <filename>
  ```

  Example:

  ```bash
  load mmc 0:1 0x80000 Image
  ```

  Loads `Image` from SD card (device 0, partition 1) into memory at `0x80000`.

* **Inspecting Memory**

  ```bash
  md <address>
  ```

  Dumps memory contents starting at a specific address.

* **Listing Files**

  ```bash
  ls <interface> <device>:<partition> <dir>
  ```

  Lists files inside a directory on the chosen device/partition.

---

## ⚡ Workflow Example: Booting a Linux Kernel

1. **Load Kernel Image into RAM**

   ```bash
   load mmc 0:1 $kernel_addr_r Image
   ```
2. **Load Device Tree**

   ```bash
   load mmc 0:1 $fdt_addr_r my_board.dtb
   ```
3. **Set Boot Arguments**

   ```bash
   setenv bootargs "console=ttyAMA0,115200 root=/dev/mmcblk0p2 rw rootfstype=ext4"
   ```
4. **Boot the Kernel**

   ```bash
   booti $kernel_addr_r - $fdt_addr_r
   ```
5. **Save Configuration (optional)**

   ```bash
   saveenv
   ```
