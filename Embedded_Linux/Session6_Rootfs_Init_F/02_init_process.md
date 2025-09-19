# 🔑 Init Process in Linux (Embedded Systems)

The **Init process** is the very first program executed in **userspace** after the Linux kernel finishes booting.
It plays a critical role in preparing the system for use by mounting filesystems, loading modules, and eventually providing user control (via shell or GUI).

---

## 🛠 Responsibilities of Init

The Init process is responsible for:

1. **Mounting Pseudo Filesystems**
   These are special filesystems provided by the kernel that expose information about hardware and running processes to userspace.
   Examples include:

   * `/proc` → process information
   * `/sys` → kernel attributes, drivers, GPIO, device tree entries
   * `/dev` → device nodes for hardware access

2. **Initializing User Interface (CLI or GUI)**

   * Starts a shell (like `/bin/ash` from BusyBox) for command-line control
   * On desktop systems, starts GUI components like X11, Wayland, or display managers

3. **Loading Dynamic Kernel Modules**

   * Drivers that were not built statically into the kernel must be loaded at runtime
   * Init (or udev/systemd) handles this

4. **Giving User Control**

   * After setup, Init spawns terminals (virtual consoles) or launches the default application in an embedded system.

---

## 🔧 Mounting Root Filesystem Folders

General command format for mounting a virtual filesystem:

```bash
mount -t <fstype> <source> <target>
```

| Filesystem   | Mount Command                     | Purpose                                                                                         |
| ------------ | --------------------------------- | ----------------------------------------------------------------------------------------------- |
| **sysfs**    | `mount -t sysfs sysfs /sys`       | Kernel exposes hardware attributes here (GPIO, drivers, power states, device tree info)         |
| **procfs**   | `mount -t proc proc /proc`        | Shows runtime information about processes, CPU, memory                                          |
| **devtmpfs** | `mount -t devtmpfs devtmpfs /dev` | Automatically creates device nodes for block/character devices so userspace can access hardware |

> ✅ **Why we mount them:** Without these mounts, we cannot inspect running processes, access GPIOs, or interact with devices — the system would be very limited.

---

## ⚙ Automating the Mounts

Mounting these filesystems manually every time is inconvenient.
We can **automate the process** in two ways:

1. **Kernel Configuration**

   * In the kernel menuconfig, enable:

     ```
     Device Drivers → Generic Driver Options → Maintain a devtmpfs filesystem to mount at /dev
     ```

     This allows `/dev` to be populated automatically by the kernel at boot.

2. **Init Script (or inittab)**

   * For `/proc` and `/sys`, you can add mount commands to Init’s configuration file so they are mounted during boot.

---

## 🏗 Types of Init Systems

There are multiple Init implementations, each with its own complexity level:

1. **BusyBox Init** (Lightweight)

   * Ideal for embedded systems
   * Small footprint and minimal configuration

2. **System V Init (SysVinit)**

   * Traditional UNIX-style init system
   * Uses runlevels and shell scripts

3. **Systemd**

   * Modern init system used in most desktop/server Linux distros
   * Supports **parallel service startup**, dependencies, priorities, logging

4. **Zygote (Android)**

   * Special init for Android that starts the Dalvik/ART runtime

---

## 🏗 BusyBox Init Process (Embedded Focus)

When using BusyBox as Init:

* The init binary is located at:

  ```
  /sbin/init
  ```
* Configuration files must be in:

  ```
  /etc
  ```

### Creating the `inittab`

1. If `/etc` does not exist, create it:

   ```bash
   mkdir -p /etc
   ```
2. Create `inittab`:

   ```bash
   touch /etc/inittab
   ```

### `inittab` Syntax

Each line has the format:

```
<device>::<action>:<command>
```

* **device** – which terminal to run on (e.g., `ttyS0`, `console`, or `null`)
* **action** – tells init when to run this command (e.g., `sysinit`, `respawn`, `askfirst`)
* **command** – the program or script to execute

### Example `inittab`

```bash
null::sysinit:mount -t proc proc /proc
null::sysinit:mount -t devtmpfs devtmpfs /dev
null::sysinit:mount -t sysfs sysfs /sys
console::askfirst:/bin/ash
```

Explanation:

* **sysinit** lines run at system initialization — here we mount `/proc`, `/dev`, and `/sys`.
* **askfirst** spawns a shell (`/bin/ash`) on the console, but only after asking for user input (press Enter).

> ⚠ If you see the error **"can't find /dev/console"**, try removing the device field (use `::sysinit:command`).

Absolutely — a **practical guide** for writing and using `inittab` entries will make this much clearer.
Here’s a detailed **step-by-step guide** for the `<device>::<action>:<command>` syntax with explanations and examples:

---

# 📝 Guide to `inittab` for BusyBox Init

BusyBox `init` uses a single configuration file located at `/etc/inittab`.
Each line in this file tells `init` **what to run, when to run it, and where to attach it**.

The general format is:

```
<device>::<action>:<command>
```

---

## 1️⃣ **Device Field** (`<device>`)

This specifies the terminal (TTY) or console on which the program should run.
Examples:

* `ttyS0` → Serial console on UART0
* `tty1`  → First virtual terminal (usually HDMI screen + keyboard)
* `console` → Generic system console
* `null` → No device (run in the background, no terminal attached)

🔧 **Tip:** If unsure, use `null` for system services (e.g., mounting filesystems) so they run silently.

---

## 2️⃣ **Action Field** (`<action>`)

This tells `init` **when** to run the command.
Common actions:

| Action         | When It Runs                                                            | Example Use                                   |
| -------------- | ----------------------------------------------------------------------- | --------------------------------------------- |
| **sysinit**    | Runs once during system initialization (before everything else)         | Mount filesystems, run early scripts          |
| **respawn**    | Runs continuously — if the process exits, it is restarted               | Critical daemons, login shells                |
| **askfirst**   | Similar to `respawn`, but waits for user to press Enter before starting | Interactive shells                            |
| **wait**       | Runs once and waits until it completes before running other commands    | Initialization scripts that must finish first |
| **once**       | Runs once and does not restart if it exits                              | Non-critical services                         |
| **ctrlaltdel** | Runs when Ctrl+Alt+Del is pressed                                       | Graceful reboot scripts                       |
| **shutdown**   | Runs when the system is shutting down                                   | Cleanup scripts                               |

---

## 3️⃣ **Command Field** (`<command>`)

This is the actual program or script to execute.
Examples:

* `mount -t proc proc /proc`
* `/bin/ash`
* `/sbin/getty 115200 ttyS0`

---

## 🧑‍💻 Example: Minimal `inittab`

Here’s a working example that mounts required filesystems and gives a shell on the serial console:

```bash
# /etc/inittab
# Mount pseudo filesystems
null::sysinit:mount -t proc proc /proc
null::sysinit:mount -t sysfs sysfs /sys
null::sysinit:mount -t devtmpfs devtmpfs /dev

# Start a shell on the console (ask user to press Enter)
console::askfirst:/bin/ash
```

Explanation:

* First three lines mount `/proc`, `/sys`, `/dev` at boot
* The last line spawns a shell on the `console` and waits for user interaction

---

## 🧪 Example: Serial Console + Auto-Respawn

If you want a login shell that **automatically respawns** on serial port `ttyS0` (good for embedded devices):

```bash
ttyS0::respawn:/sbin/getty -L ttyS0 115200 vt100
```

Explanation:

* Runs `getty` on `ttyS0` (UART serial port) at `115200` baud
* If the shell is closed, `init` restarts it

---

## 🧪 Example: Run a Startup Script

If you want to run a custom script once at boot:

```bash
null::wait:/etc/init.d/my_startup.sh
```

Explanation:

* `wait` ensures this script finishes before continuing
* Good for network setup, GPIO initialization, etc.

---

## ✅ Checklist for Using `inittab`

1. **Create the file**:

   ```bash
   mkdir -p /etc
   nano /etc/inittab
   ```
2. **Add your entries** (mounts, shells, services)
3. **Ensure required directories exist** in rootfs:

   ```bash
   mkdir -p /proc /sys /dev
   ```
4. **Set init= in bootargs** (if needed):

   ```bash
   setenv bootargs "... init=/sbin/init ..."
   ```
5. Boot and test — you should see your mounts and shell working.

---

## 🗂 Preparing RootFS

Make sure these directories exist in the rootfs before boot:

```bash
mkdir -p /dev /sys /proc
```

---

## 🚀 Booting on Raspberry Pi with Custom Init

If you want the kernel to use your BusyBox init process explicitly, set the bootargs in U-Boot:

```bash
editenv bootargs "root=/dev/mmcblk0p2 rw rootfstype=ext4 8250.nr_uarts=1 console=ttyS0,115200n8 init=/sbin/init rootwait"
```

Then boot the kernel. It will execute `/sbin/init`, which will read `/etc/inittab`, mount the filesystems, and drop you into a shell.

---

## 🧪 Practical Task (GPIO Control)

As a practical example:

* Write a **C++ application** that:

  * Configures one GPIO pin as **input**
  * Configures another GPIO pin as **output**
  * Toggles the output pin based on the input pin state

This can be used to test that `/sys` and `/dev` are mounted and working, since GPIO control depends on them.
