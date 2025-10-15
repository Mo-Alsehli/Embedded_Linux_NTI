# Raspberry Pi Custom Splash Screen

## Overview

The **splash screen** is the image that appears during the system boot process before the GUI (e.g., Weston or X11) is launched.
To customize it in a **Yocto build for Raspberry Pi**, we modify the `psplash` recipe using a `.bbappend` file.

---

## Step 1: Create a Custom Layer

We’ll first create a dedicated layer to hold our modifications so that the original `psplash` recipe remains untouched.

```bash
bitbake-layers create-layer ../meta-custom
bitbake-layers add-layer ../meta-custom
```

---

## Step 2: Create the Recipe Structure

Inside the layer, create the folder structure to match the original recipe’s path.

```bash
cd ../meta-custom
mkdir -p recipes-core/psplash/files
cd recipes-core/psplash
```

Place your **custom splash image** inside the `files` directory.

> Supported formats: `.png` (recommended), ideally `320×240` or `800×480` depending on your screen resolution.

Example:

```
meta-custom/
└── recipes-core/
    └── psplash/
        ├── files/
        │   └── my_splash.png
        └── psplash_git.bbappend
```

---

## Step 3: Create the `.bbappend` File

Create `psplash_git.bbappend` inside `recipes-core/psplash/` and add the following:

```bash
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SPLASH_IMAGES = "file://my_splash.png;outsuffix=default"
```

### Explanation

* `FILESEXTRAPATHS` ensures Yocto looks into your `files` directory for additional resources.
* `SPLASH_IMAGES` replaces the default Yocto splash image with your custom one.
* The `outsuffix` value defines the image variation (usually set to `default` unless multiple images are used).

---

## Step 4: Enable Splash Feature in Distro Configuration

Edit your **distro configuration file** (for example `meta-mmagdi-distro/conf/distro/mmagdi.conf`) and ensure the following line exists:

```bash
IMAGE_FEATURES:append = " splash"
```

This ensures the `psplash` package is included in all images built with your distro.

---

## Step 5: Clean and Rebuild psplash

Clean and rebuild `psplash` to apply your changes:

```bash
bitbake -c cleansstate psplash
bitbake psplash
```

Then rebuild your image:

```bash
bitbake core-image-weston
```

---

## Step 6: Flash and Test

After building, flash your new image to the SD card and boot the Raspberry Pi.
Your **custom splash screen** should now appear during startup.

---

## Notes

* The image must be in **RGB format**, no transparency or alpha channel.
* Use tools like `convert` (from ImageMagick) to resize or format it:

  ```bash
  convert custom.png -resize 800x480! -depth 8 my_splash.png
  ```
* For multiple resolutions or branding, you can define:

  ```bash
  SPLASH_IMAGES = "file://splash-small.png;outsuffix=small \
                   file://splash-large.png;outsuffix=large"
  ```
* If psplash does not appear, ensure your image includes `psplash` and not a conflicting boot service like `plymouth`.

---

## Summary of Solid Steps

1. **Create Layer:** `bitbake-layers create-layer ../meta-custom`
2. **Add Recipe Path:** `recipes-core/psplash/files`
3. **Add Image:** Place your splash image inside `files/`
4. **Write .bbappend:** Define `SPLASH_IMAGES` variable
5. **Enable Splash:** Add `IMAGE_FEATURES:append = " splash"` to `distro.conf`
6. **Rebuild:** Clean, rebuild, and flash image
7. **Test Boot:** Verify the new splash appears on startup

---

This method cleanly overrides the default Yocto `psplash` image while keeping your base layers intact and modular.
