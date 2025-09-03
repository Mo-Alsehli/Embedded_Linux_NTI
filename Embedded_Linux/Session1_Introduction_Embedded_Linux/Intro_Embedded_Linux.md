# 📘 Introduction to Embedded Linux

Linux in embedded systems requires specific **hardware capabilities** such as memory management, CPU architecture support, and storage. Unlike bare-metal systems, Linux is an operating system kernel that needs a certain baseline of resources and hardware features to function properly.

---

## 🔹 Main Tasks in Embedded Linux

1. **Hardware Bring-up**

   * First step is board selection → choose a board that supports Linux.
   * Run a reference (ordinary) Linux on the board to verify all hardware components work correctly.

2. **Linux Customization**

   * Kernel tuning and configuration according to project requirements.

3. **Application Development**

   * Writing and deploying user-space applications for the target device.

---

## 🔹 Hardware Bring-up

* **Get the hardware board.**

* **Obtain the Board Support Package (BSP):**

  * A BSP is like a *reference Linux* tailored to the board.
  * Includes:

    * Linux Kernel
    * Device Drivers
    * Toolchain
  * Most board features are already enabled in BSP (customization comes later).

* **Flash the BSP on the board**

  * Test the kernel boots successfully.
  * Verify drivers (e.g., Ethernet, USB, GPIO, UART).

📍 **Diagram – Hardware Bring-up**

```
[Hardware Board] → [BSP (Kernel + Drivers + Toolchain)] → [Boot Test & Driver Test]
```

---

## 🔹 Linux Customization

Steps:

1. Download Kernel Source Code.
2. Identify hardware components that need enabling (Wi-Fi, USB, etc.).
3. Configure kernel options.
4. Build the kernel.
5. Boot and test.

👉 Methods:

* **Manual:** Buildroot
* **Automatic:** Yocto

---

## 🔹 Application Development

* Develop **user-space applications** (e.g., C/C++, Python).
* Applications run on top of the **kernel and libraries**.
* Examples: IoT applications, UI software, or industrial controllers.

---

## 🔹 Main Components of Embedded Linux

1. **Bootloader** – Initializes hardware, loads the kernel. (e.g., U-Boot)
2. **Kernel** – Manages hardware, provides system services.
3. **Root File System (RFS)** – Holds libraries, configuration, and applications.
4. **Toolchain** – Compilers and build tools for cross-compiling.
5. **User-space Applications** – The actual apps running on the device.

📍 **Diagram – Embedded Linux Components**

```
+---------------------+
| User Applications   |
+---------------------+
| Libraries (glibc)   |
+---------------------+
| Linux Kernel        |
+---------------------+
| Device Drivers      |
+---------------------+
| Bootloader & BSP    |
+---------------------+
| Hardware Board      |
+---------------------+
```

---

## 🔹 Compilation Models

### 1. Application Layer Compilation

Requirements:

* Application source code.
* **glibc** (interface between app and kernel via syscalls).

  * Must be **compatible with kernel version**.
* Compiler (from toolchain).

### 2. Kernel Layer Compilation

* Kernel Source Code + Compiler.

### 3. Device Driver Compilation

* Kernel Source Code
* Kernel Headers
* Driver Source Code
* Compiler

---

## 🔹 Toolchains

A **toolchain** is a set of tools for compiling code for a specific architecture.

* Toolchain may contain multiple compilers.
* **Types of Toolchains:**

  1. **Baremetal Toolchain** → used for microcontrollers, no OS.
  2. **Linux Toolchain** → used for Linux systems.

     * Pre-built Toolchain (distro-specific).
     * Customized Toolchain.

👉 **Cross-Toolchain (crosstool-NG)** is widely used for building custom Linux toolchains.
📖 Docs: [https://crosstool-ng.github.io/](https://crosstool-ng.github.io/)

---

## 🔹 Crosstool-NG Commands

1. Clean previous config:

   ```bash
   bin/ct-ng distclean
   ```

   → Deletes `.config` and resets customization.

2. List supported samples:

   ```bash
   bin/ct-ng list-samples
   ```

   → Shows supported boards.

   * `aarch64` → ARM 64-bit
   * `arm` → ARM 32-bit

   Example:

   ```
   aarch64-rpi3-linux-gnu
   ```

   → ARM 64-bit, board = Raspberry Pi 3, libc = glibc

3. Select a sample:

   ```bash
   bin/ct-ng <sample>
   ```

   → Creates `.config` for chosen board.

4. Configure toolchain:

   ```bash
   bin/ct-ng menuconfig
   ```

   → Modify `.config` (set versions of gcc, glibc, kernel headers, etc.).

5. Build toolchain:

   ```bash
   bin/ct-ng build
   # Or build with multiple cores:
   bin/ct-ng build.n
   ```

   → Final toolchain output will be in `~/x-tools`.

📍 **Diagram – Toolchain Workflow**

```
[Source Code] → [Compiler (cross)] → [Executable for Target Board]
```

---

## 🔹 Version Handling in Toolchains

Typical versions chosen in `menuconfig`:

* Kernel Headers: **5.15.xx**
* glibc: **2.31 → 2.35**
* gcc: **11.5.0**
* Binutils: **2.34**

---

## 🔹 Important Questions (Interview/Exam)

1. What is the **boot sequence** of Linux? (Bootloader → Kernel → Init → RFS → User Apps)
2. How does the **kernel handle events**? (Syscalls, interrupts, scheduling).
3. What is the difference between **kernel version** and **distribution version**?

