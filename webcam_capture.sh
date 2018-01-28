#!/bin/bash
# Capture images using the built-in raspberry pi camera

# Options for captuer
QUALITY="-q 80"
SIZE="-w 1296 -h 972"
EXPOSURE="--exposure verylong" # or night
RANGE="--drc high"
OPTIONS="$QUALITY $SIZE $EXPOSURE $RANGE --stats"

# Take the photo in ranmdisk, then rsync to nfs
DATE=$(date +%Y-%m-%d_%H:%M:%S)
TEMP="/run/user/`id -u`"
CAMERA="/home/pi/camera/mushroom"
echo $DATE

# Cycle through various options
#for EFFECT in none negative solarise sketch denoise emboss oilpaint hatch gpen pastel watercolour film blur saturation colourswap washedout posterise colourpoint colourbalance cartoon
for EFFECT in none
do
  NAME="/grape_${DATE}.jpg"
  raspistill $OPTIONS --imxfx $EFFECT -o $TEMP/$NAME
  rsync -av $TEMP/$NAME $CAMERA/ && rm $TEMP/$NAME
  ls -ltrh $CAMERA/$NAME
done
