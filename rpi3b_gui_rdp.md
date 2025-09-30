Excellent 👍 — let’s do this **end-to-end** so you have both options:

1. **Build-time prep in Yocto** (so Weston has RDP backend).
2. **Runtime direct command** (test manually on the Pi).
3. **systemd service override** (permanent auto-start).

---

# 🔧 Part 1: Build-time Yocto prep

### 1.1 Add RDP backend to Weston

Create a bbappend in your custom layer (`meta-mmagdi`):

```bash
cd ~/yocto/meta-mmagdi
mkdir -p recipes-graphics/wayland
nano recipes-graphics/wayland/weston_%.bbappend
```

Put inside:

```
# Enable RDP backend in Weston
PACKAGECONFIG:append = " rdp"
```

This ensures `rdp-backend.so` is built.

**Verify backend exists on rpi**
```bash
ls /usr/lib/libweston-*/rdp-backend.so
ls /usr/lib/libweston-*/vnc-backend.so
````

---

### 1.2 Rebuild Weston and your image

```bash
cd ~/yocto/build-rpi3b
bitbake -c cleansstate weston
bitbake weston
bitbake core-image-weston
```
```
Flash the image to your SD card and boot your Raspberry Pi.
---

# 🔧 Part 2: Runtime — run Weston with RDP manually

### 2.1 Generate TLS cert and key

On the Pi:

```bash
mkdir -p /etc/freerdp/keys

openssl req -x509 -nodes -newkey rsa:2048 -days 365 \
  -keyout /etc/freerdp/keys/server.key \
  -out /etc/freerdp/keys/server.crt \
  -subj "/CN=weston"
```

This creates:

* `/etc/freerdp/keys/server.key`
* `/etc/freerdp/keys/server.crt`

---

### 2.2 Stop existing Weston and run with RDP

```bash
systemctl stop weston

weston --backend=rdp-backend.so \
  --rdp-tls-cert=/etc/freerdp/keys/server.crt \
  --rdp-tls-key=/etc/freerdp/keys/server.key \
  --no-clients-resize
```

Now Weston is serving RDP on **port 3389**.

---

### 2.3 Connect from laptop (Remmina)

* Protocol: **RDP**
* Server: `<pi-ip>:3389`
* User: `root` (or your user)
* Password: (use `passwd` to set if not set yet)
* Resolution: 1280x800 (or match your preference)
* Color depth: 16-bit for speed

#### On remmina 
- create a new device.
- add the server as `rpi IP`.
- add the username and password as `root`
- save and connect.

---

# 🔧 Part 3: systemd — make Weston always start with RDP

### 3.1 Create systemd override

On the Pi:

```bash
mkdir -p /etc/systemd/system/weston.service.d
cat >/etc/systemd/system/weston.service.d/override.conf <<'EOF'
[Service]
ExecStart=
ExecStart=/usr/bin/weston --backend=rdp-backend.so \
  --rdp-tls-cert=/etc/freerdp/keys/server.crt \
  --rdp-tls-key=/etc/freerdp/keys/server.key \
  --no-clients-resize
EOF
```

Reload and restart:

```bash
systemctl daemon-reload
systemctl restart weston
```

Now, after every reboot, Weston will start **with RDP backend** automatically.

---

# ✅ Summary

* **Yocto build step:** add `weston_%.bbappend` → `PACKAGECONFIG:append = " rdp"` → rebuild.
* **Manual test:** stop Weston → run `weston --backend=rdp-backend.so ...`.
* **Permanent service:** create systemd override → Weston always runs with RDP.
* **Client:** connect from laptop with Remmina (RDP, IP:3389).

Perfect 👍 Let’s make it so that when you flash the SD card, the RPi already has the RDP certs in place and Weston auto-starts with RDP — no manual `openssl` needed.

---

# 🔧 Adding RDP certs at build time (Yocto integration)

We’ll do this with a **small custom recipe** in your `meta-mmagdi` layer.

---

## 1. Create a recipe to install certs

Go to your layer:

```bash
cd ~/yocto/meta-mmagdi
mkdir -p recipes-core/rdp-certs/files
```

### 1.1 Generate self-signed certs on your host (once)

On your **host PC** (not inside Yocto):

```bash
cd recipes-core/rdp-certs/files
openssl req -x509 -nodes -newkey rsa:2048 -days 3650 \
  -keyout server.key -out server.crt \
  -subj "/CN=weston"
```

Now you have `server.key` and `server.crt` inside `files/`.

---

### 1.2 Create the recipe

Create `recipes-core/rdp-certs/rdp-certs.bb`:

```bitbake
SUMMARY = "Pre-generated TLS certs for Weston RDP backend"
LICENSE = "CLOSED"

SRC_URI = "file://server.key \
           file://server.crt"

S = "${WORKDIR}"

do_install() {
    install -d ${D}/etc/freerdp/keys
    install -m 600 ${WORKDIR}/server.key ${D}/etc/freerdp/keys/server.key
    install -m 644 ${WORKDIR}/server.crt ${D}/etc/freerdp/keys/server.crt
}

FILES:${PN} = "/etc/freerdp/keys"
```

---

## 2. Add the recipe to your image

In your **distro conf (`mmagdi.conf`)** or `local.conf`:

```conf
IMAGE_INSTALL:append = " rdp-certs"
```

---

## 3. Make Weston auto-start with RDP backend

You already created a `systemd` override manually. Let’s also bake that into the image.

### 3.1 Create service override file

In your layer:

```bash
mkdir -p recipes-graphics/weston/files
```

Create `recipes-graphics/weston/files/weston-rdp-override.conf`:

```ini
[Service]
ExecStart=
ExecStart=/usr/bin/weston --backend=rdp-backend.so \
  --rdp-tls-cert=/etc/freerdp/keys/server.crt \
  --rdp-tls-key=/etc/freerdp/keys/server.key \
  --no-clients-resize
```

---

### 3.2 Extend Weston recipe

Create `recipes-graphics/weston/weston_%.bbappend`:

```bitbake
PACKAGECONFIG:append = " rdp"

SRC_URI:append = " file://weston-rdp-override.conf"

do_install:append() {
    install -d ${D}${systemd_system_unitdir}/weston.service.d
    install -m 0644 ${WORKDIR}/weston-rdp-override.conf \
        ${D}${systemd_system_unitdir}/weston.service.d/override.conf
}
```

This ensures the override file is installed at `/etc/systemd/system/weston.service.d/override.conf`.

---

## 4. Rebuild image

```bash
cd ~/yocto/build-rpi3b
bitbake core-image-weston
```

Flash the SD card.

---

## 5. Boot and test

On the Pi:

```bash
systemctl status weston
```

It should show Weston starting with `--backend=rdp-backend.so ...`.

On your laptop (Remmina):

* Protocol: RDP
* Server: `<pi-ip>:3389`
* User: `root` (or your Yocto user)
* Password: `passwd` set password

You should connect immediately 🎉

---

# ✅ End Result

* `rdp-certs` recipe installs `/etc/freerdp/keys/server.key` + `server.crt`.
* `weston_%.bbappend` enables RDP backend and auto-configures systemd.
* After flashing, Weston is ready for RDP out of the box.
