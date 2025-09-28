# Adding QT to our Image


## how to add qt
- Use `devtool`.
- in the yocto store we search for `meta-qt6` and install and add it.
- We start making it as bbappend class, so we will append and modify the `meta-qt6`.
- `inherit qt6-cmake.bbclass`.
- **Dependacies Issues**:
    - Make sure to assign the dependencies inside the recipe.
    - `DEPENDS` and `RDEPENDS`.
- Make sure to assing `wayland` or `x11` in the IMAGE_INSTALL.
- When we try deploy it the rpi will require some variables.
    - export:
        - `DISPLAY`.
        - `XDG-RUNTIME-DIR`.
        -`QT-QPA-PLATFORM=wayland`.
    - So we will make a service and set the `Environment` variable with values of these variables
    ```bash
    Environment=DISPLAY=0
    Environment=XDG-RUNTIME-DIR=/run/usr/0
    Environment=QT-QPA-PLATFORM=wayland
    # Then we start the application
    ExecStart=application
    User="created user for the qt application"
    [INSTALL]
    Wanted By = multi-user.target
    ```