#!/usr/bin/bash

# This script is created to manage files extensions in corresponding directory.
# Nouns : 
#   directory --> string (path)
#.  path --> string (path)
#.  argument , files : string (path) , sub-directory , extension (string) , misc (directory)

# Verbs :
#  org --inputs-> path of directory.
# 1. get files by extension.
# 2. move files into directories by extension.
#  create --inputs-> extension.
# 1. create directory by extension.
#  move --input-> path file , path destination
# 1. move file


function org () {
    if [[ $# -lt 1 ]] ;then
        echo "ERROR:: Usage: org <path>"
        return 1
    fi

    DIR_PATH=$1

    ALL_FILES=$(ls "$DIR_PATH" -a)

    # Searching in the current directory
    for file in $ALL_FILES; do
        # Get files name, extensions.
        local FILE_NAME=$(basename "$file")
        local EXT="${FILE_NAME##*.}"

                echo "FILE: $FILE_NAME, EXT: $EXT"

        #if [ -f "$file" ]; then
            if [[ ($EXT == "txt" || $EXT == "jpg" || $EXT == "pdf") && "$FILE_NAME" != "$EXT" ]]; then
                create "$EXT" "$DIR_PATH"
                move "$DIR_PATH/$FILE_NAME" "$DIR_PATH/$EXT"   
            else
                create "misc"  "$DIR_PATH"
                move "$DIR_PATH/$FILE_NAME" "$DIR_PATH/misc" 
            fi
        #fi
    done
    
    return 0
}

function create(){
    if [[ $# -lt 2 ]] ;then
        echo "ERROR:: Usage: create <dir_name> <dir_path>"
        return 1
    fi

    local EXT=$1
    local DIR_PATH=$2
    local NEW_DIR="$DIR_PATH/$EXT"
    if [ ! -d "$DIR_NAME" ]; then
        echo "Creating $DIR_NAME @ $DIR_PATH"
        mkdir -p "$NEW_DIR"
    fi
    
    return 0
}

function move(){
    if [[ $# -lt 2 ]] ;then
        echo "ERROR:: Usage: move <file_name> <dir_name>"
        return 1
    fi

    local FILE_PATH=$1
    local DIST_PATH=$2
    echo "Move $FILE_PATH to $DIST_PATH"
    mv "$FILE_PATH" "$DIST_PATH"

    return 0
}


function main(){
    org "$1"
}


main "$@"
 