#!/bin/bash

# This script builds and exports the project to NGW100 and starts the server locally

if [ -f /etc/os-release ]; then
	. /etc/os-release
	if [[ $NAME == "Ubuntu" ]]; then
		echo "This is Ubuntu"
		exit 3
	fi
else
	echo "Unknown OS..."
	exit 2
fi

make 2> /dev/null

if [ $? -eq 0 ]
then
  echo "Build successfull. Exporing tests and starting server..."
  cp main /export/nfs/home/avr32/main
  cd ../server
  ./miniproject-server
  gnuplot plot4
  gnuplot plot5
  exit 0
else
  echo "Build failed." >&2
  exit 1
fi