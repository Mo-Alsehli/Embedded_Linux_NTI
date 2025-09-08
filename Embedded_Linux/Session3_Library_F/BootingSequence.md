# Booting Sequence

When building an embedded Linux distribution from scratch, we need to understand the **boot process**.
The sequence typically follows this flow:

```
Boot ROM / Firmware  ⇒  Bootloader  ⇒  Kernel  ⇒  Init Process  ⇒  Shell
```

⚠️ Note: The exact sequence **varies between platforms** (PCs, development boards, microcontrollers, Raspberry Pi, etc.).

---

## Building Our Own Distribution (High-Level Steps)

1. **Build a cross-toolchain** for the target board.
2. **Provide boot components**:

   * Bootloader (e.g., U-Boot).
   * Linux Kernel (zImage / Image).
   * Device Tree Blob (DTB).
   * Root filesystem (Init + userland).
3. **Follow the boot sequence** outlined below.

---

## Booting Sequence Phases

### **Phase 1 – Boot ROM / Firmware Chip**

* Executed immediately after power-on.
* Contains **OTP (One-Time Programmable) code** burned into SoC ROM at factory.
* Responsible for:

  * Basic **hardware initialization** (clock, RAM controller, stack pointer).
  * **DRAM initialization**.
  * Locating the **next-stage loader** (BIOS in PCs, proprietary firmware in dev boards).
  * Loading this code into DRAM and updating **PC (Program Counter)** to jump there.

---

### **Phase 2 – BIOS (PCs only, not on dev boards like Raspberry Pi)**

* **BIOS code execution** in DRAM.
* Tasks:

  * Detect storage device.
  * Locate the **boot image** (via **Master Boot Record – MBR**).
  * Verify boot partition.
  * Jump to the **bootloader**.

*(Modern PCs often use UEFI instead of BIOS, but the principle is the same.)*

---

### **Phase 3 – Bootloader (Common across all platforms)**

* Bootloader code is copied into DRAM.
* BIOS/firmware space in DRAM is freed.
* **Bootloader responsibilities**:

  * Load the **Linux kernel image** into DRAM.
  * Load the **Device Tree Blob (DTB)** describing hardware.
  * Optionally load an **initramfs**.
  * Pass control to the kernel.
  * Free its own memory.

**Examples**: U-Boot, Barebox, Coreboot (PCs).

---

### **Phase 4 – Kernel**

* Kernel is now in control.
* Tasks:

  * Initialize CPU, MMU, scheduler.
  * Detect and initialize drivers.
  * Mount the **root filesystem**.
  * Launch the **init process**.

---

### **Phase 5 – Init Process**

* **First user-space process** started by kernel (`/sbin/init` or alternatives like `systemd`, `busybox init`, `runit`).
* Responsibilities:

  * Set up environment variables.
  * Mount remaining filesystems.
  * Launch system services.
  * Finally, spawn a **login shell** (user interaction).

---

## Raspberry Pi Boot Sequence

Unlike a PC, the Raspberry Pi relies on its GPU (VideoCore IV) to bootstrap the system.

```
Boot ROM ⇒ bootcode.bin ⇒ start.elf ⇒ U-Boot (optional) ⇒ Kernel ⇒ Init
```

### Explanation

1. **Boot ROM (inside SoC)**

   * Executes on the GPU (not the ARM CPU).
   * Searches for **bootcode.bin** on the SD card FAT32 partition.

2. **bootcode.bin (closed source, vendor-provided)**

   * Initializes SDRAM.
   * Loads **start.elf** into memory.

3. **start.elf (GPU firmware, closed source)**

   * Runs on the GPU.
   * Loads `config.txt`, `fixup.dat`, and device tree blobs (`*.dtb`).
   * Prepares the ARM core(s).

4. **Bootloader (optional)**

   * You can replace the default kernel with a **bootloader like U-Boot** (`kernel8.img` on ARM64).
   * Provides more flexibility for debugging, loading custom kernels, NFS boot, etc.

5. **Kernel**

   * Linux kernel image (`kernel.img`, `kernel7.img`, or `kernel8.img` depending on architecture).
   * Device tree is passed to it.

6. **Init Process**

   * Kernel runs `/sbin/init` from the root filesystem.
   * Brings the system to multi-user mode, then login shell.