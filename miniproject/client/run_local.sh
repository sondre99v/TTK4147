#!/bin/bash

# This script tests the implementation locally

make clean > /dev/null
make COMPILER=gcc LDFLAGS=-pthread CFLAGS="-Wall -DRUN_ON_LOCALHOST" > /dev/null

if [ $? -eq 0 ]
then
	echo "Build successfull. Running tests..."
	cd ../server
	rm -rf *.dat *.png
	./miniproject-server &
	../client/main
	echo STOP > /dev/udp/127.0.0.1/9999
	gnuplot plot4
	rm -rf local
	mkdir -p local
	# mv *.dat *.png local
	# if [ $? -eq 0 ]; then
	# 	echo "yo"
	# else
	# 	echo "Cannot maybe plot results"
	# fi
	exit 0
else
	echo "Build failed."
	exit 1
fi