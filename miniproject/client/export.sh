#!/bin/bash

# This script builds and exports the project to NGW100 and starts the server locally

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