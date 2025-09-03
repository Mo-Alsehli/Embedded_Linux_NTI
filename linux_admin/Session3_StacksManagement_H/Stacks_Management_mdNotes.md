# Linux Kernel & File System Notes

## 🖥️ Kernel Overview
The **Linux Kernel** is divided into multiple stacks (subsystems).  
Each subsystem handles a specific area of the operating system and is connected with hardware or device drivers.

- **Network Stack** – manages networking (TCP/IP, sockets, etc.).
- **Process Kernel Stack** – manages processes, scheduling, and CPU interaction.
- **File System Stack** – manages files, directories, and storage devices.

👉 **Operating System = User Space + Kernel**

- **User Space**: Where applications run (e.g., terminal, editors, browsers).  
- **Kernel Space**: Core of the OS, directly interacts with hardware.

---

## ⚙️ Command Flow in Linux
When you run a command in Linux, the following steps happen:

1. **Run command in terminal** (user space).  
2. **libc (C standard library)** is called → it provides the interface to interact with the kernel.  
3. **System Calls** are invoked by libc (e.g., `open()`, `read()`, `write()`).  
4. The **kernel subsystem** (e.g., file system, process, network) takes over.  
5. The **subsystem interacts with device drivers**.  
6. The **device drivers talk to hardware** to complete the request.  

---

## 📂 File System Stack
The **file system stack** is responsible for translating user-friendly file structures into binary structures in memory and storage.

### ✅ Main Responsibilities
- **Translation Layer**: Converts user-space view (files/directories) into binary structures for hardware.  
- **Connection Points**:
  - Between **user-space ↔ file system stack**.
  - Between **file system stack ↔ storage devices**.
- **Dynamic Storage**: Adds new storage devices at runtime.  
- **Protocol Understanding**: Supports multiple file system types (Ext3, Ext4, FAT, etc.).  
- **Root File System**: At least one file system (root `/`) must always be mounted.  

---

## 🛠️ Preparing a New Storage Device

1. **Check connected storages**  
   ```bash
   lsblk

Shows all storage devices attached to the Linux system.

2. **Create a partition**

   ```bash
   sudo fdisk /dev/sdx
   ```

   Replace `x` with the correct device letter (e.g., `sdb`).

3. **Format partition with a file system**

   ```bash
   sudo mkfs.ext4 /dev/sdx
   ```

   * Creates an **Ext4** protocol on the partition.

4. **Mount the file system**

   ```bash
   sudo mount -t ext4 /dev/sdx /mnt/target_dir
   ```

   * `-t ext4`: File system type.
   * `/dev/sdx`: Device file.
   * `/mnt/target_dir`: Directory to attach the storage.

5. **Unmounting**

   ```bash
   sudo umount /mnt/target_dir
   ```

---

## 📊 Useful File System Commands

* **Check disk/partitions**:

  ```bash
  lsblk
  ```
* **Check directory size**:

  ```bash
  du -h --max-depth=1 <path>
  ```

  * `-h`: Human-readable (KB, MB, GB).
  * `--max-depth=1`: Shows sizes of subdirectories at one level.

---

## 📝 Comments & Explanations

* **Why stacks?**
  Each subsystem isolates responsibility → modular design → easier debugging, development, and performance tuning.

* **Why system calls?**
  They provide **safe, controlled access** to hardware. Without them, user programs would sdirectly manipulate hardware → very dangerous.

* **Why mounting is needed?**
  Linux does not auto-recognize new partitions. You must tell the kernel:

  * "This device exists."
  * "Use this file system type."
  * "Make it accessible from this path."

* **Why root (`/`) must be mounted?**
  It is the foundation of the Linux system. Without it, the OS cannot start since all paths and executables come from `/`.

---

````markdown
# 🔹 Process Management Commands in Linux

Linux is a **multi-tasking operating system**. It runs many processes at the same time.  
We use **process management commands** to view, control, and manage these processes.

---

## 1. `ps` – Process Status
- Shows information about running processes.
- Common usages:
  ```bash
  ps          # show processes in current shell
  ps -e       # show all processes
  ps -ef      # full-format listing of all processes
  ps aux      # show all processes with detailed info
````

* Important columns:

  * **PID** → Process ID
  * **TTY** → Terminal controlling the process
  * **TIME** → CPU time used
  * **CMD** → Command that started the process

---

## 2. `pstree` – Process Tree

* Shows processes in a **tree structure** (parent → child).
* Helps to see which process started which.

  ```bash
  pstree
  pstree -p   # show PIDs
  pstree -u   # show users
  ```

---

## 3. `top` – Dynamic Process Viewer

* Interactive tool to monitor processes in **real time**.

  ```bash
  top
  ```
* Useful keys inside `top`:

  * `q` → quit
  * `k` → kill a process
  * `r` → renice (change priority)
  * `h` → help
* Shows CPU usage, memory, running processes.

Alternative: `htop` (more user-friendly, colorful).

---

## 4. Process Priority with `nice` and `renice`

### `nice`

* Run a process with a **specific priority** (niceness value).
* Range: **-20 (highest priority)** → **19 (lowest priority)**.
* Example:

  ```bash
  nice -n 10 firefox
  ```

  → Starts Firefox with lower priority.

### `renice`

* Change priority of a **running process**.
* Example:

  ```bash
  renice -n 5 -p 1234
  ```

  → Changes process with PID 1234 to nice value 5.

---

## 5. Killing Processes (`kill`, `killall`)

### `kill`

* End a process by **PID**.

  ```bash
  kill 1234         # send default signal (TERM)
  kill -9 1234      # force kill (KILL)
  kill -STOP 1234   # stop process (pause)
  kill -CONT 1234   # continue stopped process
  ```

### `killall`

* End a process by **name** instead of PID.

  ```bash
  killall firefox        # kill all processes named firefox
  killall -9 firefox     # force kill
  ```

---

# 📌 Summary

* `ps` → snapshot of processes
* `pstree` → hierarchical tree view
* `top` → live monitoring
* `nice` / `renice` → control process priority
* `kill` / `killall` → terminate processes

---

```

Do you want me to also add a **table of signals (SIGTERM, SIGKILL, SIGSTOP, etc.)** so you know which signal is best to use when killing processes?
```
````markdown
# 🔹 Are These the Only Process Management Commands?

No ✅ — the commands I explained (`ps`, `pstree`, `top`, `nice`, `renice`, `kill`, `killall`) are the **most common and essential** ones, but Linux has **many more tools** for managing processes.  

---

## 1. Viewing & Monitoring Processes
- `ps` → snapshot of processes
- `pstree` → process tree
- `top` → real-time monitor
- `htop` → improved `top` (interactive, colored, easier to use)
- `pgrep` → search for processes by name
  ```bash
  pgrep firefox
````

---

## 2. Controlling Processes

* `kill` → terminate by PID
* `killall` → terminate by name
* `pkill` → terminate by pattern (similar to pgrep + kill)

  ```bash
  pkill -9 firefox
  ```
* `jobs` → list background jobs in current shell
* `fg` → bring job to foreground
* `bg` → continue a stopped job in background
* `disown` → remove job from shell’s job table

---

## 3. Priorities & Scheduling

* `nice` → start process with priority
* `renice` → change running process priority
* `chrt` → set **real-time scheduling** policy/priority
* `uptime` / `w` → show system load & logged users

---

## 4. Advanced Tools

* `systemctl` → manage systemd services (processes managed as services)
* `service` → older command for service management
* `strace` → trace system calls of a process
* `lsof` → list open files by processes
* `vmstat`, `iostat` → system & process performance stats

---

# 📌 Summary

The commands I gave you earlier are the **core set** that everyone uses daily.
But Linux has **extra tools** depending on whether you want:

* Basic monitoring (`ps`, `top`, `htop`)
* Job control (`jobs`, `fg`, `bg`)
* Advanced debugging (`strace`, `lsof`)
* Service management (`systemctl`, `service`)

---

```

Do you want me to prepare a **full categorized cheat-sheet PDF** with *all process management commands* (basic + advanced) so you can use it as a reference for your Embedded Linux diploma?
```

