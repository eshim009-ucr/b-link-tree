#include "operations.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>


uint_fast64_t int_pow(uint_fast16_t base, uint_fast8_t exp) {
	uint_fast64_t result = 1;
	for (uint_fast8_t i = 0; i < exp; ++i) {
		result *= base;
	}
	return result;
}


const uint_fast8_t MIN_EXP = 3;
const uint_fast8_t MAX_EXP = 6;
#define MAX 1000000


static Request reqbuf[MAX];
static Response respbuf[MAX];


int main(int argc, int **argv) {
	FILE *fout;
	bkey_t tmp_key;
	uint_fast32_t tmp_idx;
	char strbuf[64];

	srand(0);

	// Sequential Insert
	printf("Generating Sequential Insert Cases...\n");
	for (uint_fast32_t i = 0; i < MAX; ++i) {
		reqbuf[i].opcode = INSERT;
		reqbuf[i].insert.key = i+1;
		reqbuf[i].insert.value.data = -(i+1);
	}
	for (uint_fast8_t i = MIN_EXP; i <= MAX_EXP; ++i) {
		sprintf(strbuf, "insert_sequential_1e%d_req.bin", i);
		fout = fopen(strbuf, "wb");
		fwrite(reqbuf, sizeof(Request), int_pow(10,i), fout);
		fclose(fout);
	}
	// Sequential Search
	// Search keys is at same offset
	printf("Generating Sequential Search Cases...\n");
	for (uint_fast32_t i = 0; i < MAX; ++i) {
		reqbuf[i].opcode = SEARCH;
	}
	for (uint_fast8_t i = MIN_EXP; i <= MAX_EXP; ++i) {
		sprintf(strbuf, "search_sequential_1e%d_req.bin", i);
		fout = fopen(strbuf, "wb");
		fwrite(reqbuf, sizeof(Request), int_pow(10,i), fout);
		fclose(fout);
	}

	// Random Insert
	printf("Generating Random Insert Cases...\n");
	for (uint_fast32_t i = 0; i < MAX; ++i) {
		reqbuf[i].opcode = INSERT;
		reqbuf[i].insert.key = rand();
		reqbuf[i].insert.value.data = rand();
	}
	for (uint_fast8_t i = MIN_EXP; i <= MAX_EXP; ++i) {
		sprintf(strbuf, "insert_random_1e%d_req.bin", i);
		fout = fopen(strbuf, "wb");
		fwrite(reqbuf, sizeof(Request), int_pow(10,i), fout);
		fclose(fout);
	}
	// Random Search
	printf("Generating Random Search Cases...\n");
	for (uint_fast32_t i = 0; i < MAX; ++i) {
		reqbuf[i].opcode = SEARCH;
	}
	for (uint_fast8_t i = MIN_EXP; i <= MAX_EXP; ++i) {
		sprintf(strbuf, "search_sequential_1e%d_req.bin", i);
		fout = fopen(strbuf, "wb");
		fwrite(reqbuf, sizeof(Request), int_pow(10,i), fout);
		fclose(fout);
	}

	// Read-Write
	printf("Generating Random Mixed Cases...\n");
	for (uint_fast8_t ratio = 20; ratio <= 80; ratio += 20) {
		for (uint_fast32_t i = 0; i < MAX; ++i) {
			if (rand() % 100 < ratio) {
				reqbuf[i].opcode = SEARCH;
				reqbuf[i].search = rand();
			} else {
				reqbuf[i].opcode = INSERT;
				reqbuf[i].insert.key = rand();
				reqbuf[i].insert.value.data = rand();
			}
		}
		for (uint_fast8_t i = MIN_EXP; i <= MAX_EXP; ++i) {
			sprintf(strbuf, "mixed-rw_%d-%d_1e%d_req.bin", ratio, 100-ratio, i);
			fout = fopen(strbuf, "wb");
			fwrite(reqbuf, sizeof(Request), int_pow(10,i), fout);
			fclose(fout);
		}
	}

	return 0;
}
