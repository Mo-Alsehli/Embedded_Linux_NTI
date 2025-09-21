# U-Boot Final Guide

This document covers the final steps of working with **U-Boot** for booting an operating system image.
It explains how to load images over the network (via **TFTP** and **NFS**), automate the boot process using **extlinux.conf**, and set up the **SD card partitions and files**.

---

## 1. Loading Images Over the Network

### 1.1 TFTP (Trivial File Transfer Protocol)

* **Purpose**: TFTP allows U-Boot to fetch kernel images, device trees, and initramfs files from a remote server over the network.
* **Protocol details**:

  * Operates at the **Application Layer** of the OSI model.
  * Uses **UDP** as the transport protocol.
  * Default port: **69**.

#### Steps to use TFTP with U-Boot:

1. Enable **TFTP support** in U-Boot (`CONFIG_CMD_TFTP` in menuconfig).
2. On the host PC:

   ```bash
   sudo apt update
   sudo apt install tftpd-hpa
   ```

   * Default TFTP root: `/srv/tftp`.
   * Copy kernel (`Image`), device tree (`.dtb`), and initramfs there.
3. Configure U-Boot networking:

   ```bash
   setenv ipaddr 192.168.1.100      # Target board IP
   setenv serverip 192.168.1.10     # TFTP server IP
   ```
4. Fetch and boot:

   ```bash
   tftp ${loadaddr} Image
   tftp ${fdt_addr_r} board.dtb
   booti ${loadaddr} - ${fdt_addr_r}
   ```

---

### 1.2 NFS (Network File System)

* **Purpose**: Mounts the **root filesystem** directly over the network.
* This avoids writing a full rootfs onto the SD card.

#### Steps to use NFS:

1. On host (Ubuntu):

   ```bash
   sudo apt install nfs-kernel-server
   ```
2. Edit `/etc/exports`:

   ```
   /srv/nfs/rootfs  *(rw,nohide,insecure,no_subtree_check,async,no_root_squash)
   ```
3. Restart service:

   ```bash
   sudo systemctl restart nfs-kernel-server
   ```
4. In U-Boot:

   ```bash
   setenv bootargs "root=/dev/nfs nfsroot=192.168.1.10:/srv/nfs/rootfs,v3 rw ip=dhcp"
   ```
5. Boot:

   ```bash
   tftp ${loadaddr} Image
   tftp ${fdt_addr_r} board.dtb
   booti ${loadaddr} - ${fdt_addr_r}
   ```

---

## 2. Automating Image Load with `extlinux.conf`

### 2.1 Setup

1. On the **boot partition** of the SD card:

   ```bash
   mkdir extlinux
   ```
2. Create `extlinux.conf`.

### 2.2 Example Configurations

#### Yocto image:

```conf
default Yocto
label Yocto
    kernel /Image
    fdt /board.dtb
    append root=PARTUUID=e170178b-02 rootwait console=ttyS0,115200
```

#### Local SD boot:

```conf
default localboot
timeout 11

label localboot
    kernel /Image
    fdt /bcm2710-rpi-3-b.dtb
    append root=/dev/mmcblk0p2 rw rootwait console=ttyS0,115200
```

### 2.3 Usage

* Multiple entries supported (Yocto, Debian, Recovery, etc).
* Run in U-Boot:

  ```bash
  bootflow scan
  ```

---

## 3. SD Card Partition Layout

For U-Boot + Linux boot, the SD card is typically split into **two partitions**:

### Partition Table

| Partition       | Filesystem | Purpose                                        | Example Mount |
| --------------- | ---------- | ---------------------------------------------- | ------------- |
| **p1** (boot)   | **FAT32**  | Holds U-Boot, kernel, DTB, and `extlinux.conf` | `/boot`       |
| **p2** (rootfs) | **EXT4**   | Contains the Linux root filesystem             | `/`           |

---

### Partition 1 (Boot Partition – FAT32)

Files required here:

```
/boot
 ├── u-boot.bin             # U-Boot bootloader
 ├── Image                  # Linux kernel image
 ├── board.dtb              # Device tree blob
 ├── extlinux/
 │    └── extlinux.conf     # Boot configuration
```

Notes:

* Kernel name can vary (`zImage`, `uImage`, or `Image` depending on config).
* If using initramfs, add it here as well.

---

### Partition 2 (Root Filesystem – EXT4)

Contains the actual root filesystem:

```
/
 ├── bin/          # User binaries
 ├── etc/          # Config files
 ├── lib/          # Libraries
 ├── sbin/         # System binaries
 ├── usr/          # Applications, libraries
 ├── var/          # Logs, spool
 ├── dev/          # Device nodes (populated by udev/mdev)
 ├── proc/         # Kernel virtual filesystem
 ├── sys/          # Kernel sysfs
 └── home/         # User home directories
```

---

## 4. Boot Flow Recap

1. **Boot ROM** loads U-Boot SPL.
2. **U-Boot SPL** initializes hardware, loads U-Boot proper.
3. **U-Boot proper** executes boot scripts:

   * Loads kernel + dtb via SD / TFTP.
   * Mounts rootfs via EXT4 (SD) or NFS (network).
4. **Kernel** mounts rootfs and executes `init`.

---

## 5. Troubleshooting

* **TFTP fails** → Check server firewall, IPs, and file paths.
* **NFS fails** → Run `showmount -e` on host to verify export.
* **extlinux not detected** → Ensure file is at `/boot/extlinux/extlinux.conf` on FAT32 partition.
* **Kernel panic: no rootfs** → Verify `root=` bootarg matches device or NFS path.

---

✅ With this setup, your board can boot:

* Fully from **SD card** (boot + rootfs).
* **Hybrid** (Kernel/DTB via TFTP, rootfs local).
* Fully over **network** (Kernel/DTB via TFTP + rootfs via NFS).
