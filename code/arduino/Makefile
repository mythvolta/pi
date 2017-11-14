# Simple Makefile for Arduino

# Board settings
#BOARD      := pro5v
#BOARD      := pro
BOARD      := uno
ARDUINODIR := /usr/share/arduino
#SERIALDEV  := /dev/ttyUSB0
SERIALDEV  := /dev/ttyACM0

# Actually use arduino.mk
include ./arduino.mk

# Reset the board with this command
reset: ino/reset.ino
	-mv *.ino ino/
	-rsync -avv ino/reset.ino .
	make -j4
	make upload
	rm -f reset.ino *.o *.hex
