#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# photo.py
#
# Show a photo on the TFT display

from PIL import Image, ImageDraw
import board
import time
import digitalio
#import adafruit_rgb_display.ili9341 as ili9341
import adafruit_rgb_display.st7789 as st7789  # pylint: disable=unused-import
#import adafruit_rgb_display.hx8357 as hx8357  # pylint: disable=unused-import
#import adafruit_rgb_display.st7735 as st7735  # pylint: disable=unused-import
#import adafruit_rgb_display.ssd1351 as ssd1351  # pylint: disable=unused-import
#import adafruit_rgb_display.ssd1331 as ssd1331  # pylint: disable=unused-import

# Set some pins
cs_pin = digitalio.DigitalInOut(board.CE0)
dc_pin = digitalio.DigitalInOut(board.D25)
reset_pin = digitalio.DigitalInOut(board.D24)
backlight_pin = digitalio.DigitalInOut(board.D26)

# Config for display baudrate (default max is 24mhz):
BAUDRATE = 24000000
 
# Set up SPI bus using hardware SPI
spi = board.SPI()

# Set up the display
disp = st7789.ST7789(spi, rotation=90, width=135, height=240, cs=cs_pin, dc=dc_pin, rst=reset_pin, baudrate=BAUDRATE)
print("Image dimesions are %02dx%02d" % (disp.width, disp.height))

# Create blank image for drawing
if disp.rotation % 180 == 90:
    # Portrait
    height = disp.width
    width = disp.height
else:
    # Landscape
    width = disp.width
    height = disp.height
# if
print("New dimesions are %02dx%02d" % (width, height))

image = Image.new("RGB", (width, height))
draw = ImageDraw.Draw(image)

# Draw a black filled box to clear the image
draw.rectangle((0, 0, width, height), outline=0, fill=(0, 0, 0))
disp.image(image)

# Scale and show the image
image = Image.open("blinka.jpg")

# Scale the image to the smaller screen dimension
image_ratio = image.width / image.height
screen_ratio = width / height
if screen_ratio < image_ratio:
    scaled_width = image.width * height // image.height
    scaled_height = height
else:
    scaled_width = width
    scaled_height = image.height * width // image.width
# if
image = image.resize((scaled_width, scaled_height), Image.BICUBIC)

# Crop and center the image
x = scaled_width // 2 - width // 2
y = scaled_height // 2 - height // 2
image = image.crop((x, y, x + width, y + height))
 
# Display the image
disp.image(image)

# Flash the backlight_pin on and off
backlight_pin.switch_to_output()
isBacklight = True
backlight_pin.value = isBacklight

for i in range(10):
    time.sleep(2)
    isBacklight = not isBacklight
    print("%02d : %0d" % (i, isBacklight))
    backlight_pin.value = isBacklight
# for
