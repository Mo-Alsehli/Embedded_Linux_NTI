# Yocto Recipes — From Zero to Working Package

This README gives you a **complete, step-by-step path** to create a custom **layer** and **recipe** in Yocto, explains the **task flow**, the **key variables**, and how to make your package land in an image. It also includes **full explanations**, **common patterns** (Autotools/CMake/Make), **patching**, and **packaging** details.

---

## Solid Steps

### 0) Prerequisites

* Installed Yocto/Poky checkout (e.g., `git clone git://git.yoctoproject.org/poky`)
* Host build deps installed (compilers, libs per Yocto docs)
* Basic Git knowledge

---

### 1) Create and register your layer

1. **Initialize the build environment**

   ```bash
   source oe-init-build-env <build-dir>
   # creates <build-dir> and drops you into it with env set
   ```

2. **Create a new layer**

   ```bash
   bitbake-layers create-layer /home/mmagdi/yocto/meta-custom-layer
   ```

3. **Add the layer to the build**

   ```bash
   bitbake-layers add-layer /home/mmagdi/yocto/meta-custom-layer
   # this edits conf/bblayers.conf to include the layer path
   ```

4. **Verify**

   ```bash
   bitbake-layers show-layers
   # confirm meta-custom-layer is listed
   ```

---

### 2) Understand layer structure and `conf/layer.conf`

A newly created layer usually looks like:

```
meta-custom-layer/
├─ conf/
│  └─ layer.conf
└─ recipes-<category>/
   └─ <pkg>/
      └─ <pkg>_<version>.bb
```

**Key `layer.conf` variables (full explanations):**

* `BBFILE_COLLECTIONS`
  Logical name for your layer collection (e.g., `custom`). This is the name used elsewhere when referencing the layer. It **does not have to equal the directory name**.

* `BBFILE_PATTERN:<collection>`
  Regex that tells BitBake which files (recipes) belong to this collection. Typically set to match the layer directory.

* `BBFILE_PRIORITY:<collection>`
  Higher number wins when multiple layers provide the same recipe. Use carefully to intentionally override upstream recipes.

* `BBPATH`
  Augments BitBake’s search path. Usually set to `${LAYERDIR}`; BitBake uses it to find `conf`, classes, and files.

* `LAYERDIR`
  Absolute path to the root of this layer. Auto-set in `layer.conf`.

* `BBFILES`
  Glob(s) that list where recipe files live, e.g.:

  ```bitbake
  BBFILES += "${LAYERDIR}/recipes-*/*/*.bb \
              ${LAYERDIR}/recipes-*/*/*.bbappend"
  ```

* `LAYERVERSION:<collection>`
  Optional semantic version for your layer.

* `LAYERDEPENDS:<collection>`
  Other layers this one depends on (e.g., `core`, `openembedded-layer`). Use the **logical layer names** of those layers.

* `LAYERSERIES_COMPAT:<collection>`
  Compatible Yocto release series (e.g., `nanbield kirkstone`). Keeps your layer tied to supported versions.

**Tip:** Dump all environment variables visible to BitBake for a target and grep for something:

```bash
bitbake -e <target> | grep ^WORKDIR=
```

---

### 3) Targets in BitBake (what you pass to `bitbake`)

* **Recipes (`.bb`)**: Build a single package/library/tool (e.g., `bitbake zlib`).
* **Images (`.bb` under `recipes-core/images` or custom)**: Build a root filesystem (e.g., `bitbake core-image-minimal`).
* **SDKs**: Build extensible SDK/toolchains (e.g., `bitbake core-image-minimal -c populate_sdk`).

You will most often build **recipes** and **images**.

---

### 4) The recipe lifecycle: default task sequence

BitBake executes a series of **tasks** (functions that start with `do_`). Common default flow:
- Create a layer and cd to it's directory to /examples and create your recipe target_*.*.bb
- `SUMMARY`, `DESCRIPTION` are just for documenting.
- `LICENCE` can be `MIT` or `CLOSED`.

1. **`do_fetch`**
* **do_fetch**
    - The `do_fetch` is implemented by setting the variable `SRC_URI`, we can do it with do_fetch(){...} but it's more complicated.
    - `SRC_URI`="<'schema'>://url;protocol="<'protocol e.g. https or ssh'>";branch="<'branch e.g. master'>""
        - schema can be: `git` for github, `file` for a local file, `http` something from internet.
    - `SRCREV`="the id of the last commit"

2. **`do_unpack`**
   Unpack archives into the work area.

3. **`do_patch`**
   Apply patches listed in `SRC_URI` (e.g., `file://fix.patch`).

4. **`do_configure`**
   Prepare build system (e.g., run `./configure`, `cmake`, `meson`).

5. **`do_compile`**
   Compile the software.

6. **`do_install`**
   Install into **destination dir** `${D}` (a staging area, not your real rootfs).

7. **`do_package`**
   Split files from `${D}` into output packages (`.ipk`, `.deb`, `.rpm`) according to `PACKAGES`, `FILES:*`, etc.

8. **`do_populate_sysroot`**
   Make headers/libs available for other recipes at build time.

**Key dirs/vars during tasks:**

* `${WORKDIR}`: Working dir for this recipe’s build.
* `${S}`: Source dir (defaults to `${WORKDIR}/<unpacked>`).
* `${B}`: Build dir (often equals `${S}`, but can be separate for out-of-tree builds).
* `${D}`: Destination install root used by `do_install`.
* `${bindir}`, `${sbindir}`, `${libdir}`, `${includedir}`, `${sysconfdir}`, etc.: Standard GNU dir variables for proper install paths.

**Debug helpers:**

```bash
bitbake <pkg> -c listtasks
bitbake <pkg> -c devshell        # open a shell with env for the recipe
bitbake -e <pkg> | less          # inspect env
bb.plain("message")              # print from inside tasks
```

---

### 5) Write a recipe from scratch (example: tiny “hello” via Make)

**Directory layout**

```
meta-custom-layer/
└─ recipes-example/
   └─ hello/
      ├─ hello_1.0.bb
      └─ files/
         ├─ Makefile
         └─ hello.c
```

**`hello_1.0.bb`**

```bitbake
SUMMARY = "Minimal hello program"
DESCRIPTION = "A tiny example showing a custom Yocto recipe"
HOMEPAGE = "https://example.local/hello"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=<md5sum-of-your-license-file>"

# For local source files:
SRC_URI = "file://hello.c \
           file://Makefile \
           file://LICENSE \
"

S = "${WORKDIR}"

# If using a simple Makefile:
do_compile() {
    oe_runmake -C ${S}
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 hello ${D}${bindir}/hello
}

# Package splitting (optionally override defaults)
# Default package is ${PN}. You can define subpackages if needed.
FILES:${PN} += "${bindir}/hello"
RDEPENDS:${PN} = ""
```

**`files/hello.c`**

```c
#include <stdio.h>
int main() {
    puts("Hello from Yocto!");
    return 0;
}
```

**`files/Makefile`**

```make
all:
	$(CC) hello.c -o hello
install:
	install -d $(DESTDIR)/usr/bin
	install -m 0755 hello $(DESTDIR)/usr/bin/hello
```

**Build it**

```bash
bitbake hello
```

**Add to an image**

* Option A (quick): in `conf/local.conf`

  ```conf
  IMAGE_INSTALL:append = " hello"
  ```
* Option B (clean): in your image recipe

  ```bitbake
  IMAGE_INSTALL += "hello"
  ```

---

### 6) Fetching source from Git/HTTP and pinning revisions

**Git example**

```bitbake
SRC_URI = "git://github.com/example/dash.git;branch=master;protocol=https"
SRCREV = "abcdef1234567890deadbeefcafebabe12345678"  # commit pin
S = "${WORKDIR}/git"                                  # git fetcher uses ${WORKDIR}/git
```

**HTTP tarball with checksums**

```bitbake
SRC_URI = "https://example.org/releases/foo-1.2.3.tar.gz"
SRC_URI[sha256sum] = "0123...abcd"  # use 'sha256sum foo-1.2.3.tar.gz'
```

**Local files and patches**

```bitbake
SRC_URI += " \
    file://config.example \
    file://0001-fix-build-warning.patch \
"
```

---

### 7) Patching (`do_patch`) and file search paths

Put patches in `files/` and list them in `SRC_URI` as `file://`. BitBake applies them during `do_patch` in order listed.

If your files are not found, extend the search path:

```bitbake
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
```

---

### 8) Configure/Build patterns (Autotools, CMake, Meson, plain Make)

**Autotools**

```bitbake
inherit autotools pkgconfig

EXTRA_OECONF = "--disable-static --enable-feature-x"
# do_compile/do_install come from class; override only when needed
```

**CMake**

```bitbake
inherit cmake pkgconfig

EXTRA_OECMAKE = "-DENABLE_FEATURE_X=ON"
# S (sources) and B (build dir) are handled by cmake class
```

**Meson**

```bitbake
inherit meson pkgconfig

EXTRA_OEMESON = "-Dfeature-x=true"
```

**Plain Make**

```bitbake
do_compile() {
    oe_runmake -C ${S}
}
do_install() {
    oe_runmake -C ${S} install DESTDIR=${D} prefix=/usr
}
```

---

### 9) Installing files correctly (`do_install` and FHS dirs)

Use standard dir vars so packaging is automatic:

* `${bindir}` → `/usr/bin`
* `${sbindir}` → `/usr/sbin`
* `${libdir}` → `/usr/lib`
* `${includedir}` → `/usr/include`
* `${sysconfdir}` → `/etc`
* `${datadir}` → `/usr/share`

Example:

```bitbake
do_install() {
    install -d ${D}${sysconfdir}
    install -m 0644 ${S}/hello.conf ${D}${sysconfdir}/hello.conf

    install -d ${D}${bindir}
    install -m 0755 ${S}/hello ${D}${bindir}/hello
}
```

---

### 10) Packaging and splitting

By default, BitBake creates `${PN}` as the main package. You can create subpackages and control which files go where.

```bitbake
# Create a -dev subpackage for headers and pkgconfig
PACKAGES =+ "${PN}-dev"

FILES:${PN}       += "${bindir}/hello ${sysconfdir}/hello.conf"
FILES:${PN}-dev   += "${includedir} ${libdir}/pkgconfig"

# Runtime dependencies (installed on target)
RDEPENDS:${PN} += "bash"
# Build-time pkg-config dependencies (for other recipes)
RDEPENDS:${PN}-dev += "pkgconfig"
```

---

### 11) Making it part of the rootfs image

* Ensure your layer is in `bblayers.conf`.
* Include your package in the image:

  ```conf
  IMAGE_INSTALL:append = " hello"
  ```
* Build your image:

  ```bash
  bitbake core-image-minimal
  ```

---

### 12) Cleaning, rebuilding, and sstate

```bash
bitbake <pkg> -c clean            # remove workdir, keep sstate
bitbake <pkg> -c cleansstate      # remove workdir and sstate cache for pkg
bitbake <pkg> -f -c compile       # force rerun a task
```

---

## Full Explanations

### BitBake/Yocto key concepts

* **Layer**: A modular bundle of metadata: recipes, classes, conf. Layers stack and override one another via priority.
* **Recipe (`.bb`)**: Build instructions for a piece of software: where to fetch sources, how to configure/compile/install, and how to package outputs.
* **Classes (`.bbclass`)**: Reusable task/variable definitions you can `inherit` (e.g., `autotools`, `cmake`).
* **Configuration (`conf/*.conf`)**: Global and per-layer settings (`local.conf`, `bblayers.conf`, `layer.conf`).

### Common recipe variables (beyond those already used)

* **Identity & Versioning**

  * `PN` — Package name (defaults to recipe basename)
  * `PV` — Version (from filename `_X.Y.Z.bb` or set inside)
  * `PR` — Revision (increment when changing packaging)
* **Fetch**

  * `SRC_URI` — Where to get sources (`git://`, `http://`, `file://`)
  * `SRCREV` — Git commit to fetch
  * `SRC_URI[sha256sum]` — Tarball checksum
* **Build control**

  * `EXTRA_OECONF` — Extra args for `configure` (Autotools)
  * `EXTRA_OECMAKE` — Extra args for CMake
  * `EXTRA_OEMESON` — Extra args for Meson
* **Runtime/Build deps**

  * `DEPENDS` — Build-time dependencies (headers, libs needed to compile)
  * `RDEPENDS:${PN}` — Runtime deps installed on the target
* **Packaging**

  * `PACKAGES` — List of output packages (default `${PN}`)
  * `FILES:<pkg>` — File patterns assigned to each output package
  * `RRECOMMENDS:<pkg>` — Soft deps (recommended but not required)
* **Compatibility**

  * `COMPATIBLE_MACHINE` — Restrict recipe to certain machines
  * `INHIBIT_PACKAGE_DEBUG_SPLIT` — Control debug symbol splits

### Patching explained

* Add patch files under `recipes-*/<pkg>/<pkg>/files/`.
* Reference in `SRC_URI` as `file://0001-fix.patch`.
* BitBake auto-applies during `do_patch`. If order matters, list them in order.
* For quilt-style patch mgmt, you can use `quilt` in `do_patch` or `devtool modify`.

### Using `devtool` (fast iterative workflow)

```bash
devtool add hello https://example.org/hello-1.0.tar.gz
# Edits a workspace layer, generates a recipe scaffold

devtool modify hello
# Apply changes, patch management

devtool build hello
# Build from the workspace

devtool finish hello /home/mmagdi/yocto/meta-custom-layer
# Writes back the final recipe/patches to your layer
```

### Example: packaging software that installs with `make install`

If upstream supports `DESTDIR`:

```bitbake
do_install() {
    oe_runmake -C ${B} install DESTDIR=${D}
}
```

If not, install files manually with `install` commands as shown earlier.

### Example: a `dash` recipe skeleton (Git-based)

```bitbake
SUMMARY = "Small POSIX-compliant shell"
DESCRIPTION = "Dash is a minimal shell designed to be fast and POSIX compliant."
HOMEPAGE = "http://gondor.apana.org.au/~herbert/dash/"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://COPYING;md5=<md5>"

SRC_URI = "git://git.kernel.org/pub/scm/utils/dash/dash.git;branch=master;protocol=https"
SRCREV = "<commit-id>"
S = "${WORKDIR}/git"

inherit autotools pkgconfig

EXTRA_OECONF = "--with-libedit --disable-static"

do_install:append() {
    # If upstream install misses symlinks or configs, add here
    # install -d ${D}${base_bindir}
    # ln -sf /bin/dash ${D}${base_bindir}/sh
}

FILES:${PN} += "${base_bindir}/dash"
RDEPENDS:${PN} += "libedit"
```

Add it to an image:

```conf
IMAGE_INSTALL:append = " dash"
```

---

## Troubleshooting & Tips

* **Recipe not found?**
  Check `BBFILES`/`BBFILE_PATTERN` in `layer.conf` and that your layer is in `bblayers.conf`.

* **Files not packaged?**
  Confirm they are installed under `${D}` and that `FILES:${PN}` (or subpackages) include the correct paths.

* **Checksum mismatch for tarballs?**
  Update `SRC_URI[sha256sum]` with the correct value.

* **Wrong install paths?**
  Use GNU dir vars (`${bindir}`, `${libdir}`, …). Avoid hardcoding `/usr/...` in install commands; prefer `${D}${bindir}`, etc.

* **Force a rebuild**
  `bitbake <pkg> -c cleansstate && bitbake <pkg>`

* **List available tasks**
  `bitbake <pkg> -c listtasks`

---

## Quick Reference

* Show workdir:

  ```bash
  bitbake -e <pkg> | grep ^WORKDIR=
  ```
* Open recipe shell:

  ```bash
  bitbake <pkg> -c devshell
  ```
* Add layer:

  ```bash
  bitbake-layers add-layer /path/to/meta-custom-layer
  ```
* Build an image:

  ```bash
  bitbake core-image-minimal
  ```
