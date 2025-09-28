# Image Generation with Yocto

Yocto applies the concept of Docker-like isolation when generating images.
It creates a controlled environment to ensure builds are reproducible and independent of host system tools.
Even if a tool exists on your host PC, Yocto will fetch and use its own version inside the build environment.

---

## `bitbake` Tool

* **Bitbake** is the primary build tool in Yocto, responsible for executing recipes.
* A **recipe** defines:

  * The steps needed to build software (compilation, configuration, packaging).
  * Metadata about the software (version, source, dependencies, patches).

Recipes form the foundation of image generation in Yocto.

---

## OpenEmbedded Architecture Workflow (Yocto Model)

The Yocto Project uses the [Layer Model](https://docs.yoctoproject.org/overview-manual/yp-intro.html#the-yocto-project-layer-model), which organizes the build process into well-defined stages:

### 1. **User Configurations**

* `layer.conf`: Defines layer-specific settings and priorities.
* `bblayers.conf`: Lists all the layers included in the build.

### 2. **Bitbake Execution**

* **Source Fetching** – Retrieves required sources and tools.
* **Patch Application** – Applies modifications and bug fixes.
* **Package Management** – Splits software into packages, handles dependencies.

### 3. **Quality Assurance (QA) Tests**

* Ensures built packages and images meet defined policies.
* Runs automated checks for licensing, dependencies, and build integrity.

---

This structured workflow ensures reproducible, modular, and policy-compliant image generation across diverse embedded Linux systems.
