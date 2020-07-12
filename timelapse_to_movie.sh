#!/bin/bash
#
# Create movie from all timelapse images,
#  one movie per day per camera

# Set the command to run
#FFMPEG="nice -n 7 ffmpeg -v quiet -framerate 30 -c:v libx264 -vf yadif -r 30"
FFMPEG="nice -n 7 ffmpeg -v quiet -threads 0 -y -r 30"
OPTS="-c:v libx264 -crf 20 -preset slow"

# Get yesterday's date
YESTERDAY=`date -d "yesterday" +%Y-%m-%d`
echo
date
echo

# Move to the camera directory
cd /x/ssd/camera/

# First two parts are server and camera name
CAMERAS=`ls -1 *.jpg | cut -d'_' -f1-2 | uniq`
for C in $CAMERAS
do
	echo "[$C]"

	# Make a single movie per day per camera
	DAY=`ls -1 ${C}*.jpg | cut -d'_' -f3 | uniq`
	for D in $DAY
	do
		# Only run for yesterday
		if [[ $D != $YESTERDAY ]]
		then
			#echo "Skipping, today is $D"
			continue
		fi
		
		# Start timer
		X=`date`
		echo "  $X"

		# Show the number of files
		F="${C}_${D}"
		N=`ls -1 ${F}*.jpg | wc -l`
		echo "  $D: $N files"

		# Actually transcode
		OUT="movie/timelapse_${F}.mp4"
		echo "  Generating $OUT"
		$FFMPEG -pattern_type glob -i "${F}*.jpg" $OPTS $OUT
		if [[ $? -ne 0 ]]
		then
			echo "  Failed!"
			exit 1
		fi
		date

		# Remove the files
		rm ${F}*.jpg
		N=`ls -1 ${F}*.jpg | wc -l`
		echo "  $D: $N files"

		# Stop timer
		X=`date`
		echo "  $X"

		echo
	done
	echo
done

cd - > /dev/null
