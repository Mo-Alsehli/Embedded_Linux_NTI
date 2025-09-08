

# Linux Technical Reference

### **1. What Is Linux?**

Linux is a free and open-source operating system modeled after UNIX.  
It is built around a **kernel**, which manages hardware, and a set of **user tools** and **utilities** used to interact with the system.

#### Key Characteristics

- Multi-user, multitasking
    
- Modular and portable kernel
    
- Extensive use of **shell commands**
    
- “Everything is a file” philosophy
    

---

### **GNU vs Linux – Difference and Relationship**

| Item               | GNU                                                                 | Linux                                                     |
| ------------------ | ------------------------------------------------------------------- | --------------------------------------------------------- |
| **Type**           | Collection of free software tools and utilities (project)           | Kernel (core of an operating system)                      |
| **Founded by**     | Richard Stallman (1983)                                             | Linus Torvalds (1991)                                     |
| **Purpose**        | Build a complete free (libre) Unix-like OS                          | Provide the operating system kernel                       |
| **Components**     | GNU C Library (glibc), GNU Shell (bash), coreutils, compilers, etc. | Process scheduler, memory management, device drivers, IPC |
| **License**        | GNU General Public License (GPL)                                    | GNU General Public License (GPL)                          |
| **Standalone OS?** | No (missing kernel)                                                 | No (kernel only)                                          |

**Relationship**

The **GNU project** provides user-space tools and system utilities (compilers, shell, libraries), and **Linux** provides the **kernel**.

Together, they form a full operating system commonly referred to as **GNU/Linux**.

**In short:**

> Linux = kernel  
> GNU = utilities and tools  
> GNU + Linux = complete operating system (what people normally call “Linux”).

### **2. Linux Boot Process**

```
BIOS → Bootloader → Kernel → Init system → Services/Targets → Shell/Login
```

|Stage|Description|
|---|---|
|BIOS/UEFI|Hardware initialization|
|Bootloader|Loads the kernel into memory (`GRUB`, `LILO`)|
|Kernel|Detects hardware, mounts root filesystem|
|Init System|Starts system services (`systemd`, `SysV`, `Upstart`)|
|Services|Network, logging, cron, sshd, etc.|
|Login Shell|Prompts user for credentials|

---

### **3. Linux File System Layout**

| Directory        | Description                                        |
| ---------------- | -------------------------------------------------- |
| `/`              | Root of the file system                            |
| `/bin`           | Essential user binaries                            |
| `/sbin`          | System binaries (admin utilities)                  |
| `/usr`           | User applications and libraries                    |
| `/var`           | Variable files (logs, databases)                   |
| `/etc`           | Configuration files                                |
| `/proc`          | Virtual filesystem for kernel and process info     |
| `/dev`           | Device files (block and character devices)         |
| `/home`          | User home directories                              |
| `/mnt`, `/media` | Mount points for temporary / removable filesystems |

---

### **4. Filesystem Types and Mounting**

| Type | Description              |
| ---- | ------------------------ |
| ext4 | Default Linux filesystem |
| xfs  | High performance         |
| vfat | FAT32 (USB drives)       |
| ntfs | Windows filesystem       |

Mount example:

```bash
sudo mount /dev/sdb1 /mnt/usb
```

List mounted filesystems:

```bash
mount | column -t
```

---

### **5. Basic Navigation & Info Commands**

| Command       | Function                                      |
| ------------- | --------------------------------------------- |
| `pwd`         | Show current directory                        |
| `cd /path`    | Change directory                              |
| `ls -al`      | List files with permissions & details         |
| `df -h`       | Disk usage                                    |
| `du -sh *`    | Size of each file/folder in current directory |
| `file <name>` | Identify file type                            |

---

### **6. File and Directory Operations**

```bash
touch file.txt                 # create file
mkdir mydir                    # create directory
cp file1 file2                 # copy file
mv old new                     # move/rename
rm file.txt                    # remove file
rm -r dir                      # remove directory
```

---

### **7. Users & Permissions**

Create a new user:

```bash
sudo adduser magdi
```

Modify a password:

```bash
passwd magdi
```

**Permissions:**

| Flag | Meaning |
| ---- | ------- |
| r    | Read    |
| w    | Write   |
| x    | Execute |

```
chmod 755 file.sh
chown magdi:magdi file.sh
```

---

### **8. Process Management**

|Command|Description|
|---|---|
|`ps aux`|List running processes|
|`top` / `htop`|Display CPU/memory usage|
|`kill <PID>`|Terminate a process|
|`kill -9 <PID>`|Force terminate|
|`systemctl status`|Check service status|

Background example:

```bash
sleep 60 &      # run in background
jobs            # list jobs
fg %1           # bring back to foreground
```

---

### **9. Package Management**

**APT (Debian/Ubuntu)**

```bash
sudo apt update
sudo apt install git
sudo apt remove git
```

**DNF (Fedora/Rocky)**

```bash
sudo dnf install httpd
sudo dnf remove httpd
```

**Pacman (Arch)**

```bash
sudo pacman -Syu
```

---

### **10. Systemd (Init System)**

|Command|Description|
|---|---|
|`systemctl start <service>`|Start a service|
|`systemctl stop <service>`|Stop a service|
|`systemctl status <service>`|Check status|
|`systemctl enable <service>`|Start at boot|

Example:

```bash
sudo systemctl enable sshd
sudo systemctl start sshd
```

---

### **11. Networking**

```bash
ip a               # show IP configuration
ping 8.8.8.8       # test connectivity
ss -tulpn          # list listening ports
/etc/hosts         # local hostname resolution
```

---

### **12. Shell Scripting (Essentials)**

```bash
#!/bin/bash

for file in *.log
do
    echo "Compressing $file..."
    gzip "$file"
done
```

Condition example:

```bash
if [ -f /var/log/syslog ]; then
  echo "Log file exists"
fi
```

---

### **13. Learning Roadmap (Recommended Order)**

1. Basic commands and filesystem structure
    
2. Permissions and user management
    
3. Process and service control
    
4. Package managers
    
5. Bash scripting
    
6. Networking tools
    
7. System administration (cron, ssh, firewall)
    
8. Advanced topics (containers, SELinux, kernel modules)
    

---
**FreeRTOS vs Linux – Key Differences**

|Aspect|FreeRTOS|Linux|
|---|---|---|
|**Type**|Real-Time Operating System (RTOS, microkernel style)|General-purpose OS (monolithic kernel)|
|**Determinism**|Hard real-time – deterministic task scheduling|Soft real-time (non-deterministic due to scheduler and services)|
|**Footprint**|Very small (tens of KB) – fits in microcontrollers|Large (MBs) – requires MMU and full hardware resources|
|**Architecture**|Bare-metal style, simple scheduler, no memory protection|Full multitasking OS with virtual memory, drivers, user space|
|**Use Case**|Highly constrained embedded systems (sensors, MCUs, IoT nodes)|Complex systems (SBCs, servers, embedded Linux devices)|
|**Drivers / File systems**|Minimal – developer often writes drivers manually|Built-in drivers, networking stacks, filesystems, processes|
|**Development Model**|Single application tightly integrated with OS|Multi-process environment running multiple applications|

**Summary:**  
FreeRTOS is used for microcontrollers when deterministic timing and minimal footprint are required. Linux is used on more powerful hardware when multi-tasking, networking, drivers and a full operating system environment are needed.