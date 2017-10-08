#!/bin/bash

# Run the clock on the LED matrix
LED=~pi/code/rpi-rgb-led-matrix
#sudo $LED/examples-api-use/clock -f $LED/fonts/6x13B.bdf -b3 -x1 -y-1
sudo .$LED/examples-api-use/clock -f $LED/fonts/6x13B.bdf -b10 -d '%H%M' --led-rows=16 -S1 -y0 -x1
