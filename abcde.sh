#!/bin/bash

# A lot of preferences are stored in ~/.abcde.conf

LOGFILE=/var/log/abcde.log;
date >> $LOGFILE;
echo "Arguments are ($@)" >> $LOGFILE
#abcde -d /dev/sr1 2>&1 >> $LOGFILE;
abcde -d /dev/cdrom 2>&1 >> $LOGFILE;
date >> $LOGFILE;
echo >> $LOGFILE;
