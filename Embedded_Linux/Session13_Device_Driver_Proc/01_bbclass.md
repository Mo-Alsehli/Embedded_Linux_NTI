# `bbclasses` in Yocto

## Overview

* A **`.bbclass`** file in Yocto is a **reusable and abstract build class** that encapsulates common functionality, variables, or functions that can be shared across multiple recipes.
* They act as **blueprints** for recipes (`.bb` files) to simplify and standardize build processes.
* Classes are stored in the **`classes/`** directory within your custom layer and are **inherited** using the `inherit` keyword inside recipes.

---

## Types of Classes

Yocto provides many predefined classes such as `cmake`, `autotools`, `kernel`, etc., and you can create your own:

1. **Recipe Classes** — For general applications and libraries (e.g., `cmake`, `autotools`).
2. **Kernel Module Classes** — For drivers or modules (e.g., `module.bbclass`).
3. **Generic Classes** — For reusable, custom logic that can be shared across layers.

---

## Writing a Custom `.bbclass`

When creating your own class:

* Define variables and functions exactly as in a recipe.
* Avoid modifying or overriding Yocto’s core variables directly.
* Store your `.bbclass` inside the `classes/` directory of your layer, e.g.:

  ```
  meta-yourlayer/
  └── classes/
      └── hello.bbclass
  ```

---

### Example: `hello.bbclass`

```bash
HELLO ?= "Hello from generic class"
MY_FILE ?= "hello.txt"
HELLO_DIR ?= "hello"

# Custom function to create a text file
do_hello() {
    echo "${HELLO}" > ${B}/${MY_FILE}
}

# Register the task in the build sequence
addtask hello before do_compile after do_patch

# Extend the install process
do_install:append() {
    install -d ${D}${datadir}/${HELLO_DIR}
    install -m 0644 ${B}/${MY_FILE} ${D}${datadir}/${HELLO_DIR}
}

# Ensure the files are packaged
FILES:${PN}:append = " ${datadir}/${HELLO_DIR}/ ${datadir}/${HELLO_DIR}/${MY_FILE}"
```

---

## Using the `.bbclass` in a Recipe

To apply this class in a recipe:

```bash
SUMMARY = "Hello world recipe using custom bbclass"
LICENSE = "CLOSED"
inherit hello

HELLO = "Hello Yocto World"
MY_FILE = "yocto_message.txt"
HELLO_DIR = "my_hello"
```

This recipe will:

1. Inherit all functions from `hello.bbclass`.
2. Replace variable defaults with its own.
3. Execute `do_hello` before `do_compile` to generate the file.

---

## Key Difference: `.bbclass` vs `.bbappend`

| Feature      | `.bbclass`                                                | `.bbappend`                              |
| ------------ | --------------------------------------------------------- | ---------------------------------------- |
| **Purpose**  | Reusable and generic functionality shared by many recipes | Modify or extend an existing recipe      |
| **Scope**    | Global — usable by any recipe via `inherit`               | Local — affects only one specific recipe |
| **Location** | `classes/` directory in your layer                        | Same path and name as target recipe      |
| **Usage**    | `inherit <class-name>`                                    | `<recipe>%.bbappend`                     |

---

## Summary

* **`.bbclass`** = reusable abstraction, encourages modular design.
* Store it in the `classes/` folder of your layer.
* Use `inherit` in recipes to apply its logic.
* Combine it with Yocto’s predefined classes (`cmake`, `autotools`, etc.) for complex build automation.
* Never override core variables; extend with `:append` or `:prepend` to maintain layer integrity.
