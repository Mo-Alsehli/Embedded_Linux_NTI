savedcmd_platform_device.mod := printf '%s\n'   platform_device.o | awk '!x[$$0]++ { print("./"$$0) }' > platform_device.mod
