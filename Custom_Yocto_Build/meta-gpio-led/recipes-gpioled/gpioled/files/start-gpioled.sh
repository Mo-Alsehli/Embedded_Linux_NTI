#!/bin/bash

# Enable gpio17 && gpio27 
echo 17 > /sys/class/gpio/export
echo 27 > /sys/class/gpio/export

exec /usr/bin/gpioled