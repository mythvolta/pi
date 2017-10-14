#!/bin/bash
# Run the clock on the LED matrix
LED=~pi/code/rpi-rgb-led-matrix

# Ensure that only one version of this script runs
SCRIPT=`basename $0`
for LINE in `lsof -c $SCRIPT -F p`; do 
  if [ $$ -gt ${LINE#?} ] ; then
    exit 0
  fi
done

# Kill all old instances of mythclock before starting this one
sudo pkill -9 -f mythclock
#sudo $LED/examples-api-use/clock -f $LED/fonts/6x13B.bdf -b3 -x1 -y-1
#sudo $LED/examples-api-use/clock -f $LED/fonts/6x13B.bdf -b10 -d '%H%M' --led-rows=16 -S1 -y-2 -x0 -C 128,0,0 &
sudo $LED/eric/mythclock -f $LED/fonts/6x13B.bdf -b10 -d '%H%M' --led-rows=16 -S1 -y-2 -x0 -C 128,0,0
