# Yocto `devtool` — Solid Steps & Full Explanations

A practical guide to creating, building, deploying, and finishing recipes with `devtool`—both from local sources and remote git—plus essential packaging notes, QA fixes, and Linux special-permissions (setuid/setgid/sticky) with their Yocto implications.

---

## Prerequisites

* A configured Yocto build environment:

  ```bash
  source oe-init-build-env <build-dir>
  ```
* Your target `MACHINE` set in `conf/local.conf` (e.g., `raspberrypi3-64`).
* SSH access to your target device for deployment (e.g., `root@<rpi-ip>`).
* (Recommended for `devtool finish`) Your layer under git control and **clean** (no uncommitted changes).

---

## Concepts You’ll Use

* **Workspace layer**: `build-dir/workspace/`

  * `workspace/sources/<recipe-name>` → *live source tree when using remote git or `devtool modify`.*
  * `workspace/recipes/<recipe-name>/<recipe-name>_<version>.bb` → *auto-generated recipe while developing.*
  * `workspace/appends/*.bbappend` → *temporary appends sometimes created by devtool.*
* **Key variables**:

  * `S` → build-time source directory (defaults vary; with `devtool` it often points into `workspace/sources/<recipe-name>`).
  * `B` → build directory (separate from `S` for out-of-tree builds; useful with CMake).
  * `${D}` → destination staging root for packaging (used in `do_install`).
  * `${bindir}`, `${sbindir}`, `${libdir}`, `${sysconfdir}` → target install prefixes.

---

## Quick Start — Local Sources

### 1) Create a recipe (local)

Solid Steps:

```bash
# From inside your build directory (after sourcing oe-init-build-env)
devtool add <recipe-name> </absolute/or/relative/path/to/local/sources>
```

**What happens:**

* A temporary recipe is created under:

  ```
  <build-dir>/workspace/recipes/<recipe-name>/<recipe-name>_<version>.bb
  ```
* Your layer `workspace` is auto-added to `bblayers.conf`.
* `S` will point at your local source (or a mirrored `workspace/sources/<recipe-name>` depending on context).
* If a build system is detected, `devtool` may inject `inherit cmake` or `inherit autotools`/`inherit pkgconfig` etc.

### 2) Build the recipe

Solid Steps:

```bash
devtool build <recipe-name>
```

**Explanation:**

* Runs the standard OE tasks (`do_fetch`, `do_unpack`, `do_patch`, `do_configure`, `do_compile`, `do_install`, `do_package`, …).
* If `do_install` fails, add or fix it in the recipe (see below).

### 3) Fix `do_install` (common case)

**Typical minimal `do_install` for a single executable built by CMake into `${B}`:**

```bitbake
# <build-dir>/workspace/recipes/<recipe-name>/<recipe-name>_<version>.bb

SUMMARY = "My app"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=<md5sum>"  # Update accordingly

inherit cmake

SRC_URI = "file://CMakeLists.txt \
           file://src/main.cpp \
          "

S = "${WORKDIR}"          # We copied files directly via SRC_URI, so S is WORKDIR
B = "${WORKDIR}/build"    # Out-of-tree build

do_install() {
    install -d ${D}${bindir}
    # If your CMake builds a binary into ${B}, install it:
    install -m 0755 ${B}/myapp ${D}${bindir}/myapp
}

# Ensure main package ships the binary (often default, but explicit is fine):
FILES:${PN} += "${bindir}/myapp"
```

> **Tip:** If you used `devtool add` pointing at a *pre-existing tree*, `S` may be the workspace source path (e.g., `${WORKDIR}/git` or `workspace/sources/<recipe-name>`). Adjust `S`, `B`, and installed file paths accordingly.

### 4) Deploy to a running target

Solid Steps:

```bash
# Replace <ip> with your device IP
devtool deploy-target <recipe-name> root@<ip>
```

**Explanation:**

* Copies the packaged output to the target over SSH.
* Best for quick-run tests without rebaking the full image.

> To roll back deployed files:
>
> ```bash
> devtool undeploy-target <recipe-name> root@<ip>
> ```

### 5) Finish (inject into a real layer)

Solid Steps:

```bash
# Write finalized recipe & patches into your permanent layer
devtool finish <recipe-name> /path/to/meta-yourlayer
```

**Explanation:**

* Moves the temporary `workspace` changes into your layer.
* Requires a **git repo** and a clean index in `/path/to/meta-yourlayer`.
* After finishing, the recipe is part of your layer; remove workspace copy with `devtool reset`.

### 6) Clean up

Solid Steps:

```bash
devtool reset <recipe-name>
```

* Removes the temporary workspace recipe/append so future builds use your finalized layer copy.

---

## Quick Start — Remote Git (e.g., GitHub)

### 1) Create a recipe (remote)

Solid Steps:

```bash
devtool add <recipe-name> <git-url> [--branch <branch>] [--version <pv>]
```

**What happens:**

* `workspace/sources/<recipe-name>` is created with a git checkout.
* A recipe appears under `workspace/recipes/<recipe-name>/…`.
* `SRC_URI` becomes something like:

  ```bitbake
  SRC_URI = "git://github.com/user/proj.git;branch=main;protocol=https"
  SRCREV = "<git-commit>"    # Autopinned
  PV = "1.0+git${SRCPV}"
  S = "${WORKDIR}/git"
  ```

### 2) Build, deploy, finish

Same commands as local:

```bash
devtool build <recipe-name>
devtool deploy-target <recipe-name> root@<ip>
devtool finish <recipe-name> /path/to/meta-yourlayer
devtool reset <recipe-name>
```

---

## Editing, Modifying, and Iterating

* **Edit the current recipe**:

  ```bash
  devtool edit-recipe <recipe-name>
  ```
* **Open source in your editor** (after `devtool add` or `devtool modify`):

  * Edit files under `workspace/sources/<recipe-name>`.
  * Rebuild quickly:

    ```bash
    devtool build <recipe-name>
    ```
* **Adopt an existing recipe to a writable workspace**:

  ```bash
  devtool modify <existing-recipe>
  # Makes a workspace source tree; edit, then rebuild & test
  devtool build <existing-recipe>
  devtool finish <existing-recipe> /path/to/meta-yourlayer
  devtool reset <existing-recipe>
  ```

---

## Two Common Patterns for `SRC_URI`

### A) Local “file://” (no external repo)

Use when you copy files into the recipe:

```bitbake
SRC_URI = "file://CMakeLists.txt \
           file://src/main.cpp \
          "
S = "${WORKDIR}"
B = "${WORKDIR}/build"
```

Place files next to the `.bb` recipe under a `files/` directory:

```
recipes-foo/foo/foo_1.0.bb
recipes-foo/foo/files/CMakeLists.txt
recipes-foo/foo/files/src/main.cpp
```

### B) Git “git://”

Use when tracking upstream:

```bitbake
SRC_URI = "git://github.com/user/proj.git;branch=main;protocol=https"
SRCREV = "abcdef1234..."        # Pin to a commit for reproducibility
S = "${WORKDIR}/git"
```

---

## Typical Recipe Templates

### 1) CMake Project

```bitbake
SUMMARY = "Example CMake app"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=<md5sum>"

inherit cmake pkgconfig

SRC_URI = "git://github.com/user/app.git;branch=main;protocol=https"
SRCREV = "abcdef1234..."
S = "${WORKDIR}/git"
B = "${WORKDIR}/build"

DEPENDS += "openssl"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/app ${D}${bindir}/app
}

FILES:${PN} += "${bindir}/app"
```

### 2) Makefile Project

```bitbake
SUMMARY = "Example Make app"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=<md5sum>"

inherit pkgconfig

SRC_URI = "git://github.com/user/mkproj.git;branch=main;protocol=https"
SRCREV = "abcdef1234..."
S = "${WORKDIR}/git"

EXTRA_OEMAKE = "CC='${CC}' CXX='${CXX}' LD='${LD}'"

do_compile() {
    oe_runmake
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/mybin ${D}${bindir}/mybin
}

FILES:${PN} += "${bindir}/mybin"
```

---

## Deploying & Running on Target

* **Deploy**:

  ```bash
  devtool deploy-target <recipe-name> root@<ip>
  ```
* **Run**:

  ```bash
  ssh root@<ip> /usr/bin/myapp
  ```
* **Undeploy**:

  ```bash
  devtool undeploy-target <recipe-name> root@<ip>
  ```

---

## Finishing Into Your Layer

* Ensure your layer is a clean git repo:

  ```bash
  cd /path/to/meta-yourlayer
  git status   # should be clean
  ```
* Finish:

  ```bash
  devtool finish <recipe-name> /path/to/meta-yourlayer
  ```
* Verify the new recipe landed under e.g.:

  ```
  meta-yourlayer/
    recipes-<cat>/<recipe-name>/<recipe-name>_<version>.bb
    recipes-<cat>/<recipe-name>/<files>...
  ```
* Remove workspace version:

  ```bash
  devtool reset <recipe-name>
  ```

> **Note:** If `devtool finish` complains, it’s usually because the destination layer isn’t a git repo or you have uncommitted changes. Commit/stash first.

---

## Adding to Your Image

To pull your package into an image:

```bitbake
# In your image recipe or distro config
IMAGE_INSTALL:append = " <recipe-name>"
```

---

## Debugging Build Issues

* Review task logs:

  ```
  tmp/work/<triplet>/<recipe-name>/<version>/temp/log.do_<task>
  ```
* Re-run just one task:

  ```bash
  bitbake <recipe-name> -c compile -f
  bitbake <recipe-name> -c install -f
  ```
* Clean state if stuck:

  ```bash
  bitbake -c cleansstate <recipe-name>
  devtool reset <recipe-name>
  ```

---

## Systemd Services (Optional)

If your app is a service:

```bitbake
inherit systemd

SYSTEMD_SERVICE:${PN} = "myapp.service"

do_install:append() {
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/myapp.service ${D}${systemd_system_unitdir}/
}

FILES:${PN} += "${systemd_system_unitdir}/myapp.service"
```

Enable at runtime:

```bash
systemctl enable myapp
systemctl start myapp
```

---

## Special Permissions in Linux (setuid, setgid, sticky) — With Yocto Notes

### setuid (u+s)

**What:** Executable runs with the **file owner’s** effective UID (commonly root).
**Example:** `/usr/bin/passwd` needs to update `/etc/shadow`.

**Set on target (CLI):**

```bash
chmod u+s /usr/bin/myhelper
```

**In Yocto (during do_install):**

```bitbake
do_install:append() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/myhelper ${D}${bindir}/myhelper
    chmod u+s ${D}${bindir}/myhelper
}
```

> **QA note:** Some distros flag setuid binaries. If QA warns, document why it’s needed. Only use setuid where absolutely necessary and consider capabilities (`setcap`) as a safer alternative.

### setgid (g+s)

**What:** Executable runs with the **file group’s** effective GID. On directories, new files inherit the directory’s group.

**Set on target (CLI):**

```bash
chmod g+s /usr/lib/mygroupdir
```

**In Yocto:**

```bitbake
do_install:append() {
    install -d -m 2775 ${D}${localstatedir}/lib/mygroupdir
    # 2 = setgid bit; ensures group inheritance for new files
}
```

### sticky bit (+t)

**What:** On directories (e.g., `/tmp`), only owner or root can delete/rename their own files, even if the directory is world-writable.

**Set on target (CLI):**

```bash
chmod +t /var/tmp/shared
```

**In Yocto:**

```bitbake
do_install:append() {
    install -d -m 1777 ${D}${localstatedir}/tmp/shared
    # 1 = sticky; 777 perms with sticky are typical for temp dirs
}
```

---

## Packaging & Files Hints

* Ensure installed paths are packaged:

  ```bitbake
  FILES:${PN} += "${bindir}/myapp ${sysconfdir}/myapp.conf"
  ```
* Add runtime dependencies (tools needed **on target**):

  ```bitbake
  RDEPENDS:${PN} += "bash openssl"
  ```
* Add build-time dependencies (headers/libs to **build**):

  ```bitbake
  DEPENDS += "openssl"
  ```

---

## `require` vs `include` vs `inherit` (Yocto Syntax Primer)

* **`inherit <class>`**
  Pulls in a `.bbclass` file from `classes/` (adds functions/vars/behaviors).
  *Example:* `inherit cmake`, `inherit systemd`.

* **`require <file>`**
  Includes another recipe snippet **and errors out** if not found.
  Used to factor common variables/tasks across multiple recipes.
  *Example:* `require recipes-common/common-flags.inc`.

* **`include <file>`**
  Like `require` but **does not error** if the file is missing.
  Useful for optional overrides.

> Rule of thumb:
>
> * Use **`inherit`** for behaviors (classes).
> * Use **`require`** for mandatory shared content.
> * Use **`include`** for optional shared content.

---

## Common Pitfalls & Fixes

* **`devtool finish` fails** → Ensure destination layer is a git repo and `git status` is clean; commit or stash first.
* **`do_install` missing** → Explicitly create `${D}${bindir}` etc., and `install` files there.
* **Binary not in final image** → Add to `IMAGE_INSTALL:append = " <recipe-name>"` or make your image depend on a meta-package that pulls it.
* **Wrong file perms/caps** → Set in `do_install`; for capabilities, use `setcap` under `fakeroot` (ensure target has `libcap`).
* **RDEPENDS/DEPENDS confusion** → `RDEPENDS` for runtime, `DEPENDS` for build time.
* **SRCREV floating** → Pin `SRCREV` to a commit for reproducible builds.

---

## Cheatsheet (Commands)

```bash
# Start dev environment
source oe-init-build-env <build-dir>

# Create from local path
devtool add <recipe> </path/to/src>

# Create from git
devtool add <recipe> <git-url> [--branch main]

# Build
devtool build <recipe>

# Deploy / Undeploy to target
devtool deploy-target <recipe> root@<ip>
devtool undeploy-target <recipe> root@<ip>

# Edit recipe
devtool edit-recipe <recipe>

# Modify existing recipe into workspace
devtool modify <recipe>

# Finish into your layer
devtool finish <recipe> /path/to/meta-yourlayer

# Reset (cleanup workspace copy)
devtool reset <recipe>
```

---

## Minimal Example (Local CMake App)

**Tree:**

```
meta-yourlayer/
  recipes-demo/myapp/
    myapp_1.0.bb
    files/
      CMakeLists.txt
      src/main.cpp
      LICENSE
```

**`myapp_1.0.bb`:**

```bitbake
SUMMARY = "Demo app"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=<md5sum>"

inherit cmake

SRC_URI = "file://CMakeLists.txt \
           file://src/main.cpp \
           file://LICENSE \
          "

S = "${WORKDIR}"
B = "${WORKDIR}/build"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/myapp ${D}${bindir}/myapp
}

FILES:${PN} += "${bindir}/myapp"
```

**Build & Test:**

```bash
bitbake myapp
devtool deploy-target myapp root@<ip>
ssh root@<ip> /usr/bin/myapp
```

---

## Notes on “Local Might Fail (finish step)”

* If you initially used **absolute local paths** with `devtool add <path>`, finishing can be awkward unless your layer is a git repo.
* **Best practice:** keep your layer under git from the start. If needed, `git init`, `git add`, `git commit` before `devtool finish`.

---

## Where Your Stuff Ends Up

* **During dev:** `build-dir/workspace/recipes/<recipe>/...` and `build-dir/workspace/sources/<recipe>/...`
* **After finish:** `/path/to/meta-yourlayer/recipes-*/<recipe>/<recipe>_<version>.bb` (+ any `files/` you installed)

---

## Security Reminder on Special Permissions

Use `setuid`/`setgid` **sparingly**; prefer Linux capabilities where possible. Always justify these permissions in commit messages and recipe comments. Keep QA checks enabled and address warnings with clear rationale.
