savedcmd_char-device.mod := printf '%s\n'   char-device.o | awk '!x[$$0]++ { print("./"$$0) }' > char-device.mod
