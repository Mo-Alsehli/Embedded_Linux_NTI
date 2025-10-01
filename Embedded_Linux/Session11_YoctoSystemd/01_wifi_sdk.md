# Wi-Fi & SDK on Yocto (Raspberry Pi 3)

> Solid steps + full explanations for enabling Wi-Fi, SSH, and generating/using an SDK toolchain.
> Tested mindset: **Yocto (kirkstone-ish), RPi3/3B+, `core-image-weston`, systemd**.

---

## Quick Reference

* List a recipe’s tasks:

  ```bash
  bitbake -c listtasks <recipe-name>
  ```

* Show open ports (modern):

  ```bash
  ss -tunlp
  ```

  (Legacy)

  ```bash
  netstat -tun
  ```

---

## 1) Verify Wi-Fi Support

### 1.1 On the running RPi

1. Check interfaces:

   ```bash
   ip a
   ```

   You should see an interface like `wlan0`. If it’s missing, firmware/driver may be absent or rfkill is blocking.

2. Check rfkill status:

   ```bash
   rfkill list
   rfkill unblock all
   ```

### 1.2 Kernel configuration (from build host)

If Wi-Fi driver isn’t present, review kernel config:

```bash
bitbake -c menuconfig virtual/kernel
```

Search for:

* `Device Drivers` → `Network device support` → `Wireless LAN`
* Broadcom/Cypress drivers used by RPi3 (brcmfmac)

Save and persist with a kernel fragment (recommended) instead of hand-editing each time.

### 1.3 Ensure firmware is included

Raspberry Pi 3/3B+ typically needs Broadcom firmware:

* `linux-firmware-brcm` (Yocto splits may include `brcmfmac43430` / `brcmfmac43455` blobs)
* `raspberrypi-firmware` (boot files; separate from Wi-Fi chipset firmware)

Add to your image if missing (see **2.2 Add packages permanently**).

---

## 2) Enable Wi-Fi with ConnMan

ConnMan is the lightweight connection manager commonly used in Yocto images.

### 2.1 One-time interactive connection (on the device)

```bash
connmanctl
# inside the shell:
enable wifi
agent on
scan wifi
services                  # note the service name (e.g., wifi_..._managed_psk)
connect <SERVICE-NAME>    # paste the exact service ID from 'services' output
# you'll be prompted:
# Passphrase? ************
quit
```

> ConnMan stores profiles under `/var/lib/connman/`. After a successful connect, it will auto-reconnect on future boots.

### 2.2 Add packages permanently (build-time)

Edit your **image** or a small **bbappend** (preferred) so the image always contains the right tools/firmware.

In your image `.bb` (or a custom `*.bbappend`):

```bitbake
# Enable an OpenSSH server via image feature (pulls required pkgs)
EXTRA_IMAGE_FEATURES += "ssh-server-openssh"

# ConnMan client tools for on-device management
IMAGE_INSTALL:append = " connman connman-client"

# Wi-Fi firmware for RPi3 variants (adjust to your BSP's package names)
IMAGE_INSTALL:append = " linux-firmware-brcm"

# (Optional but useful)
IMAGE_INSTALL:append = " wpa-supplicant wireless-regdb iw rfkill"
```

> Notes
>
> * **Use `+=` or `:append`** (not `?=`) when you want to **ensure** the feature/package is included.
> * On many BSPs, installing `connman` is enough; it ships a systemd service.

### 2.3 Ensure ConnMan runs at boot (systemd)

ConnMan’s service is usually enabled by the recipe. If not, do:

```bash
systemctl enable connman
systemctl start connman
systemctl status connman
```

> If `systemctl` is not found, your image may not be systemd based. Use a systemd distro (e.g., `DISTRO_FEATURES` includes `systemd` and `VIRTUAL-RUNTIME_init_manager = "systemd"`), or switch your image to one that uses systemd.

---

## 3) SSH Server in the Image

Two reliable ways:

### 3.1 Via image **feature** (recommended)

```bitbake
EXTRA_IMAGE_FEATURES += "ssh-server-openssh"
```

This pulls the necessary `openssh` packages. On boot, `sshd` is usually started automatically (verify with `systemctl status sshd`).

### 3.2 Explicit packages (optional)

If you prefer explicit packages:

```bitbake
IMAGE_INSTALL:append = " openssh openssh-sftp-server"
```

Then on device:

```bash
systemctl enable sshd
systemctl start sshd
```

**Verify from your host:**

```bash
ssh root@<rpi-ip>
```

> If login fails, check `/etc/ssh/sshd_config` and ensure your user/password or keys are set. Default Yocto images often use `root` with no password; consider adding a password or keys for security.

---

## 4) Troubleshooting Wi-Fi

* **No `wlan0`:**

  * Confirm `brcmfmac` module:

    ```bash
    lsmod | grep brcmfmac || modprobe brcmfmac
    dmesg | grep brcmfmac
    ```
  * Verify firmware files exist:

    ```bash
    ls /lib/firmware/brcm/
    ```

* **Blocked by rfkill:**

  ```bash
  rfkill list
  rfkill unblock all
  ```

* **Region/domain issues:**

  ```bash
  iw reg get
  iw reg set EG    # example: set to Egypt's country code
  ```

  Persist by adding `CRDA`/regdb packages and proper config at build time.

* **ConnMan shows services but fails to connect:**

  * Re-enter passphrase; remove profile and try again:

    ```bash
    rm -rf /var/lib/connman/*   # beware: removes saved networks
    ```
  * Check logs:

    ```bash
    journalctl -u connman -b
    ```

---
Here’s a tuned version of that section, rewritten to emphasize your workflow where `bitbake meta-toolchain` (or `populate_sdk`) produces a shell environment that mimics your RPi target:

---

## 5) SDK Toolchains (Cross-Compile)

Yocto can generate a **self-contained SDK** (Software Development Kit) that installs cross-compilers, sysroots, and environment setup scripts.
This lets your host behave like the target (e.g., Raspberry Pi) for building apps outside BitBake.

There are two main flavors:

* **Classic SDK** → `-c populate_sdk`
* **Extensible SDK (eSDK)** → `-c populate_sdk_ext` (adds `devtool`, recipe workspace, layer management)

---

### 5.1 Generate the SDK

```bash
# Classic SDK for your image
bitbake core-image-weston -c populate_sdk

# Extensible SDK (optional, richer dev experience)
bitbake core-image-weston -c populate_sdk_ext

# Or generate a generic toolchain (not image-specific)
bitbake meta-toolchain
```

Artifacts are placed under:

```
tmp/deploy/sdk/
```

You’ll find a self-extracting installer, e.g.:

```
poky-glibc-x86_64-core-image-weston-aarch64-toolchain-<version>.sh
```

---

### 5.2 Install the SDK

```bash
chmod +x ./tmp/deploy/sdk/*toolchain*.sh
./tmp/deploy/sdk/*toolchain*.sh
# Choose an install directory (e.g., /opt/poky/...)
```

---

### 5.3 Enter the SDK Environment

After installation, Yocto provides a setup script. Sourcing it **configures your shell as if you were inside the target environment**:

```bash
source /opt/poky/<version>/environment-setup-aarch64-poky-linux
```

This exports variables such as:

* `CC`, `CXX`, `AR`, `LD` → cross-compiler tools
* `PKG_CONFIG_PATH` → pkg-config for target libs
* `CFLAGS`, `CXXFLAGS` → include target sysroot
* `OECORE_*` → Yocto SDK paths

At this point, your host shell behaves like an **RPi cross-build shell**.

---

### 5.4 Cross-Compile a Test Program

```bash
$CC -o hello hello.c    # compiles using cross compiler
file hello              # should show "aarch64" (for rpi3-64 target)
```

---

### 5.5 Qt SDK (Optional)

If you need Qt cross-development:

```bash
bitbake meta-toolchain-qt6    # requires meta-qt6 layer
```

> ✅ **Tip:** Modern practice is to run `-c populate_sdk(_ext)` **on your actual image recipe** (e.g., `core-image-weston`) so the generated SDK exactly matches the libraries and ABI in your final rootfs.

---

Do you want me to also add a **step showing how to re-enter that SDK shell later** (so you can just run one command and drop into an RPi-like environment)?

---

## 6) Bake It In (Recommended Layer/Image Edits)

**Example**: add Wi-Fi, ConnMan, firmware, and SSH to your custom image recipe (e.g., `recipes-core/images/core-image-weston-custom.bbappend`):

```bitbake
# Ensure SSH server
EXTRA_IMAGE_FEATURES += "ssh-server-openssh"

# ConnMan + tools
IMAGE_INSTALL:append = " connman connman-client"

# Wi-Fi firmware + utilities
IMAGE_INSTALL:append = " linux-firmware-brcm wpa-supplicant wireless-regdb iw rfkill"

# (Optional) some helpful diagnostics
IMAGE_INSTALL:append = " iproute2 iputils ethtool tcpdump"
```

Rebuild:

```bash
bitbake core-image-weston
```

Flash, boot, then connect Wi-Fi via `connmanctl` once; it will persist.

---

## 7) Post-Boot Verification Checklist (RPi)

1. `systemctl status connman` → **active (running)**
2. `ip a` → **wlan0 present with an IP**
3. `ping 8.8.8.8 -c 2` (connectivity test)
4. `ss -tunlp` → verify `sshd` is listening on `0.0.0.0:22` or `:::22`
5. From host: `ssh root@<rpi-ip>` → shell access works

---

## 8) Useful Commands Recap

```bash
# List tasks in a recipe
bitbake -c listtasks <recipe-name>

# Kernel menuconfig
bitbake -c menuconfig virtual/kernel

# ConnMan interactive flow
connmanctl
  enable wifi
  agent on
  scan wifi
  services
  connect <SERVICE-NAME>
  # enter passphrase when prompted
  quit

# Open ports (modern)
ss -tunlp

# Enable SSH via image features
# (in image recipe or bbappend)
EXTRA_IMAGE_FEATURES += "ssh-server-openssh"

# Build SDKs
bitbake core-image-weston -c populate_sdk
bitbake core-image-weston -c populate_sdk_ext

# Install & use SDK
/tmp/deploy/sdk/*toolchain*.sh
source /opt/poky/<ver>/environment-setup-aarch64-poky-linux
$CC -o app app.c
```

---

## 9) Common Pitfalls & Fixes

* **Used `?=` instead of `+=`:**
  `?=` only sets if unset; use `+=` or `:append` to guarantee inclusion.
* **No `systemctl`:**
  Your image may be using BusyBox `init`. Switch to systemd or adjust services accordingly.
* **Wi-Fi firmware missing:**
  Add `linux-firmware-brcm` (or the correct split for your BSP) to `IMAGE_INSTALL`.
* **Profile doesn’t survive reboot:**
  Ensure the first `connmanctl connect` succeeded (a directory is created in `/var/lib/connman/…`), and rootfs isn’t read-only.
* **Wrong country/regdb:**
  Set regulatory domain (`iw reg set <CC>`) and include `wireless-regdb`.

---

## 10) Optional: Non-interactive Auto-Provision (Headless)

Create a small systemd unit or one-shot script that runs on first boot and performs a `connmanctl` scripted connect (or drops a pre-made service file into `/var/lib/connman/`). This is handy for unattended devices—just ensure you securely handle the passphrase.
