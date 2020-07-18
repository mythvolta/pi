#!/bin/bash
#
# Capture a single picture from a Raspberry Pi camera
#
# crontab to run every minute, at least during daylight hours
# * 5-22 * * * $HOME/toolbox/camera_pi.sh >/dev/null

# Set some variables
DIR="$HOME/.camera"
DATE=`date +%Y-%m-%d_%H%M%S`
FILE="${HOSTNAME}_pi_${DATE}.jpg"
LINK="movie/${HOSTNAME}_pi_latest.jpg"
OPTS="-ex auto -awb off -awbg 1.5,1.2 -n"

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

# Capture a single image, and make a link
raspistill -o "$DIR/$FILE" $OPTS
ln -sf "../$FILE" "$DIR/$LINK"

# If fswebcam is installed, also use the USB webcam
if [[ -e "$(command -v fswebcam)" ]]
then
	sleep 10
	DATE=`date +%Y-%m-%d_%H%M%S`
	FILE="${HOSTNAME}_logitech_${DATE}.jpg"
	fswebcam -d /dev/video1 -r 1600x1200 --no-banner "$DIR/$FILE"
fi
