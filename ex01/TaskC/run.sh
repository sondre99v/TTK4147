make >> //dev/null

echo
echo -n "Measuring resolution __rdtsc"
./main -A | gnuplot -p -e "plot '<cat' with boxes title '__rdtsc'"

echo
echo -n "Measuring resolution clock_gettime"
./main -B | gnuplot -p -e "plot '<cat' with boxes title 'clock_gettime()'"

echo
echo -n "Measuring resolution times"
./main -C | gnuplot -p -e "plot '<cat' with boxes title 'times()'"