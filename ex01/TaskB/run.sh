make >> //dev/null

echo -n "Measuring latency __rdtsc"
time ./main -Ab

echo
echo -n "Measuring latency clock_gettime"
time ./main -Bb

echo
echo -n "Measuring latency times"
time ./main -Cb


echo
echo -n "Measuring resolution __rdtsc"
./main -A | gnuplot -p -e "plot '<cat' with boxes title '__rdtsc'"

echo
echo -n "Measuring resolution clock_gettime"
./main -B | gnuplot -p -e "plot '<cat' with boxes title 'clock_gettime()'"

echo
echo -n "Measuring resolution times"
./main -C | gnuplot -p -e "plot '<cat' with boxes title 'times()'"