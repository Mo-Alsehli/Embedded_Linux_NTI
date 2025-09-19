# Raspberry Pi Boot Process

* [Building OS from scratch scratch](https://www.rpi4os.com/)
> just for fun
* [baremetal programming and os dev](https://forums.raspberrypi.com/viewtopic.php?t=377875)

📖 References:

* [Raspberry Pi Documentation](https://www.raspberrypi.com/documentation/computers/processors.html)
* [Raspberry Pi SoC TRM (BCM2837)](https://cs140e.sergio.bz/docs/BCM2837-ARM-Peripherals.pdf)
* [Raspberry Pi Firmware Repository](https://github.com/raspberrypi/firmware/tree/master/boot)
* [U-Boot GitHub](https://github.com/u-boot/u-boot)

---

## Introduction

* **No eMMC on Raspberry Pi**

  > Unlike many embedded boards, the Raspberry Pi does not have on-board eMMC (embedded memory). Instead, it boots directly from the **microSD card**.

* **System-on-Chip (SoC)**

  * Raspberry Pi 3 uses **BCM2837** (sometimes referred to as BCM2710, essentially the same family).
  * The SoC includes:

    * ARM Cortex-A53 CPU (quad-core, 64-bit).
    * GPU (VideoCore IV).

* **Boot Requirements**

  During boot, the Raspberry Pi GPU executes the initial bootloader stored in ROM, which then loads required files from the **FAT32 boot partition** on the SD card:

  * `bootcode.bin` → First-stage bootloader (GPU-side).
  * `start.elf` → GPU firmware.
  * `fixup.dat` → GPU configuration data.
  * `config.txt` → User configuration (created manually).
  * `*.dtb` → Device Tree Blobs (e.g., `bcm2710-rpi-3-b.dtb`).
  * `overlays/` → Device Tree overlays for peripherals.

* **Firmware Files**

  These files can be downloaded from the [Raspberry Pi Firmware Repository](https://github.com/raspberrypi/firmware/tree/master/boot).

---

## Boot Sequence (Raspberry Pi 3)

1. **BootROM** (inside SoC)

   * GPU executes the bootcode stored in ROM.
   * Looks for `bootcode.bin` on the SD card (FAT32 partition).

2. **bootcode.bin**

   * Initializes SDRAM.
   * Loads `start.elf` (GPU firmware).

3. **start.elf & fixup.dat**

   * Configures memory split between ARM and GPU.
   * Loads kernel or U-Boot depending on configuration.

4. **Device Tree**

   * `.dtb` file passed to the kernel (or bootloader).

5. **Kernel / U-Boot**

   * If using default RPi firmware: directly boots `kernel7.img` or `kernel8.img`.
   * If using U-Boot: RPi firmware loads U-Boot as the “kernel” image, then U-Boot takes over booting the actual kernel.

---

## U-Boot

> **U-Boot (Universal Bootloader)** is an open-source, GPL-licensed bootloader widely used in embedded systems. It allows flexible boot management, loading kernels, device trees, and initramfs.

---

### Building U-Boot for Raspberry Pi 3

1. **Clone the repository**:

   ```bash
   git clone https://github.com/u-boot/u-boot.git
   cd u-boot
   ```

2. **Checkout a stable version** (optional):

   ```bash
   git checkout v2025.10-rc3
   ```

3. **Set up the cross-compiler** (for ARM64):

   ```bash
   export CROSS_COMPILE=aarch64-linux-gnu-
   export ARCH=arm
   ```

4. **Configure for Raspberry Pi 3**:

   ```bash
   make rpi_3_defconfig
   ```

5. **Build U-Boot**:

   ```bash
   make -j$(nproc)
   ```

6. **Generated Output**:

   * `u-boot.bin` → main U-Boot binary.

---

### Installing U-Boot on Raspberry Pi 3

1. **Prepare SD card partitions**

   * Partition 1: **FAT32 boot** (\~256MB).
   * Partition 2: **ext4 rootfs** (rest of space).

2. **Copy firmware files** (from Raspberry Pi firmware repo) into the boot partition:

   * `bootcode.bin`
   * `start.elf`
   * `fixup.dat`
   * `*.dtb`
   * `overlays/`
   * `u-boot.bin`
   * `config.txt`


4. **Edit `config.txt`** in the boot partition:

   ```ini
   kernel=u-boot.bin
   arm_64bit=1
   enable_uart=1
   ```

5. **Insert SD card and boot**

   * On power-up, U-Boot should appear on **serial console (UART)** or HDMI.
   * From U-Boot, you can load the Linux kernel (`Image`) and device tree manually or via `boot.scr`.

---

## Cross-Compiling and Running an Application

### 1. Install Cross Compiler (on Ubuntu host)

```bash
sudo apt update
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
```

---

### 2. Write a Simple Application

Create `hello.c`:

```c
#include <stdio.h>

int main() {
    printf("Hello from Raspberry Pi 3!\n");
    return 0;
}
```

---

### 3. Cross-Compile for Raspberry Pi

```bash
aarch64-linux-gnu-gcc hello.c -o hello_rpi
```

This generates a **64-bit ARM binary** (`hello_rpi`).

---

### 4. Transfer Binary to Raspberry Pi

* If SD card rootfs is mounted:

  ```bash
  cp hello_rpi /media/$USER/rootfs/home/pi/
  ```

* Or, if RPi is network-connected:

  ```bash
  scp hello_rpi pi@<RPI_IP>:/home/pi/
  ```

---

### 5. Run on Raspberry Pi

On Raspberry Pi terminal:

```bash
chmod +x hello_rpi
./hello_rpi
```

Output:

```
Hello from Raspberry Pi 3!
```

---

✅ At this point you have:

* Installed **U-Boot** as a bootloader.
* Cross-compiled and executed your **own application** on Raspberry Pi 3.

---

## Running A baremetal on raspi:

Perfect — now we’re getting into the **real difference between running U-Boot and running a “pure” bare-metal kernel.**

Right now you are essentially booting into **U-Boot’s own environment**, which is like a very small monitor program.
If you wanted to run *your own* bare-metal kernel, here’s what would be different:

---

### 1. **What Happens Now (With U-Boot)**

Your setup:

* **bootcode.bin → start.elf → fixup.dat**: Raspberry Pi firmware stage.
* **u-boot.bin** (renamed as `kernel8.img`): loaded by the firmware.
* U-Boot runs, initializes DRAM, sets up clocks, brings you to a U-Boot CLI.
* From U-Boot you can load additional images (kernel, DTB, ramdisk) and boot them manually or automatically (via `boot.scr`, `extlinux.conf`, etc.).

So, **your code never runs yet** — you’re just sitting in a bootloader waiting to load something else.

---

### 2. **What Happens in Pure Bare-Metal**

If you do what I showed earlier:

```bash
aarch64-none-elf-gcc -nostdlib -nostartfiles -ffreestanding -c start.S -o start.o
aarch64-none-elf-gcc -nostdlib -nostartfiles -ffreestanding -c main.c -o main.o
aarch64-none-elf-ld -T linker.ld start.o main.o -o kernel.elf
aarch64-none-elf-objcopy -O binary kernel.elf kernel8.img
```

Then put **your `kernel8.img`** on the FAT partition and remove/ignore U-Boot completely, the flow becomes:

* **bootcode.bin → start.elf → fixup.dat** (GPU firmware as usual)
* **kernel8.img = your code** loaded directly by firmware into memory
* CPU jumps straight into your `start.S` at `_start`
* Your code is now responsible for:

  * Setting up stack pointer, exception levels
  * Initializing MMU/EL2/EL1 state if you need it
  * Initializing UART/GPIO/timers
  * Doing anything else (printing messages, blinking LEDs, running your logic)

There is **no shell, no CLI, no U-Boot commands, no second-stage loader.**

---

### 3. **Main Differences**

| Aspect                         | U-Boot Scenario                                           | Pure Bare-Metal Scenario                                   |
| ------------------------------ | --------------------------------------------------------- | ---------------------------------------------------------- |
| Who runs first after firmware? | U-Boot binary                                             | Your `start.S`                                             |
| Who sets up DRAM, MMU, caches? | U-Boot does it for you                                    | You must do it manually                                    |
| How to load your code?         | `load mmc ...` / `tftpboot` / `booti` in U-Boot           | Already loaded as `kernel8.img`                            |
| Output seen?                   | U-Boot prompt (`=>`) + its drivers                        | Whatever you program UART to print                         |
| Debugging?                     | You can inspect memory, environment, test boot parameters | You must add your own debug output                         |
| Complexity                     | Easier for loading Linux or other payloads                | You own **everything** (great for learning, but more work) |

---

### 4. **Files Needed**

When running pure bare-metal you still need:

* `bootcode.bin`, `start.elf`, `fixup.dat`, `config.txt` → Pi firmware stage
* `kernel8.img` → **your code**, not U-Boot
* Optional: `overlays/` if you want device tree overlays (not strictly required for minimal bare-metal)

You **don’t need**:

* `u-boot.bin`
* Any U-Boot scripts (`boot.scr`, `extlinux.conf`)

---

### 5. **When to Prefer Each Approach**

* **U-Boot route:**
  Great if you want to experiment with loading different kernels, using network boot, testing memory, or eventually booting Linux — without recompiling the SD card image every time.

* **Pure bare-metal route:**
  Great if you are **learning low-level programming**, writing your own kernel, or want full control of the hardware from the very first instruction.

---

✅ **So in short:**
The commands I showed earlier (`aarch64-none-elf-gcc ... objcopy ... kernel8.img`) produce a kernel image that replaces U-Boot completely. The Pi firmware loads your code directly, you get control right away, and you must do all initialization manually.


## Full baremetal implementation
Perfect — let's build a **minimal bare-metal "Hello World"** for Raspberry Pi 3B (64-bit, ARMv8-A).
This will completely replace U-Boot, so you can see how a pure bare-metal program works.

---

## 1. **Directory Layout**

Let's assume this folder structure:

```
baremetal/
 ├── start.S
 ├── main.c
 ├── linker.ld
 └── Makefile
```

---

## 2. **start.S (Minimal Assembly Startup)**

```asm
// start.S - minimal AArch64 startup code for Raspberry Pi 3
.section .text
.global _start

_start:
    // Disable interrupts
    msr daifset, #0xf

    // Set stack pointer (use address just below 0x80000)
    ldr x0, =_stack_top
    mov sp, x0

    // Call main()
    bl main

1:  wfe             // Wait For Event (low-power idle)
    b 1b            // Loop forever

// Define a simple stack region
.section .bss
.space 4096
_stack_top:
```

This:

* Disables interrupts
* Sets up a small stack
* Jumps to `main()` in C
* Stays in an infinite loop afterward

---

## 3. **main.c (Hello World over Mini UART)**

```c
// main.c - minimal UART "Hello World" for Raspberry Pi 3
#define MMIO_BASE       0x3F000000UL  // Pi 3 MMIO base

#define AUX_ENABLES     ((volatile unsigned int*)(MMIO_BASE + 0x215004))
#define AUX_MU_IO_REG   ((volatile unsigned int*)(MMIO_BASE + 0x215040))
#define AUX_MU_LSR_REG  ((volatile unsigned int*)(MMIO_BASE + 0x215054))

static void uart_init(void) {
    *AUX_ENABLES |= 1;               // Enable mini UART
    // No baud setup for simplicity (firmware default is usually fine)
}

static void uart_putc(char c) {
    while (!(*AUX_MU_LSR_REG & 0x20)) {}  // Wait until TX empty
    *AUX_MU_IO_REG = c;
}

static void uart_puts(const char* s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r'); // Add CR for proper terminal display
        uart_putc(*s++);
    }
}

void main(void) {
    uart_init();
    uart_puts("Hello, bare-metal world!\n");

    while (1) {
        // Stay here forever
    }
}
```

This uses the **mini UART** on the Pi 3 (mapped at `0x3F215000`).

---

## 4. **linker.ld (Linker Script)**

```ld
ENTRY(_start)

SECTIONS {
    . = 0x80000;           /* Raspberry Pi firmware loads kernel8.img at 0x80000 */

    .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss (NOLOAD) : {
        *(.bss*)
        *(COMMON)
    }
}
```

This ensures `_start` goes at address `0x80000`, where the Pi firmware jumps.

---

## 5. **Makefile**

```makefile
CROSS=aarch64-none-elf
CFLAGS=-Wall -O2 -ffreestanding -nostdlib -nostartfiles

all: kernel8.img

kernel8.img: start.o main.o linker.ld
	$(CROSS)-ld -T linker.ld start.o main.o -o kernel.elf
	$(CROSS)-objcopy -O binary kernel.elf kernel8.img

start.o: start.S
	$(CROSS)-gcc $(CFLAGS) -c start.S -o start.o

main.o: main.c
	$(CROSS)-gcc $(CFLAGS) -c main.c -o main.o

clean:
	rm -f *.o *.elf kernel8.img
```

---

## 6. **Build and Run**

```bash
make
```

Copy `kernel8.img` to your SD card’s boot partition **instead of u-boot.bin**.
Keep the other firmware files:

```
bootcode.bin
start.elf
fixup.dat
config.txt
kernel8.img   <-- our file
```

`config.txt` should specify:

```
arm_64bit=1
kernel=kernel8.img
```

Now boot your Pi — you should see:

```
Hello, bare-metal world!
```

on the serial console (UART0, baud rate 115200).

---

## 7. **Testing with QEMU**

If you don’t want to burn an SD card every time:

```bash
qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial stdio
```

You should see the message right in your terminal.

---

✅ **What This Gives You**

* No U-Boot, no Linux — **your code runs immediately after firmware.**
* You are responsible for everything (UART init, memory map, exception levels).
* A great starting point for learning low-level kernel development.


# Running the baremtal through u-boot

## Quick summary (the idea)

1. Put your bare-metal binary on the SD (FAT) partition.
2. From the U-Boot prompt load that file into RAM (or to the address it was linked for).
3. Jump to it with `go <addr>` (for raw flat binaries) — or use `booti/bootm` for kernel formats.
4. Automate via `boot.scr` (U-Boot script) so it runs on boot.

---

## Step-by-step — interactive method

### 1) Check U-Boot environment for useful addresses

Boot into U-Boot and run:

```
printenv
```

Look for variables like `kernel_addr_r`, `loadaddr`, `fdt_addr_r`. Example values often are `kernel_addr_r=0x80000` or `loadaddr=0x40080000` — **use whatever your U-Boot on the Pi has set**.

If not present, choose a safe RAM address (e.g. `0x80000` for typical Pi firmware loads or a larger address like `0x40000000` depending on U-Boot settings). You must match where your binary is *linked* or where you will jump.

---

### 2) Copy your bare-metal binary to the SD card

Put `mybaremetal.bin` (flat binary) on the FAT boot partition (the same partition that contains `bootcode.bin`, `start.elf`) so U-Boot can read it. From Linux:

```bash
# mount and copy, or just copy if auto-mounted by desktop
cp kernel8.img /media/$USER/BOOT/mybaremetal.bin
```

(Replace `kernel8.img` with your actual binary name.)

---
## Important: Linker / load address considerations

* **If your binary is linked to a fixed address** (e.g. your linker script `ENTRY(_start)` sets origin to `0x80000`), you must load it to that same physical address (or put a small trampoline that relocates). So use:

  ```bash
  fatload mmc 0:1 0x80000 mybaremetal.bin
  go 0x80000
  ```
* **Alternatively** link your program *relocatable* to a runtime load address (the U-Boot `${kernel_addr_r}` value). The easiest method is to inspect `printenv kernel_addr_r` and link to that address (or set `kernel_addr_r` to the address you used when linking).
* If your binary is ELF and contains correct load headers, U-Boot might load/relocate it with `load` commands, but simplest for bare-metal is a *flat binary* and `go`.

---

### 3) From the U-Boot prompt, load and run

If the SD boot partition is FAT, use `fatload` (or `load` if you prefer):

```bash
# Example using kernel_addr_r variable (preferred)
fatload mmc 0:1 ${kernel_addr_r} mybaremetal.bin
go ${kernel_addr_r}
```

If `kernel_addr_r` isn’t set, use a literal address you want to load into (match link address when necessary):

```bash
# load to address 0x80000
fatload mmc 0:1 0x80000 mybaremetal.bin
go 0x80000
```

Notes:

* `fatload mmc 0:1 <addr> <file>` copies the file into RAM at `<addr>`.
* `go <addr>` jumps to `<addr>` and begins execution (for raw flat binaries).
* For ext4: use `ext4load mmc 0:1 <addr> <file>`.

---

## If your binary is a Linux Image / zImage / FIT / ELF

* For a Linux `Image` (ARM64): use `booti <addr> <initrd_addr> <fdt_addr>`.
* For a U-Boot `uImage` you may use `bootm`.
* For a FIT image you use `bootm` as well.
  But for a simple bare-metal flat binary the correct command is `go`.

---

## Automate booting with `boot.scr` (so U-Boot runs your image automatically)

Create a text file `boot.cmd`:

```
# boot.cmd
fatload mmc 0:1 ${kernel_addr_r} mybaremetal.bin
go ${kernel_addr_r}
```

Convert to `boot.scr` using the `mkimage` tool (part of u-boot-tools):

```bash
mkimage -C none -A arm64 -T script -d boot.cmd boot.scr
```

Copy `boot.scr` to the FAT boot partition. On boot U-Boot will execute it and run your bare-metal automatically.

> If `mkimage` is not installed, install `u-boot-tools` or use your toolchain’s `mkimage`.

---

## Passing a Device Tree / arguments (optional)

* If your bare-metal needs a device tree blob (DTB), use U-Boot to `fatload` the DTB and pass it, but for most bare-metal hobby kernels you don’t need a DTB.
* For Linux kernels, you usually do `fatload <addr> Image` and `fatload <fdt_addr> bcm2710-rpi-3-b.dtb` then:

  ```bash
  booti ${kernel_addr_r} - ${fdt_addr}
  ```

---

## Example full workflow (concrete)

Assume:

* Your bare-metal file is `mykernel.bin`
* You linked it for `0x80000`

**On host (copy file):**

```bash
cp mykernel.bin /media/$USER/BOOT/
```

**In U-Boot:**

```
=> mmc rescan
=> fatls mmc 0:1
... shows mykernel.bin ...
=> fatload mmc 0:1 0x80000 mykernel.bin
=> go 0x80000
```

**Automate (boot.cmd -> boot.scr):**
`boot.cmd`:

```
mmc rescan
fatload mmc 0:1 ${kernel_addr_r} mykernel.bin
go ${kernel_addr_r}
```

`mkimage -C none -A arm64 -T script -d boot.cmd boot.scr`

---

## Common pitfalls & how to debug

* **Wrong load address** → immediate crash / hang. Fix by matching linker address or load to the linked address.
* **CPU EL / MMU / caches state** → U-Boot often leaves CPU at EL2 with caches/MMU off or on depending on build. Ensure your bare-metal startup expects that. If unsure, in U-Boot `printenv` to find `fdtcontroladdr` etc. For quick tests, design your `_start` to not rely on MMU and to set up whatever state you need.
* **Missing required peripheral init** → U-Boot may have already initialized DRAM; that’s fine. But if your code expects absolute firmware behavior (like certain clocks), test and adapt.
* **Using wrong file system command** (`fatload` vs `ext4load`) — use the right one for partition FS.
* **Permissions / mount** — ensure file copied to boot FAT partition.

---

## Useful U-Boot commands summary

* `printenv` — show env vars
* `mmc rescan` — re-scan MMC/SD
* `fatls mmc 0:1` — list FAT partition
* `fatload mmc 0:1 <addr> <file>` — load from FAT
* `ext4ls mmc 0:1` / `ext4load` — ext4 variants
* `go <addr>` — jump to address (raw binary)
* `booti <addr> <initrd_addr> <fdt_addr>` — boot ARM64 Image
* `bootm` — boot uImage/FIT

---

## Final checklist before you try

* [ ] Confirm where U-Boot’s `${kernel_addr_r}` or `loadaddr` is (`printenv`).
* [ ] Link your bare-metal at that address or load to your linked address.
* [ ] Place binary on accessible partition and verify with `fatls`.
* [ ] Use `fatload` and `go` to test interactively.
* [ ] When happy, create `boot.scr` so it runs automatically.

---

If you want, I can:

* Produce an example `boot.cmd` and `mkimage` command tailored to your binary name and link address.
* Show you an example linker script snippet that links the binary to `kernel_addr_r` so you don’t have to worry about addresses. Which would you prefer?




