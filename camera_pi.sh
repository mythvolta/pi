#!/bin/bash
#
# Capture a single picture from a Raspberry Pi camera
#
# crontab to run every minute, at least during daylight hours
# * 5-22 * * * $HOME/toolbox/camera_pi.sh >/dev/null

# Set some variables
DIR="$HOME/.camera"
DATE=`date +%Y-%m-%d_%H%M%S`
FILE="$DIR/${HOSTNAME}_pi_${DATE}.jpg"
OPTS="-awb off -awbg 1.5,1.2 -ISO 100 -n"

# Host-specific options
if [[ $HOSTNAME == "zerocam" ]]
then
	# Rotate by 180 degrees
	OPTS+=" -vf -hf"
elif [[ $HOSTNAME == "dragon" ]]
then
	# Rotate by 180 degrees
	OPTS+=" -vf -hf"
fi

# Capture a single image
raspistill -o $FILE $OPTS

# If fswebcam is installed, also use the USB webcam
if [[ -e "$(command -v fswebcam)" ]]
then
	sleep 10
	DATE=`date +%Y-%m-%d_%H%M%S`
	FILE="$DIR/${HOSTNAME}_logitech_${DATE}.jpg"
	fswebcam -d /dev/video1 -r 1600x1200 --no-banner $FILE
fi
