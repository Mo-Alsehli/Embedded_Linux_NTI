# Yocto Recipes


## How to write a recipe
- First of all we need a layer to write a layer.
- To create a layer 
    - Initialize the build environment `source oe-init-build-env <path>`
    - `bitbake-layers create-layer /home/mmagdi/yocto/meta-custom-layer`
    - There will be layer (new directory) created automatically.

- **meta-mycustom-layer**
    - The created custom layer contains some directories and folders.
- **conf/layer.conf**
    - command `bitbake -e ..`
    - any thing with prefix `BB` is for `Bitbake`.
    - Auto created variables in the `layer.conf`.
    - `BBPATH` Variable: We assing the root (current) directory of the created layer to it.
    - `LAYERDIR` contains (holds) the current directory of this layer.
    - `BBFILES` it should hold your targets.
        - This variable has specific way to be assigned and the targets folders must have a specific structure depending on how we assign this variable.
    - `BBFILE_COLLECTIONS` It should be assigned with the layer name and we can change it (it can be different from the directory name).
    - `Layer Version
    - `LAYERDEPENDS- <layer-name>` layer depends on which layer.
    - `LAYERSERIES_COMPAT- <layer-name>` layer compatible on which bitbake version.
        - Note that here in the previous layers the layer name should be the name inside it's layer.conf in the variable `BBFILES_COLLECTIONS` not the directory name.

- **Generic Steps on how to write a custom Recipe Layer**
    - Clone the source code.
    - 

## What are targets and types of them.

## Recipe Target.
- The usual parameter for the `bitbake` command is a recibe `.bb` file
- `bb.plain("message")` It's used for debuggin to print a specific message.
- So Recipe consists of some sort of tasks and each task start with `do_`, and there are multiple default tasks.


## Recipe (Target) Big Picture:
- First of all we create our custom layer.
- configure the custom layer from `layer.conf`
- inside the examples/ we write our own recipe.


## How recipes are executed by `bitbake` (The sequence of the task)
- `do_fetch - do_unpack`  - *default task*
    - Fetchs the source code (repo) and unpack them if they are compressed.
    - Usually `bitbake` download the `zip` file then it unpack it.
    - The fetch and unpack heppens in two directories {WORKDIR, UNPACKDIR} -> they are actually two variables.  
    - `WORKDIR` this is the work environment to (fetch and execute all required tasks for the package)
    - To get the `workdir` path of a recipe we use this command `bitbake -e <package-name (e.g. python3)> | grep ^WORKDIR=` 

- `do_patch`
    - We use this function to modify a specific functionality of a function in the source code.
    - It can modify some lines of the source code.

- `do_configure` - `do_compile` - `do_install`
    - `do_configure`: is used when there is any type of configurations or `menuconfig` needs to be applied.
        - Takes its input from the build directory and source directory (`$S`, `$B`) and usually those variables point to the same location.
    - `do_compile`: write in it the compilation instructions if it's with make for example.
        - Takes its input from build and source directories.
    - `do_install`: where exactly you want the package to be located in the rootfs.
        - The Path of the installed is held in the `$D` (destinaiton) variable.
        - This just strcut how the app will show in the rootfs.

- `do_package`
    - It takes the destination varaible `$D` and package it as `.deb` or `.rpm`

- `do_populate-sysroot`
    - This is optional and depend on a variable named `IMAGE_INSTALL`
    - This to tell it to but the application in the rootfs in the structure that has been set using `do_install`.


## Writing a recipe from scratch (adding a target dash shell for ex):
- Create a layer and cd to it's directory to /examples and create your recipe target_*.*.bb
- `SUMMARY`, `DESCRIPTION` are just for documenting.
- `LICENCE` can be `MIT` or `CLOSED`.
* **do_fetch**
    - The `do_fetch` is implemented by setting the variable `SRC_URI`, we can do it with do_fetch(){...} but it's more complicated.
    - `SRC_URI`="<'schema'>://url;protocol="<'protocol e.g. https or ssh'>";branch="<'branch e.g. master'>""
        - schema can be: `git` for github, `file` for a local file, `http` something from internet.
    - `SRCREV`="the id of the last commit"
* **do_unpack**
    - It's implemented by default by setting the `SRC_URI` variable.

* **do_patch**
* **do_configure**

* **do_compile**
    ```
    do_compile(){
        oe_runmake -C ${S}
    }
    ```

* **do_install**
    ```
    do_install(){

    }
    ```



* Set the IMAGE_INSTALL variable with the recipe.