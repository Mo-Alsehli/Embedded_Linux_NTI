savedcmd_supraa.mod := printf '%s\n'   supraa.o | awk '!x[$$0]++ { print("./"$$0) }' > supraa.mod
