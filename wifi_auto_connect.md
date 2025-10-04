Perfect 👍 let’s make this **clean and Yocto-friendly**, with no hardcoded IP/Gateway/DNS inside the script.
We’ll use **Yocto recipe variables**, so you can set them in `local.conf` or `distro.conf`.

---

# 🚀 Steps to Enable WiFi with Static IP in Yocto

---

## **1. Add Configuration Variables in `local.conf`**

*Optional but you need to assign them with your credintials in the script*
Edit your `build/conf/local.conf` and add:

```bash
WIFI_STATIC_IP = "192.168.1.50"
WIFI_NETMASK   = "255.255.255.0"
WIFI_GATEWAY   = "192.168.1.1"
WIFI_DNS       = "8.8.8.8"
```

👉 You can get these values on Ubuntu with:

```bash
ip route | grep default    # shows your gateway
ip a                       # shows subnet/netmask
```

---

## **2. Write the Script Template**

`recipes-connectivity/wifi-config/files/wifi-static.sh`

```bash
#!/bin/sh
# WiFi static IP setup (generated from Yocto variables)

CONF="/etc/wpa_supplicant/wpa_supplicant.conf"
IP="@WIFI_STATIC_IP@"
NETMASK="@WIFI_NETMASK@"
GATEWAY="@WIFI_GATEWAY@"
DNS="@WIFI_DNS@"

echo "[*] Starting wpa_supplicant"
killall wpa_supplicant 2>/dev/null
wpa_supplicant -B -i wlan0 -c $CONF

# Disable WiFi power save (improves stability)
iw dev wlan0 set power_save off 2>/dev/null || true

# Assign static IP
ifconfig wlan0 $IP netmask $NETMASK up

# Set default gateway
route add default gw $GATEWAY

# Configure DNS
echo "nameserver $DNS" > /etc/resolv.conf

echo "[+] WiFi static setup complete: $IP via $GATEWAY"
```

---

## **3. Create the Service File**

`recipes-connectivity/wifi-config/files/wifi-static.service`

```ini
[Unit]
Description=WiFi static IP configuration
After=network.target
Wants=network.target

[Service]
Type=oneshot
ExecStart=/usr/bin/wifi-static.sh
RemainAfterExit=yes

[Install]
WantedBy=multi-user.target
```

---

## **4. Write the Recipe**

`recipes-connectivity/wifi-config/wifi-config.bb`

```bitbake
SUMMARY = "Provide WiFi configuration with static IP service"
LICENSE = "CLOSED"

SRC_URI = "file://wpa_supplicant.conf \
           file://wifi-static.sh \
           file://wifi-static.service"

S = "${WORKDIR}"

inherit allarch systemd

# Substitute Yocto vars into the script
do_configure() {
    sed -e "s|@WIFI_STATIC_IP@|${WIFI_STATIC_IP}|" \
        -e "s|@WIFI_NETMASK@|${WIFI_NETMASK}|" \
        -e "s|@WIFI_GATEWAY@|${WIFI_GATEWAY}|" \
        -e "s|@WIFI_DNS@|${WIFI_DNS}|" \
        ${WORKDIR}/wifi-static.sh > ${WORKDIR}/wifi-static.sh
}

do_install() {
    # WiFi credentials
    install -d ${D}${sysconfdir}/wpa_supplicant
    install -m 0600 ${WORKDIR}/wpa_supplicant.conf \
        ${D}${sysconfdir}/wpa_supplicant/wpa_supplicant.conf

    # Script
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/wifi-static.sh \
        ${D}${bindir}/wifi-static.sh

    # Service
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/wifi-static.service \
        ${D}${systemd_system_unitdir}/wifi-static.service
}

FILES:${PN} += " \
    ${sysconfdir}/wpa_supplicant/wpa_supplicant.conf \
    ${bindir}/wifi-static.sh \
    ${systemd_system_unitdir}/wifi-static.service \
"

SYSTEMD_SERVICE:${PN} = "wifi-static.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"
```

---

## **5. Build and Deploy**

1. Add your layer if not already:

   ```bash
   bitbake-layers add-layer ../meta-yourlayer
   ```
2. Rebuild your image:

   ```bash
   bitbake core-image-weston
   ```
3. Flash and boot → WiFi comes up with the static IP you defined in `local.conf`.

---

## ✅ Result

* You define IP/Gateway/DNS in `local.conf` → no hardcoding.
* Recipe installs:

  * `/etc/wpa_supplicant/wpa_supplicant.conf`
  * `/usr/bin/wifi-static.sh` (generated with your values)
  * `wifi-static.service` enabled at boot
* On boot, `systemd` runs the script → WiFi connects with static IP.
