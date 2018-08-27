make >> //dev/null

echo -n "Using sleep()"
time ./main -sleep

echo
echo -n "Busywait using clock_gettime()"
time ./main -busyclock

echo
echo -n "Busywait using times()"
time ./main -busytimes