#!/bin/bash

# This script tests the implementation locally

control_c() {
   echo STOP > /dev/udp/127.0.0.1/9999
}

trap control_c SIGINT

PLOT_NEWNAME=${2:-plot4}

make clean > /dev/null
make local > /dev/null

if [ $? -eq 0 ]
then
	echo "Build successfull. Running tests..."
	cd ../server
	rm -rf *.dat *.png
	./miniproject-server &
	../client/main $1
	echo STOP > /dev/udp/127.0.0.1/9999
	gnuplot plot4-hd
	if ! [ -z ${2+x} ]
	then
		mv plot4.png $PLOT_NEWNAME.png
	fi
else
	echo "Build failed."
	exit 1
fi
