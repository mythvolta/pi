#!/usr/bin/env python3

import time
import RPi.GPIO as GPIO
from enum import Enum

# Button is on pin 29 (GPIO 5)
gpio_pin = 5

# Define the callback function
def button_changed(pin):
    print("GPIO pin %d has state %s" % (pin, GPIO.input(pin)))
# def button_changed()

# Set the button as a pull-down input
#GPIO.setwarnings(False)
#GPIO.setmode(GPIO.BOARD) # 29
GPIO.setmode(GPIO.BCM) # 5
#GPIO.setup(gpio_pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(gpio_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
#GPIO.add_event_detect(gpio_pin, GPIO.BOTH, callback=button_changed, bouncetime=300)

#message = input("Press enter to quit\n\n") # Run until someone presses enter

# States
class State(Enum):
    Wait = 0
    First = 1
    Long = 2
    Short = 3
    Second = 4
    Third = 5
# class State

# Loop until we have a valid button press
def read_button():
    increment_ms = 100
    hold_short_ms = 500
    hold_medium_ms = 1000
    hold_long_ms = 5000
    timer = 0

    # Initialize the state machine
    s = State.Wait

    # Run foreverish
    while True:
        # Pressed
        pressed = not GPIO.input(gpio_pin)

        # Handle based on state
        if s == State.Wait:
            # When pressed, transition to First
            if pressed:
                s = State.First
                timer = 0
                continue
            # if
        elif s == State.First:
            # If it is no longer pressed, transition to Short
            if not pressed:
                s = State.Short
                timer = 0
                continue
            # If the timer runs out, transition to Long
            elif timer >= hold_short_ms:
                s = State.Long
                # Don't reset timer
            # if
        elif s == State.Long:
            # If it is no longer pressed, return based on hold time
            if not pressed:
                if timer >= hold_medium_ms:
                    return "MEDIUM"
                else:
                    return "SHORT"
                # if
            # If the timer runs out, return
            elif timer >= hold_long_ms:
                return "LONG"
            # if
        elif s == State.Short:
            # If it is pressed again, transition to Second
            if pressed:
                s = State.Second
                timer = 0
                continue
            # If it is not pressed in time, return
            elif timer >= hold_short_ms:
                return "SHORT"
            # if
        elif s == State.Second:
            # If it is released again, transition to Third
            if not pressed:
                s = State.Third
                timer = 0
                continue
            # If it is not released in time, return
            elif timer >= hold_short_ms:
                return "DOUBLE"
            # if
        elif s == State.Third:
            # If it is pressed again, return
            if pressed:
                return "TRIPLE"
            # If it is not pressed in time, return
            elif timer >= hold_short_ms:
                return "DOUBLE"
            # if
        # if

        # Increment the timer
        time.sleep(increment_ms / 1000.)
        timer += increment_ms
    # while
#def read_button()

# Read five button presses
for n in range(5):
    print("Waiting for button press #%d" % n)
    b = read_button()
    print(" returned %s" % b)
    time.sleep(0.5)
    print()
# for

# Cleanup
GPIO.cleanup()
