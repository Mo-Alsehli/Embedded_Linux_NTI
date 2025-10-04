# Linux Module

## 1) Writing a Linux Module

### **Header Section**

* To build a Linux kernel module, we must include the necessary kernel headers:

  ```c
  #include <linux/module.h>
  #include <linux/init.h>
  ```
* On some systems, includes may fail because the compiler doesn’t know where kernel headers are located.
  To fix this:

  1. Hover over `#include` inside VS Code (with C/C++ extension enabled).
  2. Apply **Quick Fix → Add to includePath**.
  3. Open `.vscode/c_cpp_properties.json` and add the kernel headers path.
* Kernel headers are usually found under:

  ```
  /lib/modules/<kernel-version>/build/include
  ```

  Example: if `uname -r` outputs `6.14.0-33-generic`, then the path is:

  ```
  /lib/modules/6.14.0-33-generic/build/include
  ```
* On Raspberry Pi:

  * You may need to download/build the Raspberry Pi Linux kernel.
  * Headers are typically located in:

    ```
    linux/include
    ```

---

### **Function Section**

* Every Linux module must define:

  * **Initialization function** (called when module is inserted).
  * **Exit function** (called when module is removed).
* These functions follow fixed prototypes:

  ```c
  static int __init module_init_func(void) {
      printk(KERN_INFO "Module inserted\n");
      return 0;
  }

  static void __exit module_deinit_func(void) {
      printk(KERN_INFO "Module removed\n");
  }
  ```
* Register functions with:

  ```c
  module_init(module_init_func);
  module_exit(module_deinit_func);
  ```
* Compilation requires a **Makefile**:

  ```makefile
  # Identify the output
  obj-m := mymodule.o

  # Specify the kernel directory
  KDIR := /lib/modules/$(shell uname -r)/build

  all:
  	make -C $(KDIR) M=$(shell pwd) modules

  clean:
  	make -C $(KDIR) M=$(shell pwd) clean
  ```
* Commands:

  ```bash
  sudo insmod mymodule.ko    # Insert module
  sudo rmmod mymodule        # Remove module
  dmesg | tail               # View logs
  ```
* Use `printk()` for debugging:

  ```c
  printk(KERN_INFO "Hello Kernel!\n");
  ```

  Always include `\n` at the end to flush messages.

---

### **Variables Section**

* Use kernel-defined datatypes and structures:

  * `struct file_operations`
  * `struct proc_dir_entry`
  * `struct task_struct`
* Example:

  ```c
  static int my_value = 10;
  ```

---

## 2) Creating a File inside the procfs

* **procfs** allows kernel modules to expose information to user space under `/proc/`.

### Steps:

1. **Include necessary headers:**

   ```c
   #include <linux/proc_fs.h>
   #include <linux/seq_file.h>
   ```

2. **Define a read function (what to show in `/proc` file):**

   ```c
   static int my_proc_show(struct seq_file *m, void *v) {
       seq_printf(m, "Hello from my module! Value = %d\n", my_value);
       return 0;
   }
   ```

3. **Define open function (connects `/proc` file with read logic):**

   ```c
   static int my_proc_open(struct inode *inode, struct file *file) {
       return single_open(file, my_proc_show, NULL);
   }
   ```

4. **Define file operations:**

   ```c
   static const struct proc_ops my_proc_ops = {
       .proc_open    = my_proc_open,
       .proc_read    = seq_read,
       .proc_lseek   = seq_lseek,
       .proc_release = single_release,
   };
   ```

5. **Create file in init function:**

   ```c
   static int __init module_init_func(void) {
       proc_create("mymodule", 0, NULL, &my_proc_ops);
       printk(KERN_INFO "proc file created\n");
       return 0;
   }
   ```

6. **Remove file in exit function:**

   ```c
   static void __exit module_deinit_func(void) {
       remove_proc_entry("mymodule", NULL);
       printk(KERN_INFO "proc file removed\n");
   }
   ```

7. **Result:**

   * After inserting the module, check:

     ```bash
     cat /proc/mymodule
     ```
   * You will see:

     ```
     Hello from my module! Value = 10
     ```

---

✅ With this, you now have a **Linux module** that loads/unloads properly, logs debug messages, and creates a custom `/proc` file to communicate with user space.
