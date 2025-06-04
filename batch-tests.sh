#!/bin/sh


RERUNS=1


echo 'Generating Testing Binary...'
make clean
make -j

echo 'Generating Benchmark Files...'
cd benchmark
make clean all
./gen-bench


echo '=== INSERT PARALLELISM ==='
echo '--- 1 Thread ---'
for i in `seq $RERUNS`; do
	echo " -  Run $i/$RERUNS  - "
	../blink exe insert_random_req.bin
done
echo '--- 2 Threads ---'
for i in `seq $RERUNS`; do
	echo " -  Run $i/$RERUNS  - "
	../blink exe insert_random_1-of-2_req.bin insert_random_2-of-2_req.bin
done
echo '--- 4 Threads ---'
for i in `seq $RERUNS`; do
	echo " -  Run $i/$RERUNS  - "
	../blink exe \
		insert_random_1-of-4_req.bin insert_random_2-of-4_req.bin \
		insert_random_3-of-4_req.bin insert_random_4-of-4_req.bin
done
echo '--- 8 Threads ---'
for i in `seq $RERUNS`; do
	 echo " -  Run $i/$RERUNS  - "
	../blink exe \
		insert_random_1-of-8_req.bin insert_random_2-of-8_req.bin \
		insert_random_3-of-8_req.bin insert_random_4-of-8_req.bin \
		insert_random_5-of-8_req.bin insert_random_6-of-8_req.bin \
		insert_random_7-of-8_req.bin insert_random_8-of-8_req.bin
done


echo '=== SEARCH PARALLELISM ==='
echo '--- 1 Thread ---'
for i in `seq $RERUNS`; do
	echo " -  Run $i/$RERUNS  - "
	../blink exe insert_random_req.bin then search_random_req.bin
done
echo '--- 2 Threads ---'
for i in `seq $RERUNS`; do
	echo " -  Run $i/$RERUNS  - "
	../blink exe insert_random_req.bin then \
		search_random_1-of-2_req.bin search_random_2-of-2_req.bin
done
echo '--- 4 Threads ---'
for i in `seq $RERUNS`; do
	echo " -  Run $i/$RERUNS  - "
	../blink exe insert_random_req.bin then \
		search_random_1-of-4_req.bin search_random_2-of-4_req.bin \
		search_random_3-of-4_req.bin search_random_4-of-4_req.bin
done
echo '--- 8 Threads ---'
for i in `seq $RERUNS`; do
	 echo " -  Run $i/$RERUNS  - "
	../blink exe insert_random_req.bin then \
		search_random_1-of-8_req.bin search_random_2-of-8_req.bin \
		search_random_3-of-8_req.bin search_random_4-of-8_req.bin \
		search_random_5-of-8_req.bin search_random_6-of-8_req.bin \
		search_random_7-of-8_req.bin search_random_8-of-8_req.bin
done


echo '=== READ/WRITE DOMINANCE ==='
echo '--- 20% Read, 80% Write ---'
for i in `seq $RERUNS`; do
	 echo " -  Run $i/$RERUNS  - "
	../blink exe insert_random_1-of-4_req.bin then \
		search_random_1-of-5_req.bin insert_random_2-of-5_req.bin \
		insert_random_3-of-5_req.bin insert_random_4-of-5_req.bin \
		insert_random_5-of-5_req.bin
done
echo '--- 40% Read, 60% Write ---'
for i in `seq $RERUNS`; do
	 echo " -  Run $i/$RERUNS  - "
	../blink exe insert_random_1-of-4_req.bin then \
		search_random_1-of-5_req.bin search_random_2-of-5_req.bin \
		insert_random_3-of-5_req.bin insert_random_4-of-5_req.bin \
		insert_random_5-of-5_req.bin
done
echo '--- 60% Read, 40% Write ---'
for i in `seq $RERUNS`; do
	 echo " -  Run $i/$RERUNS  - "
	../blink exe insert_random_1-of-4_req.bin then \
		search_random_1-of-5_req.bin search_random_2-of-5_req.bin \
		search_random_3-of-5_req.bin insert_random_4-of-5_req.bin \
		insert_random_5-of-5_req.bin
done
echo '--- 80% Read, 20% Write ---'
for i in `seq $RERUNS`; do
	 echo " -  Run $i/$RERUNS  - "
	../blink exe insert_random_1-of-4_req.bin then \
		search_random_1-of-5_req.bin search_random_2-of-5_req.bin \
		search_random_3-of-5_req.bin search_random_4-of-5_req.bin \
		insert_random_5-of-5_req.bin
done
