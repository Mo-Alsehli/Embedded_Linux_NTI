#!/usr/bin/bash

echo "Enter File Path to Tokenize: "
read path

ext="${path##*.}"
ext=".$ext"

filename=$(basename "$path" "$ext")


echo "Filename = $filename, ext = $ext"
