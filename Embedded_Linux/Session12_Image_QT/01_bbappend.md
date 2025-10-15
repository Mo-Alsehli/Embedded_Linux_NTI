# `.bbappend` (Editing Other Recipes)

When working with **Yocto**, we sometimes need to modify or extend the functionality of an existing recipe without directly editing it.
This is done using a **`.bbappend` file** (BitBake append).

⚠️ **Important**: It is usually a bad practice to edit the original recipe directly because:

* It breaks maintainability when upgrading Yocto or the layer.
* It causes merge conflicts when pulling upstream updates.
* It makes it difficult to track your own changes.

Instead, `.bbappend` is used to "hook into" an existing recipe and add or override functionality.

---

## Steps to Create a `.bbappend`

### 1. Create Your Own Custom Layer

* First, create a new layer (if you don’t already have one):

  ```bash
  bitbake-layers create-layer /home/you/yocto/meta-custom
  ```
* Add it to your build:

  ```bash
  bitbake-layers add-layer /home/you/yocto/meta-custom
  ```

---

### 2. Match the Recipe Name

* The `.bbappend` file **must have the same name as the original recipe**.
* Example: if you want to modify `busybox_1.33.1.bb`, your append file should be named:

  ```
  busybox_1.33.1.bbappend
  ```
* If you are unsure about the recipe version, check with:

  ```bash
  bitbake -e <recipe-name> | grep ^PV=
  ```
* To make it apply regardless of version , you can use a **wildcard**:

  ```
  busybox_%.bbappend
  ```

  The `%` symbol matches The latest version of the recipe.

---

### 3. File Location

* Place the `.bbappend` file under:

  ```
  meta-custom/recipes-<category>/<recipe-name>/
  ```

  Example:

  ```
  meta-custom/recipes-core/busybox/busybox_%.bbappend
  ```

---

### 4. Modify Tasks Using `:append` or `:prepend`

* Inside the `.bbappend`, you can extend existing functions:

  ```bitbake
  do_install:append() {
      # Extra install steps
      install -m 0644 ${WORKDIR}/myconfig.conf ${D}${sysconfdir}/myconfig.conf
  }

  do_compile:prepend() {
      echo "Custom step before compiling"
  }
  ```
* `:append` → adds to the end of the function.
* `:prepend` → adds to the beginning of the function.
* `:remove` → removes certain flags or values.

---

### 5. Add Files to `SRC_URI`

If you want to add configuration files or patches:

```bitbake
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://myconfig.conf \
            file://mypatch.patch"
```

* Place the files in the same folder as your `.bbappend`.

---

### 6. Example: Modifying a Recipe to Use Systemd

Suppose a recipe installs a service script for **sysvinit**, but you want to enable **systemd**:

1. Create a systemd service file `myapp.service`.
2. Add it with `.bbappend`:

   ```bitbake
   FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

   SRC_URI += "file://myapp.service"

   SYSTEMD_SERVICE:${PN} = "myapp.service"

   do_install:append() {
       install -d ${D}${systemd_system_unitdir}
       install -m 0644 ${WORKDIR}/myapp.service ${D}${systemd_system_unitdir}
   }
   ```
3. Put `myapp.service` in:

   ```
   meta-custom/recipes-core/myapp/myapp/
   ```

---

## Solid Checklist

✅ Create a new layer (don’t edit upstream).
✅ Name `.bbappend` to match the original recipe (with version or `%`).
✅ Add custom files via `SRC_URI`.
✅ Extend/modify tasks with `:append`, `:prepend`, `:remove`.
**Very Important If you will add new files to the extended recipe**
✅ Use `FILESEXTRAPATHS` so BitBake can find your new files.
✅ Test by rebuilding:

```bash
bitbake <recipe-name>
```
