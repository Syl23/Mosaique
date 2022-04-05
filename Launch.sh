#!/bin/sh
DIR="build"
if [ ! -z "$1" ]
then
	if [ "$1" = "-m" ]
	then
		make -C "$DIR" 
		if [ ! $? -eq 0 ]
		then
			exit 1
		fi
	fi
fi
./"$DIR"/Mosaique images/baboon.ppm test.ppm 100 1
