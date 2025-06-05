#!/bin/sh
set -e


RERUNS=4
# 10^X
SZ_EMIN=3
SZ_EMAX=6
SZ_MAX=$(( 10 ** $SZ_EMAX ))
SAFE_SZ_MAX=$(( 2 * $SZ_MAX ))
# 2^X
M_EMIN=2
M_EMAX=7


for m_exp in `seq $M_EMIN $M_EMAX`; do
	m=$(( 2 ** $m_exp ))
	echo ' _________________________________'
	echo '/                                 \'
	printf '| BRANCHING FACTOR of 2^%2d = %4d |\n' $m_exp $m
	echo '\_________________________________/'
	./update-defs.py $m $SAFE_SZ_MAX

	echo 'Generating Testing Binary...'
	make clean
	make -j

	echo 'Generating Benchmark Files...'
	cd benchmark
	make clean all
	./gen-bench


	echo '=== INSERT & SEARCH ==='
	for i in `seq $SZ_EMIN $SZ_EMAX`; do
		echo "--- 10^$i Entries, Sequential Insert ---"
		for j in `seq $RERUNS`; do
			echo " -  Run $j/$RERUNS  - "
			../blink exe "insert_sequential_1e$(echo $i)_req.bin" then \
				"search_sequential_1e$(echo $i)_req.bin"
		done
		echo "--- 10^$i Entries, Random Insert ---"
		for j in `seq $RERUNS`; do
			echo " -  Run $j/$RERUNS  - "
			../blink exe "insert_random_1e$(echo $i)_req.bin" then \
				"search_random_1e$(echo $i)_req.bin"
		done
	done


	echo '=== READ/WRITE DOMINANCE ==='
	for r_ratio in $(seq 20 20 80); do
		w_ratio=$((100 - $r_ratio))
		echo "--- $r_ratio% Read, $w_ratio% Write ---"
		for i in `seq $SZ_EMIN $SZ_EMAX`; do
			for j in `seq $RERUNS`; do
				echo " -  10^$i Entries, Run $j/$RERUNS  - "
				../blink exe "insert_random_1e$(echo $i)_req.bin" then \
					"mixed-rw_$r_ratio-$(echo $w_ratio)_$i-of-$(echo $BATCHES)_req.bin"
			done
		done
	done

	cd ..
done
