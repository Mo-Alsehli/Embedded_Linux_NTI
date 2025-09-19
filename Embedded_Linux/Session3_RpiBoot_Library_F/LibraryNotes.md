# Libraries in Embedded Linux

Libraries in Linux (and Embedded Linux) provide reusable code that applications can link against. There are two main types: **static** and **dynamic (shared)** libraries.

---

## Static Libraries

> A **static library** is linked into the application at **compile time**. All symbols are resolved before the executable is produced.

### Characteristics

* ✅ **Fast execution time** (no runtime symbol resolution).
* ❌ **Large executable size** (library code is copied into each binary).
* ❌ **Duplication in memory**:

  * If two applications use the same static library, each has its own copy of the code in memory.
  * This wastes RAM and storage.

---

## Dynamic (Shared) Libraries

> A **dynamic library** (shared object) is linked at **runtime** by the system loader (`ld-linux.so`). Only part of the symbol information is resolved at compile time.

### Characteristics

* ✅ **Smaller executable size** (the library is not embedded in the binary).
* ✅ **Memory efficiency**: multiple applications can share one instance of the library in RAM.
* ❌ **Slightly slower execution** at startup due to runtime symbol resolution.
* ✅ Only one copy of the library needs to be **installed on the system** (usually in `/lib` or `/usr/lib`).

---

## Memory Layout Comparison

### Static Linking

Each application has its own copy of the library in memory (duplication).

```
[ App1 Code | Library Code ]
[ App2 Code | Library Code ]
```

### Dynamic Linking

Applications share a single copy of the library in memory (efficient).

```
[ App1 Code ]        ┐
                     ├──> [ Shared Library Code ]
[ App2 Code ]        ┘
```

*(Illustration: static duplicates library per process, while dynamic loads one copy shared among all.)*

---

## Creating a Static Library

1. **Write source files** (e.g. `file1.c`, `file2.c`).

2. **Compile object files** (without linking):

   ```bash
   gcc -c file1.c file2.c
   ```

3. **Archive into a static library** using `ar`:

   ```bash
   ar -rcs libmylib.a file1.o file2.o
   ```

4. **Link against it** when building an application:

   ```bash
   gcc main.c -L. -lmylib -o app
   ```

   > The `-L.` tells the compiler to look in the current directory, and `-lmylib` links `libmylib.a`.

---

## Creating a Dynamic (Shared) Library

1. **Write source files** (e.g. `file1.c`, `file2.c`).

2. **Compile with Position-Independent Code (PIC)**:

   ```bash
   gcc -fPIC -c file1.c file2.c
   ```

3. **Create a shared library** (`.so` file):

   ```bash
   gcc -shared -o libmylib.so file1.o file2.o
   ```

4. **Link against it** when building an application:

   ```bash
   gcc main.c -L. -lmylib -o app
   ```

---

## System Loader and Dynamic Linking

* At runtime, the **dynamic loader** (`ld-linux.so` on ARM/Linux) loads the required shared libraries.

* The loader searches in:

  1. **Default system paths** (`/lib`, `/usr/lib`).
  2. Paths specified in `/etc/ld.so.conf`.
  3. The environment variable `LD_LIBRARY_PATH`.

* You can inspect dynamic dependencies with:

  ```bash
  ldd app
  ```
