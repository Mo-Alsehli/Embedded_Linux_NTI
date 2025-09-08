#!/usr/bin/bash




# Searching in the current directory
VAR_HID=$(find . -type f -name ".*")

for item in $VAR_HID; do
    echo "$item"
done
