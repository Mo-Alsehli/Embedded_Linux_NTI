# Migrating to `systemd` in Yocto

This guide gives **solid steps** and **full explanations** to migrate a Yocto-based system from SysV init scripts to `systemd`, and to ship your own application as a `systemd` service.

---

## 1) Key Concepts (What/Why)

### What is `systemd`?

`systemd` is an init system (PID 1) and a service manager. In Yocto, it’s delivered as packages and **enabled at the distro level** by adding the `systemd` feature and selecting it as the **virtual init provider**.

### `DISTRO_FEATURES` vs. packages

* `DISTRO_FEATURES` is a **capability switchboard** (e.g., `systemd`, `x11`, `bluetooth`, `wifi`).
  Adding a feature **does not** directly install a package; it **enables a policy** that lets recipes and virtual providers resolve to the correct implementations (e.g., choose `systemd` over `sysvinit`).
 - So we can't assign this variable with normal user pacakges.
* Actual packages are still pulled in via `RDEPENDS`, `IMAGE_INSTALL`, etc.

### Backfilling and virtual runtime

* **Backfilling**: newer Yocto releases may *auto-add* certain features to `DISTRO_FEATURES` for compatibility.
  If you **don’t** want a legacy feature injected (like `sysvinit`), explicitly list it in:

  * `DISTRO_FEATURES_BACKFILL_CONSIDERED = "sysvinit"`
      - So we assign that so that if user or image uses the `inittab`.
* **Virtual runtime**: you **select the implementation** of an abstract role:

  * `VIRTUAL-RUNTIME_init_manager = "systemd"`
   
---

## 2) Minimal Migration (Fast Path)

> Use this when you’re switching an existing image to boot with `systemd`.

Edit **one place** (prefer distro `.conf`; for quick tests, `conf/local.conf` is fine):

```conf
# Enable systemd as init
DISTRO_FEATURES:append = " systemd"

# Ensure sysvinit is NOT auto-backfilled
DISTRO_FEATURES_BACKFILL_CONSIDERED = "sysvinit"

# Select systemd as the init manager
VIRTUAL-RUNTIME_init_manager = "systemd"

# Optional: disable SysV initscripts if you had them
VIRTUAL-RUNTIME_initscripts = ""

# Recommended: use systemd-udev (device management under systemd)
VIRTUAL-RUNTIME_dev_manager = "udev"
```

Rebuild your image:

```bash
bitbake <your-image>
```

Flash and boot. On target, verify:

```bash
# Should print "systemd" or "systemd+"
ps -p 1 -o comm=

# Basic health check
systemctl is-system-running

# View logs
journalctl -b --no-pager
```

---

## 3) Solid Steps (Full, Clean Migration)

1. **Decide scope**

   * **Project-wide**: create a custom distro file (e.g., `meta-yourlayer/conf/distro/yourdistro.conf`) and put the settings there.
   * **Per build (quick test)**: put them in `conf/local.conf`.

2. **Set `systemd` feature and providers**

   ```conf
   DISTRO_FEATURES:append = " systemd"
   DISTRO_FEATURES_BACKFILL_CONSIDERED = "sysvinit"
   VIRTUAL-RUNTIME_init_manager = "systemd"
   VIRTUAL-RUNTIME_dev_manager  = "udev"
   VIRTUAL-RUNTIME_login_manager = "busybox"     # or "shadow" if you use it
   ```

3. **Remove SysV-only assumptions**

   * If you had recipes inheriting `update-rc.d` only, plan to add `systemd` units (see Section 5).
   * If both must be supported, gate installs with `DISTRO_FEATURES`.

4. **Make sure packages align**

   * Most distros pull `systemd` automatically once the feature/provider is set.
   * If you need tools:

     ```conf
     IMAGE_INSTALL:append = " systemd-analyze systemd-timesyncd"
     ```

5. **Rebuild and test**

   ```bash
   bitbake -c cleanall <your-image>
   bitbake <your-image>
   ```

6. **Boot verification**

   ```bash
   ps -p 1 -o comm=
   systemctl --version
   systemctl list-units --type=service --state=running
   journalctl -b
   ```

---

## 4) Coexistence vs. Cutover

* **Coexist (temporary):** ship both SysV scripts and `systemd` units; select at runtime based on the distro feature.
* **Cutover (recommended):** remove SysV initscripts entirely once all services have units.

Use feature conditionals in recipes:

```bitbake
# Example fragment inside a .bb
PACKAGECONFIG ??= ""
PACKAGECONFIG:append:pn-myapp = "${@bb.utils.contains('DISTRO_FEATURES','systemd',' systemd','',d)}"
```

Or use install conditionals:

```bitbake
do_install:append () {
    if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
        install -d ${D}${systemd_unitdir}/system
        install -m 0644 ${WORKDIR}/myapp.service ${D}${systemd_unitdir}/system/
    fi
}
```

---

## 5) Write a `systemd` Service for Your App

**Directory layout (in your recipe):**

```
recipes-apps/myapp/
├── files/
│   ├── myapp           # your binary or startup script (if you ship it here)
│   └── myapp.service   # systemd unit
└── myapp_1.0.bb
```

**`myapp.service` (example):**

```ini
[Unit]
Description=MyApp Service
Wants=network-online.target
After=network-online.target

[Service]
Type=simple
ExecStart=/usr/bin/myapp
Restart=on-failure
RestartSec=2
# If your app needs environment:
# Environment=MYAPP_OPTS=--fast

[Install]
WantedBy=multi-user.target
```

**`myapp_1.0.bb`:**

```bitbake
SUMMARY = "MyApp with a systemd service"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=<put-your-md5>"

SRC_URI = " \
    file://myapp \
    file://myapp.service \
"

S = "${WORKDIR}"

inherit systemd

# Where to install the unit
SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE:${PN} = "myapp.service"

# Auto-enable the service at boot
SYSTEMD_AUTO_ENABLE = "enable"

do_install () {
    # Install the binary (adjust if you actually build it)
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/myapp ${D}${bindir}/myapp

    # Install the service unit
    install -d ${D}${systemd_unitdir}/system
    install -m 0644 ${WORKDIR}/myapp.service ${D}${systemd_unitdir}/system/
}

FILES:${PN} += " \
    ${bindir}/myapp \
    ${systemd_unitdir}/system/myapp.service \
"
```

**Add to your image:**

```conf
IMAGE_INSTALL:append = " myapp"
```

**On target (runtime checks):**

```bash
systemctl daemon-reload
systemctl enable myapp
systemctl start  myapp
systemctl status myapp --no-pager
journalctl -u myapp -b --no-pager
```

---

## 6) Converting a SysV Script to a `systemd` Unit (Quick Mapping)

| SysV concept         | systemd equivalent                           |
| -------------------- | -------------------------------------------- |
| `/etc/init.d/foo`    | `foo.service` in `${systemd_unitdir}/system` |
| Runlevels            | Targets (e.g., `multi-user.target`)          |
| `Required-Start`     | `After=`, `Wants=`/`Requires=`               |
| `start/stop/restart` | `systemctl start/stop/restart foo`           |
| Logs (syslog)        | `journalctl -u foo`                          |

Common pitfalls:

* **Ordering**: use `After=network-online.target` for network-dependent apps.
* **Type**: `simple` for foreground processes; `forking` for daemons that fork.
* **Environment**: set with `Environment=KEY=VALUE` or `EnvironmentFile=`.

---

## 7) Troubleshooting Checklist

1. **Still booting into SysV?**

   * Check `ps -p 1 -o comm=` → must say `systemd`.
   * Confirm `VIRTUAL-RUNTIME_init_manager = "systemd"` and `DISTRO_FEATURES` includes `systemd`.
   * Ensure `DISTRO_FEATURES_BACKFILL_CONSIDERED = "sysvinit"` so it isn’t re-inserted.

2. **Service not starting at boot**

   * Confirm `SYSTEMD_AUTO_ENABLE = "enable"` in recipe (or `systemctl enable` on target).
   * Verify `WantedBy=multi-user.target` in the unit.
   * Run `systemctl status <service>` and `journalctl -u <service>` for errors.

3. **Network ordering**

   * Use `Wants=network-online.target` and `After=network-online.target`.
   * Install and use `systemd-networkd-wait-online` if appropriate.

4. **Unit file not installed**

   * Confirm it’s copied to `${systemd_unitdir}/system`.
   * Verify `FILES:${PN}` includes the unit path.

5. **Mixed environments**

   * If you must support both SysV and systemd during transition, gate installs with `DISTRO_FEATURES` conditionals and test both paths.

---

## 8) Reference Snippets (Copy/Paste)

**Distro or local.conf (canonical):**

```conf
DISTRO_FEATURES:append = " systemd"
DISTRO_FEATURES_BACKFILL_CONSIDERED = "sysvinit"
VIRTUAL-RUNTIME_init_manager = "systemd"
VIRTUAL-RUNTIME_dev_manager  = "udev"
```

**Image adds:**

```conf
IMAGE_INSTALL:append = " systemd-analyze systemd-timesyncd"
```

**Verify on target:**

```bash
ps -p 1 -o comm=
systemctl is-system-running
journalctl -b --no-pager
```

---

## 9) FAQ

* **Does adding `systemd` to `DISTRO_FEATURES` automatically install `systemd`?**
  It **enables** `systemd` as the implementation for the virtual init role. With `VIRTUAL-RUNTIME_init_manager = "systemd"`, the build will pull in the needed `systemd` components.

* **Do I need to keep my old SysV scripts?**
  No, if you are fully migrated. If you must support older images, keep both temporarily and gate by `DISTRO_FEATURES`.

* **How do I auto-start my service?**
  `SYSTEMD_AUTO_ENABLE = "enable"` in the recipe, or `systemctl enable <service>` on the target.

---

## 10) Quick Test Recipe Template

Use this skeleton to ship any small app as a `systemd` service:

```bitbake
SUMMARY = "Hello service"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=<md5>"

SRC_URI = "file://hello.sh \
           file://hello.service"

S = "${WORKDIR}"

inherit systemd

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE:${PN} = "hello.service"
SYSTEMD_AUTO_ENABLE = "enable"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/hello.sh ${D}${bindir}/hello

    install -d ${D}${systemd_unitdir}/system
    install -m 0644 ${WORKDIR}/hello.service ${D}${systemd_unitdir}/system/
}

FILES:${PN} += " \
    ${bindir}/hello \
    ${systemd_unitdir}/system/hello.service \
"
```

`hello.service`:

```ini
[Unit]
Description=Hello demo service
After=network.target

[Service]
Type=simple
ExecStart=/usr/bin/hello
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

`hello.sh`:

```sh
#!/bin/sh
echo "Hello from systemd service"
sleep infinity
```

Add to image:

```conf
IMAGE_INSTALL:append = " hello"
```

Build, flash, and confirm:

```bash
systemctl status hello
journalctl -u hello -b --no-pager
```
