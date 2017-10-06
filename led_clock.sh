#!/bin/bash

# Run the clock on the LED matrix
LED=~pi/code/rpi-rgb-led-matrix
sudo $LED/examples-api-use/clock -f $LED/fonts/6x13B.bdf -b3 -x1 -y-1
