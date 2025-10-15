# 🖼️ Custom Splash Screen Layer (meta-splash)

This Yocto layer provides a **custom psplash integration** that replaces the default Raspberry Pi splash screen and fixes the common **systemd dependency issue** that prevents the splash from appearing at boot.

---

## 🚨 Problem Overview

By default, the **`meta-raspberrypi`** layer installs a drop-in file:

```
/lib/systemd/system/psplash-start.service.d/framebuf.conf
```

That file adds the condition:

```ini
ConditionPathExists=/dev/fb0
```

This makes **systemd skip `psplash-start.service`** whenever the framebuffer (`/dev/fb0`) is not yet available — for example, on headless boots, RDP sessions, or when HDMI is initialized late.

As a result:

* The splash screen never appears.
* The service looks *loaded but inactive*.
* Your custom splash is overridden or never displayed.

Additionally, Raspberry Pi layers often reassign their own default splash (from `psplash-raspberrypi`), preventing custom images from being shown.

---

## 🧩 Solution Summary

This `meta-splash` layer provides a clean and persistent fix:

1. **Masks** the blocking `framebuf.conf` drop-in:

   ```bash
   /etc/systemd/system/psplash-start.service.d/framebuf.conf → /dev/null
   ```

   → Ensures psplash starts unconditionally at boot.

2. **Overrides** the default Raspberry Pi psplash provider:

   * Replaces the vendor’s `psplash-raspberrypi` with a fully custom `psplash` build.
   * Uses a user-defined splash image (e.g., company logo or project artwork).

3. **Integrates cleanly with systemd** — no manual masking or runtime edits are required.

---

## ⚙️ Layer Integration

### 1️⃣ Add the Layer

```bash
git clone https://github.com/hamzamac/meta-splash
bitbake-layers add-layer ../meta-splash
```

#### Modify the `psplash_%.bbappend`
> Add this code at the end of it.
```

# mmagdi
# Prevent meta-raspberrypi's framebuffer condition from blocking psplash (This caused by systemd)
do_install:append() {
    # Create an override to mask the framebuf.conf drop-in
    install -d ${D}${sysconfdir}/systemd/system/psplash-start.service.d
    ln -sf /dev/null ${D}${sysconfdir}/systemd/system/psplash-start.service.d/framebuf.conf
}

FILES:${PN} += "${sysconfdir}/systemd/system/psplash-start.service.d/framebuf.conf"
```

### 2️⃣ Enable Splash Feature

In your `distro.conf` or custom image recipe, add:

```bash
IMAGE_FEATURES:append = " splash"
```

### 3️⃣ Add Custom Image

Inside `meta-splash/recipes-core/psplash/files/`, place your splash image and update your append:

```bash

SPLASH_IMAGE = "file://your_logo.png"
```

### 4️⃣ Rebuild Image

```bash
bitbake core-image-weston
```

---

## 🔍 Verification Steps

1. Boot your Raspberry Pi.

2. Observe that the **custom splash** appears immediately after boot, before any other service.

3. Check service status to confirm correct startup:

   ```bash
   systemctl status psplash-start.service
   ```

   You should see `active (running)` instead of `inactive (dead)`.

4. Confirm no frame buffer dependency remains:

   ```bash
   systemctl cat psplash-start.service
   ```

---

## 🧠 What This Fix Achieves

| Issue                                                      | Status  |
| ---------------------------------------------------------- | ------- |
| `psplash-start.service` stuck due to `/dev/fb0` dependency | ✅ Fixed |
| Vendor splash overriding user splash                       | ✅ Fixed |
| Headless boot or RDP session missing splash                | ✅ Fixed |
| Persistent change across rebuilds                          | ✅ Yes   |

---

## 🧾 Credits

* Original concept and condition patching: **meta-raspberrypi**
* Layer base: [hamzamac/meta-splash](https://github.com/hamzamac/meta-splash)
* Modifications: Added systemd masking and RPi splash override fix for reliable psplash startup.

---

✅ **Result:**
Your **custom splash screen now appears on every boot**, regardless of framebuffer timing or display type — clean, simple, and 100% Yocto-integrated.
