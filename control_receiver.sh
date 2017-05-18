#!/bin/bash
# Turn on or off the Marantz Audio Receiver

# Ensure there is only 1 argument
if [ $# -ne 1 ]; then
	echo "$0 requires exactly 1 argument, not $#";
	exit 1;
fi;

# Ensure that the argument is either "on" or "off"
POWER_ON=0;
if [ $1 == "on" ]; then
	POWER_ON=1;
elif [ $1 != "off" ]; then
	echo "$0 only accepts [on] and [off] as arguments";
	exit 1;
fi;

# Get some passwords in the variables
source ~/.bash_aliases;

# Command to use
RECEIVER=5;

# Use the proper argument
if [ $POWER_ON -eq 1 ]; then
	# Turn on the Audio Receiver
	echo "on $RECEIVER" | $CEC;
	echo "p 1 $RECEIVER" | $CEC;
	echo "as $RECEIVER" | $CEC;
else
	# Turn off the Audio Receiver
	echo "standby 5" | $CEC;
	echo "standby $RECEIVER" | $CEC;
fi;
