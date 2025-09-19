# 🖥️ Emulating U-Boot & Linux Kernel with QEMU

QEMU is a **machine emulator** that lets you run U-Boot, Linux kernels, and full operating systems **without needing physical hardware**.

Since **Raspberry Pi 3B is not fully supported in QEMU**, we emulate a similar ARM platform: **`vexpress-a9`**.
This platform is widely supported by QEMU, U-Boot, and the Linux kernel — making it perfect for development and testing.

---

## 1️⃣ Install QEMU

On Ubuntu/Debian:

```bash
sudo apt update
sudo apt install qemu-system-arm qemu-system-aarch64 qemu-utils
```

* `qemu-system-arm` → ARM 32-bit system emulator
* `qemu-system-aarch64` → ARM 64-bit system emulator
* `qemu-utils` → Includes `qemu-img` (for creating virtual disks)

---

## 2️⃣ Build U-Boot for QEMU (`vexpress-a9`)

### Step 1: Clone U-Boot

```bash
git clone https://source.denx.de/u-boot/u-boot.git
cd u-boot
```

### Step 2: Configure for `vexpress-a9`

```bash
export ARCH=arm
export CROSS_COMPILE=~/x-tools/arm-none-eabi/bin/arm-none-eabi-
make vexpress_ca9x4_defconfig
```

* `ARCH=arm` → Build for 32-bit ARM
* `vexpress_ca9x4_defconfig` → Loads a config that matches QEMU’s machine

### Step 3: Build

```bash
make -j$(nproc)
```

This generates `u-boot` (an ELF binary).

---

## 3️⃣ Build a Linux Kernel for QEMU

### Step 1: Clone Kernel

```bash
git clone https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git
cd linux
```

### Step 2: Configure

```bash
export ARCH=arm
export CROSS_COMPILE=~/x-tools/arm-none-eabi/bin/arm-none-eabi-
make vexpress_defconfig
```

### Step 3: Build

```bash
make zImage -j$(nproc)
make dtbs
```

Outputs:

* `arch/arm/boot/zImage` → Kernel
* `arch/arm/boot/dts/vexpress-v2p-ca9.dtb` → Device Tree

---

## 4️⃣ Run U-Boot in QEMU

```bash
qemu-system-arm \
  -M vexpress-a9 \
  -m 256M \
  -nographic \
  -kernel u-boot
```

If successful, you should see a U-Boot prompt:

```
U-Boot> 
```

---

## 5️⃣ Boot Linux Kernel in QEMU

Boot directly (bypassing U-Boot):

```bash
qemu-system-arm \
  -M vexpress-a9 \
  -m 256M \
  -nographic \
  -kernel arch/arm/boot/zImage \
  -dtb arch/arm/boot/dts/vexpress-v2p-ca9.dtb \
  -append "console=ttyAMA0 root=/dev/ram"
```

---

## 6️⃣ (Optional) Add a Root Filesystem

1. Create a minimal rootfs (Buildroot or BusyBox).

2. Pack as initramfs:

   ```bash
   find . | cpio -H newc -o > rootfs.cpio
   ```

3. Boot with it:

   ```bash
   qemu-system-arm \
     -M vexpress-a9 \
     -m 256M \
     -nographic \
     -kernel arch/arm/boot/zImage \
     -dtb arch/arm/boot/dts/vexpress-v2p-ca9.dtb \
     -initrd rootfs.cpio \
     -append "console=ttyAMA0 root=/dev/ram rdinit=/bin/sh"
   ```

You’ll drop into a shell inside QEMU.

---

## 7️⃣ Creating and Using a Virtual SD Card

Instead of using only initramfs, you can create a **virtual SD card image** that acts like a real block device.

### Step 1: Create a Raw Disk Image

```bash
qemu-img create -f raw sdcard.img 128M
# we can make it without qemu
dd if=/dev/zero of=sdcard.img bs=1M count=128

```

This creates a **128MB empty disk**.

### Step 2: Partition and Format It

Use `fdisk` or `parted`:

```bash
sudo fdisk sdcard.img
```

Inside `fdisk`:

* `o` → create DOS partition table
* `n` → create primary partition (use defaults)
* `w` → write and exit

Then associate with a loop device:

```bash
sudo losetup -fP sdcard.img
losetup -a
```

Suppose it’s `/dev/loop0`. Format the first partition:

```bash
sudo mkfs.ext4 /dev/loop0p1
```

Mount it to copy files:

```bash
sudo mount /dev/loop0p1 /mnt
sudo cp rootfs/* /mnt/
sudo umount /mnt
sudo losetup -d /dev/loop0
```

### Step 3: Run QEMU with Virtual SD

```bash
qemu-system-arm \
  -M vexpress-a9 \
  -m 256M \
  -nographic \
  -kernel arch/arm/boot/zImage \
  -dtb arch/arm/boot/dts/vexpress-v2p-ca9.dtb \
  -append "console=ttyAMA0 root=/dev/mmcblk0p1 rw rootwait" \
  -drive file=sdcard.img,if=sd,format=raw
```

Now Linux will see `sdcard.img` as `/dev/mmcblk0`.

---

## 8️⃣ Useful QEMU Tips

* **Exit QEMU** (when using `-nographic`): `Ctrl+A` then `X`
* **Debugging:** Add `-S -s` to wait for GDB connection
* **Automation:** Wrap QEMU commands in `run_qemu.sh` for faster testing

---

✅ **With This Setup, You Can:**

* Run U-Boot and interact with it
* Boot Linux kernels
* Use a real rootfs (via initramfs or virtual SD card)
* Test device drivers and bootloader scripts without physical hardware

