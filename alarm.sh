#!/bin/bash

# Get some passwords in the variables
source ~/toolbox/codes.bash;

# Save the current playlist
T=`tempfile`;
$MPC pause;
$MPC playlist > $T;
$MPC volume 10;

# Turn on the Marantz Audio Receiver
~/toolbox/control_receiver.sh on;
sleep 2;

# Load the playlist
$MPC clear;
$MPC load Morning;
$MPC shuffle;
$MPC play;

# Slowly increase the volume
for v in `seq 30 2 80`; do
	sleep 1;
	$MPC volume $v;
done;

# Turn off the Audio Receiver after another 5 minutes
sleep 300;
~/toolbox/control_receiver.sh off;

# Reset and restore playlist
$MPC pause;
$MPC clear;
$MPC volume 70;

# Remove the tempfile
rm $T;
