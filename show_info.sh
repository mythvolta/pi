#!/bin/bash

date
echo
HOST=`hostname -s`
IP=`hostname -I`
echo "$HOST @ $IP"
uname -a | head
echo
echo "sudo apt-get -y autoremove"
echo "sudo apt-get -y autoclean"
echo "sudo apt-get -y clean"
echo
cat ~/.ssh/authorized_keys
echo
