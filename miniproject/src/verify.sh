#!/bin/bash

make 2> /dev/null

if [ $? -eq 0 ]
then
  echo "Build successfull. Running tests..."
  ../lib/miniproject-server & ./main
  mv *.dat ../lib
  cd ../lib
  gnuplot plot4
  exit 0
else
  echo "Build failed." >&2
  exit 1
fi