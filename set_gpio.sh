#!/bin/bash

# Simple wrapper script to control fan and led

GPIO_FAN=23  # The fan is connected to GPIO23 (pin 16/gnd 14, WPi 4)
GPIO_LED=18  # The first LED is connected to GPIO18 (pin 12, WPi 1) in ALT5 mode for PWM

HELP="Arguments are (fan/led) (on/off/toggle)\n";

# Only allow one or two arguments
DEV=""
CMD=""
if [[ $# > 2 ]]
then
  echo -e $HELP
  exit 2
else
  DEV=$1
  if [[ $# == 2 ]]
  then
    CMD=$2
  fi
fi

# Check the device
PIN=0
if [[ $DEV == "fan" || $DEV == "FAN" || $DEV == "f" || $DEV == "F" ]]
then
  PIN=$GPIO_FAN
elif [[ $DEV == "led" || $DEV == "LED" || $DEV == "l" || $DEV == "L" ]]
then
  PIN=$GPIO_LED
else
  echo "Invalid device ($DEV)"
  echo -e $HELP
  exit 2
fi

# Check the command
VAL=-1
if [[ $CMD == "toggle" || $CMD == "TOGGLE" || $CMD == "t" || $CMD == "T" ]]
then
  VAL=2
elif [[ $CMD == "on" || $CMD == "ON" || $CMD == 1 ]]
then
  VAL=1
elif [[ $CMD == "off" || $CMD == "OFF" || $CMD == 0 ]]
then
  VAL=0
else
  echo "Invalid command ($CMD)"
  echo -e $HELP
  exit 2
fi

# Some directories
GPIO_BASE="/sys/class/gpio"
GPIO_DIR="$GPIO_BASE/gpio$PIN"

# Check if the directory exists, create it if necessary
if [[ ! -d $GPIO_DIR ]];
then
  echo "sudo echo $PIN > $GPIO_BASE/export"
  sudo echo $PIN > $GPIO_BASE/export
fi

# If we toggle, do something different
if [[ $VAL == 2 ]]
then
  VAL=$(< $GPIO_DIR/value)
  let VAL=(1-$VAL)
fi

# Actually finish the command
echo "Setting $DEV (GPIO $PIN) to $VAL"
echo "sudo echo out > $GPIO_DIR/direction"
sudo echo out > $GPIO_DIR/direction
echo "sudo echo $VAL > $GPIO_DIR/value"
sudo echo $VAL > $GPIO_DIR/value

exit 0
