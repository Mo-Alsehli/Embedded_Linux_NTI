# Writing and Integrating Systemd Services in Yocto

This guide explains how to properly create, package, and enable custom **systemd services** for applications included in a Yocto-based Linux image.

---

## 1. Ensure Systemd Support in Your Distro

By default, some Yocto distributions use **SysVinit**. To enable `systemd`:

### In `conf/local.conf` or your custom distro config:

```conf
DISTRO_FEATURES:append = " systemd"
VIRTUAL-RUNTIME_init_manager = "systemd"
DISTRO_FEATURES_BACKFILL_CONSIDERED = "sysvinit"
VIRTUAL-RUNTIME_initscripts = ""
```

This ensures `systemd` is used instead of `sysvinit`.

---

## 2. Create a Systemd Service File

A `.service` file defines how your app starts.
Example: `myapp.service`

```ini
[Unit]
Description=My Custom App Service
After=network.target

[Service]
ExecStart=/usr/bin/myapp
Restart=always
RestartSec=3
User=root

[Install]
WantedBy=multi-user.target
```

Key fields:

* **After=network.target** → wait until networking is up.
* **ExecStart** → full path to your binary/script.
* **Restart=always** → restarts automatically if it fails.
* **WantedBy=multi-user.target** → starts on boot in normal mode.

---

## 3. Place Service File in Your Recipe

Inside your recipe directory, create `files/myapp.service`.

Example tree:

```
meta-myapp/
  recipes-apps/myapp/
    myapp_1.0.bb
    files/
      myapp.service
      main.cpp
      ...
```

---

## 4. Modify the Recipe to Install and Register Service

Example recipe (`myapp_1.0.bb`):

```bitbake
SUMMARY = "My custom app with systemd service"
LICENSE = "CLOSED"
SRC_URI = "file://main.cpp \
           file://myapp.service"

S = "${WORKDIR}"

inherit systemd

SYSTEMD_SERVICE:${PN} = "myapp.service"

do_compile() {
    ${CXX} ${CXXFLAGS} ${LDFLAGS} main.cpp -o myapp
}

do_install() {
    # Install binary
    install -d ${D}${bindir}
    install -m 0755 myapp ${D}${bindir}/

    # Install systemd service
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/myapp.service ${D}${systemd_system_unitdir}/
}
```

---

## 5. Enable the Service on Boot

Yocto won’t auto-enable services unless specified.
In the recipe:

```bitbake
SYSTEMD_AUTO_ENABLE:${PN} = "enable"
```

This ensures `systemctl enable myapp.service` is executed during image creation.

---

## 6. Add Recipe to Image

Add your package to the image:

* In `local.conf`:

  ```conf
  IMAGE_INSTALL:append = " myapp"
  ```

* Or in your custom image recipe:

  ```bitbake
  IMAGE_INSTALL += "myapp"
  ```

---

## 7. Build and Verify

Rebuild the image:

```bash
bitbake core-image-weston
```

After flashing and booting your RPi:

```bash
systemctl status myapp.service
journalctl -u myapp.service
```

---

## 8. Debugging Common Issues

* If service doesn’t start:

  * Check logs: `journalctl -xe`
  * Verify binary path in `ExecStart`.
* If recipe doesn’t package the `.service`:

  * Ensure `${systemd_system_unitdir}` is used.
* If service isn’t enabled:

  * Confirm `SYSTEMD_AUTO_ENABLE:${PN} = "enable"` is set.

---

✅ With this setup, your **application will install and run as a managed systemd service** directly in your Yocto image.

---
# Adding Systemd Services with `.bbappend`

Sometimes you don’t want to write a full recipe, but instead extend an **existing recipe** to install and enable a `systemd` service. This is done using a **`.bbappend` file**.

---

## 1. Locate the Original Recipe

Find the recipe you want to extend. For example, extending `busybox_1.36.1.bb`.

Check with:

```bash
bitbake-layers show-recipes | grep busybox
```

Suppose it’s in `meta/recipes-core/busybox/busybox_1.36.1.bb`.

---

## 2. Create a `.bbappend` File in Your Layer

In your custom layer (e.g., `meta-myappend`), create:

```
meta-myappend/
  recipes-core/
    busybox/
      busybox_1.36.1.bbappend
      files/
        busybox-extra.service
```

⚠️ The `.bbappend` filename **must match** the original recipe’s name and version.
To apply regardless of version, you can use a wildcard:

```
busybox_%.bbappend
```

---

## 3. Write the Systemd Service

Example: `files/busybox-extra.service`

```ini
[Unit]
Description=BusyBox Custom Service
After=network.target

[Service]
ExecStart=/bin/echo "Hello from busybox service"
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

---

## 4. Extend the Recipe with `.bbappend`

Inside `busybox_%.bbappend`:

```bitbake
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

inherit systemd

SYSTEMD_SERVICE:${PN} += "busybox-extra.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_install:append() {
    # Install custom systemd service
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/busybox-extra.service ${D}${systemd_system_unitdir}/
}
```

---

## 5. Add to Image

Make sure the package is included in your image (busybox is already core).
If you extended another app, add it with:

```conf
IMAGE_INSTALL:append = " <pkgname>"
```

---

## 6. Rebuild and Test

```bash
bitbake core-image-weston
```

On target:

```bash
systemctl status busybox-extra.service
journalctl -u busybox-extra.service
```

---

## ✅ Key Points

* `.bbappend` must match the original recipe name (with exact version or `%` wildcard).
* Use `inherit systemd` in the append file.
* Place service files inside `files/` and install them to `${systemd_system_unitdir}`.
* Use `SYSTEMD_AUTO_ENABLE` to auto-start at boot.


