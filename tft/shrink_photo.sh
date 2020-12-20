#!/bin/bash
#
# Shrink a photo

# Use this directory
DIR="/x/ssd/camera/movie/test"

# Always use the same output filename
DIMS="240x135"
EXT="jpg"
OUT="/tmp/small.$DIMS.$EXT"

# Main function
main() {
	# Get the latest photo
	LATEST=`recent_file`

	# Shrink the photo to the correct dimensions
	convert_to_small $LATEST
}

# Get the most recent file in the director
#  ending with the given extension
recent_file() {
	ls -1 $DIR/*.$EXT | tail -1
}

# Convert a file to a smaller size
convert_to_small() {
	PHOTO=$1

	# Get the dimensions of this file
	WIDTH_HEIGHT=(`identify -format "%w %h\n" $PHOTO | head -1`)
	WIDTH=${WIDTH_HEIGHT[0]}
	HEIGHT=${WIDTH_HEIGHT[1]}

	convert $PHOTO -resize "${DIMS}^" -gravity center -crop "${DIMS}+0+0" +repage $OUT

	# Show the name of the small photo
	ls -1 $OUT
}

# Run the main function
main
