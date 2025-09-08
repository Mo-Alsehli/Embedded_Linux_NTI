#!/usr/bin/bash



function copy(){

    local TIMESTAMP=$(date +%Y-%m-%d-%H-%M)
    local BACKUP_PATH="./backup"
    local DIR_PATH="$1"
    # Check Directory Path Existance.
    if [ ! -d "$DIR_PATH" ]; then
        echo "ERROR:: $DIR_PATH no such a directory"
        return 1
    fi

    # Check for the Backup directory and create if not found.
    if [ ! -d "$BACKUP_PATH" ]; then
        echo "Creating Backup Directory"
        mkdir -p backup
    fi

    local DIR_BACKUP_NAME="$(basename  "$DIR_PATH")-backup-$TIMESTAMP.tar.gz"

    # Compress the directory.
    tar czf "$BACKUP_PATH/$DIR_BACKUP_NAME" "$DIR_PATH"

    # Check if Backup folder has more than 5 tars and delete oldest.
    check_delete $BACKUP_PATH

}



function check_delete(){
    local BACKUP_PATH=$1
    local count=0
    
    # Searching in the current directory
    for file in "$BACKUP_PATH/"*; do
        (( count+=1 ))
    done

    for f in $(ls -t --reverse "$BACKUP_PATH" | head -n "$((count-5))"); do
        echo "Removing $f @ $BACKUP_PATH/"
        rm -r "$BACKUP_PATH/$f"
    done

    return 0    
}





function main(){
    if [[ "$#" -lt 1 ]]; then
        echo "ERROR:: Usage: ./backup.sh <Path>"
        exit 1
    fi
    copy "$1"
    return 0
}


main "$@"
