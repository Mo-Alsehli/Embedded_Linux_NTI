#!/usr/bin/bash


echo "Simple Integer Operations"
num1=$1
num2=$2

if ! [[ "$num1" =~ ^-?[0-9]+$ ]]; then
    echo "ERROR: Please Enter an Integer"
    exit 1
fi

if ! [[ "$num2" =~ ^-?[0-9]+$ ]]; then
    echo "ERROR: Please Enter an Integer"
    exit 1
fi

read -p "Enter Type of Operation(+/-): " op

if ! [[ "$op" =~ ^[+-]$ ]]; then
    echo "ERROR: Invalid Operation"
    exit 1
fi

if [[ "$op" == "+" ]]; then
    echo "Sum of $num1 + $num2 = $((num1 + num2))"
fi

if [[ "$op" == "-" ]]; then
    echo "Subtraction of $num1 - $num2 = $((num1 - num2))"
fi