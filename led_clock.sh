#!/bin/bash
# Run the clock on the LED matrix
P="~pi/toolbox/code/mythclock"

# Ensure that only one version of this script runs
SCRIPT=`basename $0`
for LINE in `lsof -c $SCRIPT -F p`; do 
  if [ $$ -gt ${LINE#?} ] ; then
    exit 0
  fi
done

# Kill all old instances of mythclock before starting this one
sudo pkill -9 -f mythclock
sudo $P/mythclock >> $P/mythclock.log
