# Image Generation

**Yocto applies the idea of Docker as it makes an isolated environment to generate the image**
- So even if there is some tool it uses to generates the image and this tool is on your host PC it will download and use another one.


## `bitbake` Tool.
- `bitbake` tool is based on recipes.
- The `recipe` is the steps or configuration that helps in image generation
- The `recipe` provides details about specific software.


## Open Embedded Architecture Workflow [Yocto Model](https://docs.yoctoproject.org/overview-manual/yp-intro.html#the-yocto-project-layer-model)
- **User Configurations**
    - layer.conf
    - bblayer.conf
- **Bitbake**
    - Source Fetching (fetchs needed build tools).
    - Patch (midification) apply.
    - Package Managing ().
- **QA Tests**
    - Based on specified polciy.