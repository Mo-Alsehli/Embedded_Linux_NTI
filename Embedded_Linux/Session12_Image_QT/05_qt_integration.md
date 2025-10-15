# 🧩 Adding Qt6 to a Yocto Image

This guide explains how to **add Qt6 support to your Yocto image**, integrate a **Qt application using devtool**, and configure **systemd environment and user setup** for proper GUI execution on Raspberry Pi.

---

## 📦 Step 1 — Install and Add `meta-qt6` Layer

1. Visit the **OpenEmbedded layer index**:
   👉 [https://layers.openembedded.org](https://layers.openembedded.org)

2. Search for **`meta-qt6`**, and download or clone the version compatible with your Yocto release (for example `6.8` for Kirkstone).

   ```bash
   git clone -b 6.8 https://github.com/meta-qt6/meta-qt6.git
   ```

3. Add the layer to your build environment:

   ```bash
   bitbake-layers add-layer ../meta-qt6
   ```

4. Confirm the layer is active:

   ```bash
   bitbake-layers show-layers
   ```

---

## ⚙️ Step 2 — Add Your Qt Application with `devtool`

Use `devtool` to import an external Qt project directly from GitHub or a local path.

```bash
devtool add <recipe-name> <repo-link> [--srcbranch <branch>]
```

> ⚠️ Use `--srcbranch` if your repo’s default branch is **not** `master`.

---

## 🧾 Step 3 — Edit the Generated Recipe

After adding the project:

```bash
devtool edit-recipe <recipe-name>
```

Inside the recipe (`.bb` file), make these essential edits:

```bash
inherit qt6-cmake

DEPENDS:append = " qtbase qtquick3d qttools cups qtdeclarative-native qtwayland"
RDEPENDS:${PN} = " qtbase qtquick3d qttools cups qtwayland qtdeclarative"
```

### 🔍 Understanding `DEPENDS` vs `RDEPENDS`

| Variable     | Purpose                                       | Example                                |
| ------------ | --------------------------------------------- | -------------------------------------- |
| **DEPENDS**  | Build-time dependencies (headers, libraries). | `qtbase`, `qttools`, `qtwayland`       |
| **RDEPENDS** | Runtime packages required to execute the app. | `qtbase`, `qtdeclarative`, `qtwayland` |

To verify library names:

```bash
cd meta-qt6
find . -iname "*qtbase*"
find . -iname "*qtwayland*"
```

Use the recipe names (without `_git.bb`) in `DEPENDS` and the package names in `RDEPENDS`.

---

## 🧱 Step 4 — Enable Qt in the Image

In your image recipe (e.g. `core-image-custom.bb`):

```bash
IMAGE_INSTALL:append = " qtwayland qtbase qttools "
```

Choose the appropriate display backend:

* **Wayland (default)**: `QT_QPA_PLATFORM=wayland`
* **X11**: `QT_QPA_PLATFORM=xcb`

---

## 🖥️ Step 5 — Set Environment Variables (via systemd Service)

Qt GUI apps need specific environment variables at runtime.
Create a systemd unit (e.g. `myqtapp.service`):

```ini
[Unit]
Description=Qt GUI Application
After=weston.service

[Service]
Environment=DISPLAY=:0
Environment=XDG_RUNTIME_DIR=/run/user/0
Environment=QT_QPA_PLATFORM=wayland
ExecStart=/usr/bin/myqtapp
User=qtuser
Restart=always

[Install]
WantedBy=multi-user.target
```

Then install and enable it through your recipe or manually:

```bash
systemctl enable myqtapp.service
```

> 🧠 **Tip:**
> If running as a non-root user, ensure `XDG_RUNTIME_DIR` matches the UID directory (e.g., `/run/user/1000`).

---

## 👤 Step 6 — Create a Dedicated Qt User

If you want to run the application under a specific user:

1. **Create a user recipe** (e.g. `qtuser.bb`):

   ```bash
   SUMMARY = "Qt Application User"
   inherit useradd

   USERADD_PARAM:${PN} = "-d /home/qtuser -p <encrypted-pass> -s /bin/bash qtuser --user-group"

   do_install() {
       install -d ${D}/home/qtuser
   }

   FILES:${PN} = "/home/qtuser"
   ```

2. Generate an **encrypted password**:

   ```bash
   openssl passwd <your-password>
   ```

3. Replace `<encrypted-pass>` with that hash.

4. Remember to update the `XDG_RUNTIME_DIR` in your service if you set a specific UID:

   ```
   XDG_RUNTIME_DIR=/run/user/<uid>
   ```

---

## 🧩 Step 7 — Build and Deploy

1. Rebuild the app and image:

   ```bash
   devtool build <recipe-name>
   bitbake core-image-custom
   ```

2. Deploy the updated image to the Raspberry Pi.

3. On boot, your Qt app should start automatically through systemd and display on the Wayland session.

---

## ✅ Summary

| Task                   | Command / File                         | Description                         |
| ---------------------- | -------------------------------------- | ----------------------------------- |
| Add Qt layer           | `bitbake-layers add-layer ../meta-qt6` | Enables Qt6 recipes                 |
| Import app             | `devtool add`                          | Adds your Qt project to Yocto       |
| Configure dependencies | `.bb` recipe                           | Include `qtbase`, `qtwayland`, etc. |
| Add to image           | `IMAGE_INSTALL:append`                 | Ensure Qt packages are built in     |
| Environment setup      | `myqtapp.service`                      | Defines display/session variables   |
| Create Qt user         | `qtuser.bb`                            | Securely add custom user            |
| Build & deploy         | `bitbake core-image-custom`            | Flash and run on target             |

---

By following these structured steps, your **Qt6 application will integrate seamlessly into your Yocto image**, automatically start at boot, and display correctly under Wayland or X11 on Raspberry Pi.
