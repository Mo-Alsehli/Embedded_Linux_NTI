# LKM Memory Section

## Overview

Every **Linux Kernel Module (LKM)** is a binary object that gets **loaded into the kernel’s memory space** (RAM) at runtime.
Once inserted, it becomes part of the **monolithic kernel**—meaning it shares the same memory and privileges as the kernel itself.

To manage this efficiently, the kernel uses **specific memory sections**, defined and organized by the **linker script** during compilation.

---

## Linker Script and Kernel Sections

When the kernel (or module) is built, a **linker script** decides **where each part of the code will be placed in memory**.
This script defines named memory regions (called *sections*) for:

* **Code (text)**
* **Data (initialized and uninitialized)**
* **Initialization and cleanup routines**
* **Read-only data**
* **Per-CPU data**, and more.

Each kernel module follows the same structure — it’s just loaded separately into RAM at runtime.

---

## Kernel Memory Concept

### 1. **Monolithic Kernel Architecture**

* The **Linux kernel** is **monolithic**, meaning all core services, device drivers, and subsystems run in the same **kernel address space**.
* However, it’s also **modular**, allowing dynamic modules (`.ko` files) to be **loaded and unloaded at runtime**.

**Dynamic (Out-of-Tree) Modules**

* Compiled separately from the kernel source.
* Loaded via `insmod` and removed via `rmmod`.
* Run **inside kernel space**, not as separate processes.
* Still part of the same monolithic memory model.

**Static (In-Tree) Modules**

* Compiled directly into the kernel image (`vmlinuz`).
* Loaded automatically during boot.
* Cannot be unloaded dynamically.

---

## Memory Layout for LKMs

| Section   | Purpose                                                      | Behavior                                     |
| --------- | ------------------------------------------------------------ | -------------------------------------------- |
| `.text`   | Code section — contains executable instructions              | Always kept in memory while module is loaded |
| `.data`   | Stores initialized global/static variables                   | Retained as long as module is active         |
| `.bss`    | Stores uninitialized variables (zero-initialized by default) | Allocated on load                            |
| `.__init` | Holds initialization code (e.g., module_init)                | Freed after initialization completes         |
| `.__exit` | Holds cleanup code (e.g., module_exit)                       | Used when module is removed                  |
| `.rodata` | Read-only constants and lookup tables                        | Permanently mapped during module lifetime    |

---

## `_init` and `_exit` Sections

### 1. **Why `_init` Section Exists**

* Code in this section runs **only once** when the module is first inserted.
* After the `init` function completes successfully, **the memory used by this section is freed** to save RAM.

**Usage**

```c
static int __init my_module_init(void) {
    printk(KERN_INFO "Module initialized\n");
    return 0;
}
```

Here, `__init` marks this function for placement in the `init` memory section.
When the initialization finishes, the kernel **releases this memory**.

---

### 2. **Why `_exit` Section Exists**

* When using **dynamic (out-of-tree)** modules, we must clean up all resources before removing the module.
* The `_exit` section holds functions that are executed **when the module is unloaded**.
* Ensures proper deallocation of memory, devices, and system hooks.

**Usage**

```c
static void __exit my_module_exit(void) {
    printk(KERN_INFO "Module removed\n");
}
```

`__exit` ensures that this function is placed in the `exit` memory section, which is called only during module removal.

---

### 3. **Encapsulation with `static`**

Both `init` and `exit` functions are declared `static` to:

* Restrict their visibility **only to the current file (module)**.
* Prevent symbol conflicts with other kernel modules.
* Enforce **encapsulation** — only the kernel should call them via `module_init()` and `module_exit()` macros.

---

## Relationship Between In-Tree and Out-of-Tree Modules

| Feature         | In-Tree (Static)                        | Out-of-Tree (Dynamic)                |
| --------------- | --------------------------------------- | ------------------------------------ |
| Build           | Compiled with the kernel source         | Built separately with kernel headers |
| File            | `.o` linked into `vmlinuz`              | `.ko` loaded into kernel memory      |
| Load Time       | At boot                                 | At runtime                           |
| Removable       | No                                      | Yes (`rmmod`)                        |
| Uses `_exit`    | Optional (kernel rarely unloads itself) | Mandatory for cleanup                |
| Memory Handling | Always resident                         | Allocated and freed dynamically      |

---

## Summary

* **All kernel modules run in RAM** and share kernel space (monolithic design).
* The **linker script** defines where each function or variable section resides.
* **`__init`** and **`__exit`** sections improve efficiency by freeing unused memory after initialization and ensuring safe removal.
* **`static`** keyword enforces encapsulation within the module.
* **Static (in-tree)** modules are compiled into the kernel image, while **dynamic (out-of-tree)** modules are built and managed independently at runtime.
