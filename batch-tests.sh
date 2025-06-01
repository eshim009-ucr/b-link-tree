#!/bin/sh


echo 'Generating Testing Binary...'
make clean
make -j

echo 'Generating Benchmark Files...'
cd benchmark
make clean all
./gen-bench


echo '=== INSERT PARALLELISM ==='
echo '--- 1 Thread ---'
for i in {1..4}; do
	echo " -  Run $i/4  - "
	../blink exe insert_random_req.bin
done
echo '--- 2 Threads ---'
for i in {1..4}; do
	echo " -  Run $i/4  - "
	../blink exe insert_random_1-of-2_req.bin insert_random_2-of-2_req.bin
done
echo '--- 4 Threads ---'
for i in {1..4}; do
	echo " -  Run $i/4  - "
	../blink exe \
		insert_random_1-of-4_req.bin insert_random_2-of-4_req.bin \
		insert_random_3-of-4_req.bin insert_random_4-of-4_req.bin
done
echo '--- 8 Threads ---'
for i in {1..4}; do
	 echo " -  Run $i/4  - "
	../blink exe \
		insert_random_1-of-8_req.bin insert_random_2-of-8_req.bin \
		insert_random_3-of-8_req.bin insert_random_4-of-8_req.bin \
		insert_random_5-of-8_req.bin insert_random_6-of-8_req.bin \
		insert_random_7-of-8_req.bin insert_random_8-of-8_req.bin
done
