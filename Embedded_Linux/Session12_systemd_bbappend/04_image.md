# Image


* **When we compile our image we execute `bitbake core-image-weston`.**

## Creating a custom image:
- Inside a wanted our custom meta-layer we but the `conf/distro/distro.conf` and but all our required recipes, patchs and configurations.
- The we create a `recipe-core` folder inside the layer.
    - inside the `recipe-core` we create a folder named `image`.
    - inside the `image` folder we create `core-image-custom.bb`.

- **core-image-custom.bb** 
```
# inherit the image class
inherit image

# Set image FSTYPES with want image extensions.
IMAGE_FSTYPES:append = 