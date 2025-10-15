# 🧩 Building a Custom Yocto Image

This guide provides a **complete, step-by-step explanation** of how to create, configure, and build a **custom Yocto image**. It explains the full workflow — from creating your own meta-layer to integrating apps, patches, and services, and finally generating and deploying your image.

---

## ⚙️ 1. Building a Standard Image

When you run:

```bash
bitbake core-image-weston
```

Yocto compiles the **Weston graphical image** using the default recipes and configurations provided by the official layers such as `meta`, `meta-oe`, and `meta-raspberrypi`.

However, this standard image is **generic** — it may not include your applications, systemd services, or splash screens.
To fully customize your system, you need to **create your own meta-layer** and a new image recipe.

---

## 🏗️ 2. Creating a Custom Meta-Layer

A **meta-layer** in Yocto is a modular collection of recipes, configurations, and patches.
Each layer defines a specific set of software and policies to be integrated into the final image.

You create your layer to:

* Add new applications or drivers.
* Modify existing system services.
* Customize boot behavior, splash screens, or GUI.
* Define your own distro policies and features.

### 📂 Step-by-Step: Creating the Layer

1. **Create the layer structure:**

   ```bash
   cd ~/yocto
   bitbake-layers create-layer meta-custom
   ```

2. **Add your layer to the build:**

   ```bash
   bitbake-layers add-layer ../meta-custom
   ```

3. **Check that it’s registered:**

   ```bash
   bitbake-layers show-layers
   ```

---

## 🧱 3. Layer Directory Structure

Below is the **recommended structure** of your custom meta-layer:

```
meta-custom/
├── conf/
│   ├── layer.conf
│   └── distro/
│       └── distro.conf
├── recipes-core/
│   └── images/
│       └── core-image-custom.bb
├── recipes-systemd/
│   └── myapp/
│       └── myapp_%.bbappend
├── recipes-apps/
│   └── myapp/
│       └── myapp_1.0.bb
└── recipes-patch/
    └── psplash/
        ├── psplash_%.bbappend
        └── files/
            └── custom-logo.png
```

### 🧩 Explanation:

* **`conf/`** – contains configurations for your layer and custom distro.
* **`recipes-core/images/`** – contains your image recipes.
* **`recipes-apps/`** – holds your custom app recipes (C, C++, or Python apps).
* **`recipes-systemd/`** – contains `.bbappend` files for enabling or configuring systemd services.
* **`recipes-patch/`** – stores file modifications, patches, and replacement assets like splash screens.

---

## ⚙️ 4. Distro Configuration (`distro.conf`)

The distro configuration defines **the behavior and policies** of your custom operating system.
Create the file:
`meta-custom/conf/distro/distro.conf`

### Example Content:

```bash
DISTRO = "customdistro"
DISTRO_NAME = "Custom Yocto Distro"
DISTRO_VERSION = "1.0.0"


## DON'T Forget To Add Poky Defaults ##

# Add common Yocto features
IMAGE_FEATURES:append = " splash ssh-server-dropbear package-management"

# Enable systemd as the init manager
VIRTUAL-RUNTIME_init_manager = "systemd"

# Use RPM as package manager
PACKAGE_CLASSES = "package_rpm"

# Add some base Yocto features
DISTRO_FEATURES:append = " wayland opengl pam"
```

### 🧠 Explanation:

* **`DISTRO` / `DISTRO_NAME` / `DISTRO_VERSION`** – define the name and version of your Linux distribution.
* **`IMAGE_FEATURES`** – adds pre-built system capabilities (splash screen, ssh server, etc.).
* **`VIRTUAL-RUNTIME_init_manager`** – tells Yocto which init system to use (`systemd` or `sysvinit`).
* **`PACKAGE_CLASSES`** – defines which package format (RPM, DEB, IPK) Yocto uses for build and deployment.
* **`DISTRO_FEATURES`** – enables extra system capabilities like graphics, OpenGL, or Wayland support.

---

## 🧩 5. Image Recipe (`core-image-custom.bb`)

This is the **main definition file** that tells Yocto how to assemble your image.
Create the file under:
`meta-custom/recipes-core/images/core-image-custom.bb`

### Example Content:

```bitbake
SUMMARY = "Custom Weston Image"
DESCRIPTION = "A customized Yocto image built for Raspberry Pi with Weston GUI and custom applications."

# Include basic Weston and system features
IMAGE_FEATURES += "splash package-management ssh-server-dropbear hwcodecs weston"

# Add custom applications and packages to the image
IMAGE_INSTALL:append = " gtk+3-demo "

# Inherit from the base core-image class
inherit core-image

# Output image formats (ext3/ext4/tar.bz2)
IMAGE_FSTYPES:append = " ext3 ext4 tar.bz2"

# Extend root filesystem space (5GB extra)
IMAGE_ROOTFS_EXTRA_SPACE = "5242880"

# Example of adding demo app
CORE_IMAGE_BASE_INSTALL += " gtk+3-demo"

# QEMU testing memory
QB_MEM = "-m 512"
```

### 🧠 Explanation:

* **`SUMMARY` / `DESCRIPTION`** – metadata for documentation and build info.
* **`IMAGE_FEATURES`** – adds features like SSH server, splash screen, and hardware codecs.
* **`IMAGE_INSTALL`** – defines the packages that will be installed into your root filesystem.
* **`inherit core-image`** – ensures your image inherits all basic build rules from Yocto.
* **`IMAGE_FSTYPES`** – specifies what image file formats to output (for SD card, debugging, etc.).
* **`IMAGE_ROOTFS_EXTRA_SPACE`** – extends your root filesystem for extra data and logs.
* **`QB_MEM`** – allocates memory for testing your image under QEMU.

---

## 🧠 6. Adding Applications (`recipes-apps`)

Custom applications are typically added as separate recipes.

Example structure:

```
meta-custom/recipes-apps/myapp/
└── myapp_1.0.bb
```

Example recipe:

```bitbake
SUMMARY = "My Custom Application"
LICENSE = "CLOSED"
SRC_URI = "file://main.cpp"

S = "${WORKDIR}"

do_compile() {
    ${CXX} main.cpp -o myapp
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 myapp ${D}${bindir}/
}
```

This compiles your app and installs it to `/usr/bin/` in the root filesystem.

---

## 🔧 7. Adding Systemd Services (`recipes-systemd`)

If your app needs to auto-start, add a `.service` file and a `.bbappend` to attach it.

Example:

```
meta-custom/recipes-systemd/myapp/
├── myapp_%.bbappend
└── files/
    └── myapp.service
```

Inside the `.bbappend`:

```bitbake
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI += "file://myapp.service"

SYSTEMD_SERVICE:${PN} = "myapp.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"
```

And inside `myapp.service`:

```ini
[Unit]
Description=MyApp Startup Service
After=network.target

[Service]
ExecStart=/usr/bin/myapp
Restart=always

[Install]
WantedBy=multi-user.target
```

### 🧠 Explanation:

* **`SYSTEMD_SERVICE`** – registers the service file to systemd.
* **`SYSTEMD_AUTO_ENABLE`** – enables it by default at boot.
* **`ExecStart`** – defines the executable command that systemd runs.

---

## 🎨 8. Adding Patches or Custom Splash

Patches and visual customization files (like splash screens) are stored under:

```
meta-custom/recipes-patch/
```

Example:

```
meta-custom/recipes-patch/psplash/
├── psplash_%.bbappend
└── files/
    └── custom-logo.png
```

Inside `.bbappend`:

```bitbake
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI += "file://custom-logo.png"
```

Then you can set:

```bash
SPLASH_IMAGES = "file://custom-logo.png"
```

This replaces the default boot splash image during startup.

---

## 🚀 9. Building Your Custom Image

After everything is ready:

```bash
bitbake core-image-custom
```

This command:

1. Parses all layers and configurations.
2. Builds the image using your defined features, applications, and services.
3. Produces `.wic`, `.ext4`, and `.tar.bz2` image files inside `tmp/deploy/images/`.

---

## 💾 10. Deploying to SD Card

To flash the image to your SD card using `bmaptool`:

```bash
sudo bmaptool copy core-image-custom-raspberrypi3-64.wic.gz /dev/sdX
```

*(Replace `/dev/sdX` with your SD card path.)*

This method automatically uses the `.bmap` file to accelerate the writing process.

---

## ✅ Summary Table

| Step | Task                | Description                                    |
| ---- | ------------------- | ---------------------------------------------- |
| 1    | Build base image    | `bitbake core-image-weston`                    |
| 2    | Create new layer    | `bitbake-layers create-layer meta-custom`      |
| 3    | Define distro       | Add `conf/distro/distro.conf`                  |
| 4    | Create image recipe | Add `recipes-core/images/core-image-custom.bb` |
| 5    | Add apps            | Place them under `recipes-apps/`               |
| 6    | Add services        | Add `.service` files under `recipes-systemd/`  |
| 7    | Add patches         | Modify system behavior in `recipes-patch/`     |
| 8    | Build custom image  | `bitbake core-image-custom`                    |
| 9    | Flash to SD card    | Use `bmaptool copy`                            |

---

## 🧠 Final Notes and Best Practices

* **Layer Naming:** Always prefix your layer with `meta-` for clarity.
* **Version Control:** Track your layer with Git to manage version changes.
* **Layer Priority:** Set `BBFILE_PRIORITY` in `layer.conf` to control override order.
* **Modularity:** Keep each recipe focused — separate apps, patches, and services.
* **Testing:** Use QEMU (`runqemu`) for quick testing before deploying to hardware.

---

By following these steps, you’ll have a **fully reproducible and customizable Yocto image**, containing your own software stack, graphical environment, and automated services — all neatly organized under your custom layer.
