#!/bin/bash

date
echo
HOST=`hostname -s`
IP=`hostname -I`
echo "$HOST @ $IP"
uname -a | head
echo
lsb_release -a
echo
echo "sudo apt -y autoremove"
echo "sudo apt -y autoclean"
echo "sudo apt -y clean"
echo
SSH=~/.ssh/authorized_keys
echo ${SSH}:
grep -Eo ' [^ ]+$' $SSH
echo
