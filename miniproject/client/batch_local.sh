#!/bin/bash

# This script tests the implementation locally

make clean > /dev/null
make COMPILER=gcc LDFLAGS=-pthread CFLAGS="-Wall -DRUN_ON_LOCALHOST" > /dev/null

Kp=7

if [ $? -eq 0 ]
then
	echo "Build successfull. Running tests..."
	cd ../server
	rm -rf *.dat
	for i in `seq 1 100`;
    do
    	./miniproject-server &
		../client/main $i
		echo STOP > /dev/udp/127.0.0.1/9999
		gnuplot plot4
		mv plot4.png $i.png
		echo $i
    done   
else
	echo "Build failed."
	exit 1
fi