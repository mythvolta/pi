#!/bin/bash

# Connect to the Arduino
DEV=/dev/ttyACM0
RATE=115200
minicom -b $RATE -o -D $DEV
