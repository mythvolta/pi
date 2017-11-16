#!/bin/bash
# Listen for button presses to control the LED and fan
GPIO=~pi/toolbox/code/bin/gpio_button_led_fan

# Ensure that only one version of this script runs
SCRIPT=`basename $0`
for LINE in `lsof -c $SCRIPT -F p`; do 
  if [ $$ -gt ${LINE#?} ] ; then
    exit 0
  fi
done

# Kill all old instances of gpio_button_led_fan before starting this one
sudo pkill -9 -f gpio_button_led_fan
sudo $GPIO
