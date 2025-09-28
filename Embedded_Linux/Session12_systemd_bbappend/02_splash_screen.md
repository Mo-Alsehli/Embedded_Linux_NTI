# RPI Splash Screen

* **We need to change our splash screen for custom applications**


## `SPLASH_IMAGE`
- We will modify this variable in an another append for the psplash-git.bb
- we create a layer `meta-layer` and create a recipe `recipe-core`.
    - create `psplash-git.bbappend`
    - `mkdir files` and load custom splash screen to it.
    - ```
    psplash-git.bbappend
    SPLASH_IMAGE = "file://picture.pin&outprefix=" // search what you have to add after the image.
    ```
    - In the `distro.conf` We add `IMAGE_FEATURE:append = " splash"`.