# Introduction to the Linux Kernel

## Overview

The **Linux Kernel** is the core component of the Linux operating system — it manages hardware resources, provides system calls, handles processes, and enables communication between software and hardware.
Kernel functionality can be extended through **modules**, which are pieces of code that can be inserted into or removed from the kernel dynamically.

---

## Module Insertion Methods

Linux supports **two main ways** to insert modules into the kernel:

### 1. **Static (In-Tree) Modules** — `module.o`

* The module is compiled **as part of the kernel source tree**.
* The resulting binary becomes part of the kernel image (`vmlinuz`).
* Requires recompiling the entire kernel when changes are made.

**Pros**

* Fast initialization since the module loads with the kernel.
* Better integration with the kernel environment.

**Cons**

* Rebuilding the full kernel source is required for any change.
* The module cannot be removed at runtime.
* Increases boot time and kernel size.

---

### 2. **Dynamic (Out-of-Tree) Modules** — `module.ko`

* The module is **compiled separately** from the main kernel source.
* It can be loaded and unloaded at runtime using:

  ```bash
  insmod <module>.ko
  rmmod <module>
  ```
* Requires kernel headers or source for the build process.

**Pros**

* No need to recompile the kernel.
* Can be inserted or removed dynamically (runtime flexibility).
* Saves RAM since it is not loaded unless needed.
* Easier for testing and development.

**Cons**

* Slightly slower to load compared to in-tree modules.
* Must match the exact kernel version and configuration.

---

## Linux Kernel Modules (LKM)

> LKMs are pieces of code that extend kernel functionality **without recompiling or rebooting** the system.

**Common Use Cases**

* Implementing new **system calls**.
* Developing **device drivers** (e.g., for GPIO, sensors, etc.).
* Extending **security** or monitoring features.
* Customizing OS behavior at runtime.

**Important Notes**

* LKMs **cannot use the standard C library (`libc`)** — it depends on system calls, which are only accessible from **user space**, not kernel space.
* Instead, LKMs rely on **kernel-provided functions** and macros.
* The kernel itself uses **assembly** for critical operations like **context switching**, where it switches CPU execution from one process to another.

---

## Structure of a Kernel Module (C Code Sections)

### 1. **Header Section**

* Kernel headers define the structures, macros, and functions used to interact with the kernel.
* Example includes:

  ```c
  #include <linux/module.h>   // Core module definitions
  #include <linux/kernel.h>   // Kernel log functions (printk)
  #include <linux/init.h>     // Macros for init/exit functions
  ```

### 2. **Initialization and Cleanup Functions**

* Every kernel module must define:

  ```c
  static int __init my_module_init(void) {
      printk(KERN_INFO "Module inserted\n");
      return 0;
  }

  static void __exit my_module_exit(void) {
      printk(KERN_INFO "Module removed\n");
  }

  module_init(my_module_init);
  module_exit(my_module_exit);
  ```

---

## Objective: Creating a `/proc` File

* The **`/proc` file system (procfs)** is a special kernel interface that exposes kernel data structures to user space.
* You can create custom entries under `/proc` to display module information or debug data.

**Concept Summary**

* A **file system** in Linux is essentially a **software protocol (language)** that defines how data is read and written.
* Including kernel headers like:

  ```c
  #include <linux/proc_fs.h>
  ```

  gives access to the functions needed to register new entries in `/proc`.

**Goal Example**

* Create a kernel module that generates a file `/proc/gpio27`.
* When the user writes to that file, the kernel performs actions (like toggling a GPIO pin).

---

## Summary

| Feature         | Static Module                | Dynamic Module                   |
| --------------- | ---------------------------- | -------------------------------- |
| Build Location  | In kernel source             | External source                  |
| File Type       | `module.o` (part of vmlinuz) | `module.ko`                      |
| Compilation     | Requires full kernel rebuild | Uses existing headers/source     |
| Runtime Control | Fixed at boot                | Load/unload dynamically          |
| Flexibility     | Low                          | High                             |
| Use Case        | Core kernel functions        | Drivers, experiments, extensions |

**In short:**

* Use **static modules** for core kernel features that must always be present.
* Use **dynamic modules (LKMs)** for flexible, testable, and removable functionality.
