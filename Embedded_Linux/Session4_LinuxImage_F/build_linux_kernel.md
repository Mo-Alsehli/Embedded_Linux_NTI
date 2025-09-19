# 🛠️ Building the Linux Kernel for Raspberry Pi (RPI)

This guide explains how to build the Linux kernel for Raspberry Pi using the official [raspberrypi/linux](https://github.com/raspberrypi/linux) repository and prepare it to boot with U-Boot.

---

## 📦 1. Clone the Raspberry Pi Linux Repository

Start by cloning the official Raspberry Pi kernel source:

```bash
git clone https://github.com/raspberrypi/linux.git
cd linux
```

This will give you the full Linux source tree tailored for Raspberry Pi.

---

## 🔧 2. Set Cross-Compiler & Architecture Variables

Since you are likely building on a PC (x86\_64) for Raspberry Pi (ARM64), you need a **cross-compiler**:

```bash
export ARCH=arm64
export CROSS_COMPILE=~/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-
```

* `ARCH=arm64` → Tells the kernel build system to target 64-bit ARM.
* `CROSS_COMPILE=<path>` → Specifies the prefix of the cross-compiler toolchain (e.g., `aarch64-rpi3-linux-gnu-gcc`).

> 💡 **Tip:**
> You can add these exports to your `~/.bashrc` or `~/.zshrc` to avoid retyping them every time.

---

## ⚙️ 3. Configure the Kernel for Raspberry Pi

Select the default configuration for Raspberry Pi 3 (64-bit):

```bash
make bcmrpi3_defconfig
```

This loads a default `.config` optimized for RPi3 hardware.

---

## 🏗️ 4. Build the Kernel Image

Now compile the kernel using all available CPU cores:

```bash
make -j$(nproc)
```

* `-j$(nproc)` uses all cores on your host machine to speed up the build.
* This produces the **kernel image** (`Image`), **device tree blobs** (`.dtb`), and **kernel modules**.

---

### 🛠️ Alternative Build Command

If the standard `make` fails, you can explicitly pass variables:

```bash
make ARCH=arm64 KERNEL=kernel8 
     CROSS_COMPILE=~/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu- 
     bcmrpi3_defconfig

make ARCH=arm64 KERNEL=kernel8 
     CROSS_COMPILE=~/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-
```

Here:

* `KERNEL=kernel8` ensures you build the **64-bit kernel** (kernel8.img).
* The `bcmrpi3_defconfig` step regenerates the `.config` before compiling.

---

## 💾 5. Deploy Kernel and Device Tree

After a successful build, copy the output files to your Raspberry Pi’s **boot partition** (FAT32):

1. **Kernel Image:**
   Copy `arch/arm64/boot/Image` to the boot partition.
2. **Device Tree Blobs (DTBs):**
   Copy relevant `.dtb` files from `arch/arm64/boot/dts/broadcom/` to the boot partition.

For example:

```bash
cp arch/arm64/boot/Image /media/$USER/boot/
cp arch/arm64/boot/dts/broadcom/*.dtb /media/$USER/boot/
```

---

## 🚀 6. Boot with U-Boot

If you are using U-Boot as a bootloader:

* **Load the Kernel Image:**

  ```bash
  load mmc 0:1 $kernel_addr_r Image
  ```
* **Load the Device Tree:**

  ```bash
  load mmc 0:1 $fdt_addr_r bcm2710-rpi-3-b.dtb
  ```
* **Set Boot Arguments:**

```bash
setenv bootargs "root=/dev/mmcblk0p2 rw rootfstype=ext4 8250.nr_uarts=1 console=ttyS0,115200n8 init=/dash rootwait"
```
* **Boot the Kernel:**

  ```bash
  booti $kernel_addr_r - $fdt_addr_r
  ```

At this point, your custom-built kernel should boot on the Raspberry Pi.
