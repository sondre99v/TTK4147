#!/bin/bash

# This script tests the implementation locally by iterating arguments

cd $(dirname $0)

for i in `seq 50 60`;
do
	./run_local.sh $i $i
	cd ../server
	mkdir -p batch
	mv *.png *.dat batch
	cd ../client
done
