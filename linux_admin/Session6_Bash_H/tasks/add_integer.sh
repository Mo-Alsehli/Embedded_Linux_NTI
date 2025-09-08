#!/usr/bin/bash


add_integers() {
    declare  num1
    declare  num2
    echo "Two Integer Sum App"
    read -p "Enter First Integer: " num1
    read -p "Enter Second Integer: " num2
    
    if ! [[ "$num1" =~ ^-?[0-9]+$ ]]; then
        echo "ERROR: Please Enter an Integer"
        exit 1
    fi
    if ! [[ "$num2" =~ ^-?[0-9]+$ ]]; then
        echo "ERROR: Please Enter an Integer"
        exit 1
    fi
    echo "Sum of two integers = $((num1 + num2))"
}

add_integers