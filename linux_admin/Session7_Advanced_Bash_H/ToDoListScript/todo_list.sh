#!/usr/bin/bash

count=0

function add(){
    if [ "$#" -lt 1 ]; then
        echo "[ERROR]: usage add <task>"
        return 1
    fi
    ((count+=1))
    echo "[ ] $1" >> todo.txt
    return 0
}

function list(){
    if [ -f "./todo.txt" ]; then
        cat -n todo.txt
    fi

    return 0
}

function markdone(){
    if [ "$#" -lt 1 ]; then
        echo "[ERROR]: usage markdonw <task_num>"
        return 1
    fi

    sed -i "$1s/\[ ]/[x]/" todo.txt

    return 0
}

function delete(){
    if [ "$#" -lt 1 ]; then
        echo "[ERROR]: usage del <task_num>"
        return 1
    fi
    ((count-=1))
    sed -i "$1d" todo.txt

    return 0
}




function main(){
    if [[ "$#" -lt 1 ]]; then
        echo "[ERROR]: Usage "
        echo "./todo_list.sh add <task>"
        echo "./todo_list.sh list"
        echo "./todo_list.sh done <task_num>"
        echo "./todo_list.sh del <task_num>"
        return 1
    fi

    # # Load Current Tasks Number.
    # if [ -f "./todo.txt" ]; then
    #     while IFS= read -r; do
    #         ((count+=1))
    #     done < ./todo.txt
    # fi
    

    if [ "$1" == "add" ]; then
        add "$2"
        return 0
    fi

    if [ "$1" == "list" ]; then
        list
        return 0
    fi

    if [ "$1" == "done" ]; then
        markdone "$2"
        return 0
    fi

    if [ "$1" == "del" ]; then
        delete "$2"
        return 0
    fi

}


main "$@"