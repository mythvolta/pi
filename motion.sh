#!/bin/bash
# Start motion as root

# Ensure that only one version of motion is running
SCRIPT=`basename $0`
if [ $(pidof -x $SCRIPT | wc -w) -gt 2 ]; then
  exit 0
fi

# Kill all old instances of motion that may be hanging
sudo pkill -9 -f "motion -n"

# Turn off blue led on logitech camera
#uvcdynctrl -i /usr/share/uvcdynctrl/data/046d/logitech.xml
uvcdynctrl -s 'LED1 Mode' 0

# Start motion but not in daemon mode
sudo motion -n &> /dev/null
