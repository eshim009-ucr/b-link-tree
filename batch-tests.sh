#!/bin/sh
set -e


BATCHES=4
RERUNS=4


echo 'Generating Testing Binary...'
make clean
make -j

echo 'Generating Benchmark Files...'
cd benchmark
make clean all
./gen-bench


echo '=== INSERT & SEARCH ==='
for i in `seq $BATCHES`; do
	echo "--- Batch $i/$BATCHES ---"
	for j in `seq $RERUNS`; do
		echo " -  Run $j/$RERUNS  - "
		../blink exe "insert_random_$i-of-$(echo $BATCHES)_req.bin" then \
			"search_random_$i-of-$(echo $BATCHES)_req.bin"
	done
done


echo '=== READ/WRITE DOMINANCE ==='
for r_ratio in $(seq 20 20 80); do
	w_ratio=$((100 - $r_ratio))
	echo "--- $r_ratio% Read, $w_ratio% Write ---"
	for i in `seq $BATCHES`; do
		for j in `seq $RERUNS`; do
			echo " -  Batch $i/$BATCHES, Run $j/$RERUNS  - "
			../blink exe "insert_random_$i-of-$(echo $BATCHES)_req.bin" then \
				"mixed-rw_$r_ratio-$(echo $w_ratio)_$i-of-$(echo $BATCHES)_req.bin"
		done
	done
done
