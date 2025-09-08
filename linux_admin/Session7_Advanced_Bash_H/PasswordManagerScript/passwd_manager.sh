#!/usr/bin/bash

function add(){
    if [ "$#" != 3 ]; then
        echo "[ERROR]:usage add <website> <username> <password>"
        return 1
    fi



    if [ ! -f "./pass.db" ]; then
        touch temp.txt
        echo "website:$1 | username:$2 | password:$3" > temp.txt
        gpg --output pass.db --encrypt --recipient mohamed.m.alsehli@gmail.com temp.txt
        rm temp.txt
    else
        gpg --output temp.txt --decrypt pass.db
        echo "website:$1 | username:$2 | password:$3" >> temp.txt
        gpg --output pass.db --encrypt --recipient mohamed.m.alsehli@gmail.com temp.txt
        rm temp.txt
    fi

    return 0
}

function list(){
    gpg --output /temp.txt --decrypt pass.db
    sed 's/| password:.*//' ./temp.txt
    rm temp.txt
    
    return 0
}

function get(){
    if [ "$#" -lt 1 ]; then
        echo "[ERROR]:Usage get <website>"
        return 1
    fi

    gpg --output temp.txt --decrypt pass.db
    grep "website:$1" temp.txt | sed 's/website:[^|]* | //'
    rm temp.txt

    return 0
}

function del(){
    if [ "$#" -lt 1 ]; then
        echo "[ERROR]:Usage del <website>"
        return 1
    fi
    
    gpg --output temp.txt --decrypt pass.db
    sed -i "/website:$1/d" ./temp.txt
    gpg --output pass.db --encrypt --recipient mohamed.m.alsehli@gmail.com temp.txt
    rm temp.txt
}





function main(){
    if [ "$#" -lt 1 ]; then
        echo "[ERROR]: to few arguments, usage:"
        echo "./pass.sh add <website> <username> <password>"
        echo "./pass.sh list"
        echo "./pass.sh get <website>"
        echo "./pass.sh del <website>"

        exit 1
    fi

    case "$1" in
        "add")
            shift
            add "$1" "$2" "$3"
            ;;
        "list")
            list
            ;;
        "get")
            shift
            get "$1"
            ;;
        "del")
            shift
            del "$1"
            ;;
        *)
            echo "[ERROR]: Invalid Operation"
            exit 1
            ;;
    esac

    return 0
}

main "$@"