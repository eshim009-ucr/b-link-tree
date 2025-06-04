#include "operations.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>


#define ENTRY_LIMIT (MAX_LEAVES * TREE_ORDER)


// Generate subdivided request packes for up to 2^N
const uint_fast8_t MAX_PARALLELISM = 4;


static Request reqbuf[ENTRY_LIMIT];
static Response respbuf[ENTRY_LIMIT];


int main(int argc, int **argv) {
	FILE *fout;
	bkey_t tmp_key;
	uint_fast32_t tmp_idx;
	char strbuf[64];

	srand(0);

	// Sequential
	fout = fopen("insert_sequential_req.bin", "wb");
	for (uint_fast32_t i = 0; i < ENTRY_LIMIT; ++i) {
		reqbuf[i].opcode = INSERT;
		reqbuf[i].insert.key = i+1;
		reqbuf[i].insert.value.data = -(i+1);
	}
	fwrite(reqbuf, sizeof(Request), ENTRY_LIMIT, fout);
	fclose(fout);

	fout = fopen("search_sequential_req.bin", "wb");
	for (uint_fast32_t i = 0; i < ENTRY_LIMIT; ++i) {
		reqbuf[i].opcode = SEARCH;
		reqbuf[i].search = (rand()%ENTRY_LIMIT) + 1;
	}
	fwrite(reqbuf, sizeof(Request), ENTRY_LIMIT, fout);
	fclose(fout);

	// Random
	fout = fopen("insert_random_req.bin", "wb");
	for (uint_fast32_t i = 0; i < ENTRY_LIMIT; ++i) {
		reqbuf[i].opcode = INSERT;
		reqbuf[i].insert.key = rand();
		reqbuf[i].insert.value.data = rand();
	}
	fwrite(reqbuf, sizeof(Request), ENTRY_LIMIT, fout);
	fclose(fout);
	for (uint_fast8_t i = 0; i < 5; ++i) {
		sprintf(strbuf, "insert_random_%d-of-5_req.bin", i+1);
		fout = fopen(strbuf, "wb");
		fwrite(reqbuf + i*ENTRY_LIMIT/5, sizeof(Request), ENTRY_LIMIT/5, fout);
		fclose(fout);
	}
	for (uint_fast8_t i = 1; i <= MAX_PARALLELISM; ++i) {
		const uint_fast8_t total = (1<<i);
		for (uint_fast8_t j = 0; j < total; ++j) {
			sprintf(strbuf, "insert_random_%d-of-%d_req.bin", j+1, total);
			fout = fopen(strbuf, "wb");
			fwrite(reqbuf + i*ENTRY_LIMIT/total, sizeof(Request), ENTRY_LIMIT/total, fout);
			fclose(fout);
		}
	}

	// Shuffle the keys from the last insert buffer
	// Insert and search place keys at the same offset
	fout = fopen("search_random_req.bin", "wb");
	for (uint_fast32_t i = 0; i < ENTRY_LIMIT; ++i) {
		reqbuf[i].opcode = SEARCH;
		tmp_idx = rand()%ENTRY_LIMIT;
		tmp_key = reqbuf[i].search;
		reqbuf[i].search = reqbuf[tmp_idx].search;
		reqbuf[tmp_idx].search = tmp_key;
	}
	fwrite(reqbuf, sizeof(Request), ENTRY_LIMIT, fout);
	fclose(fout);
	for (uint_fast8_t i = 0; i < 5; ++i) {
		sprintf(strbuf, "search_random_%d-of-5_req.bin", i+1);
		fout = fopen(strbuf, "wb");
		fwrite(reqbuf + i*ENTRY_LIMIT/5, sizeof(Request), ENTRY_LIMIT/5, fout);
		fclose(fout);
	}
	for (uint_fast8_t i = 1; i <= MAX_PARALLELISM; ++i) {
		const uint_fast8_t total = (1<<i);
		for (uint_fast8_t j = 0; j < total; ++j) {
			sprintf(strbuf, "search_random_%d-of-%d_req.bin", j+1, 1<<i);
			fout = fopen(strbuf, "wb");
			fwrite(reqbuf + i*ENTRY_LIMIT/total, sizeof(Request), ENTRY_LIMIT/total, fout);
			fclose(fout);
		}
	}

	// Read-Write
	for (uint_fast8_t ratio = 20; ratio <= 80; ratio += 20) {
		const uint_fast32_t READ_MAX = ENTRY_LIMIT / (ratio/20);
		const uint_fast32_t WRITE_MAX = ENTRY_LIMIT - READ_MAX;
		uint_fast32_t read_count = 0;
		uint_fast32_t write_count = 0;
		sprintf(strbuf, "mixed-rw_%d-%d_req.bin", ratio, 100-ratio);
		fout = fopen(strbuf, "wb");
		for (uint_fast32_t i = 0; i < ENTRY_LIMIT; ++i) {
			if (rand() % 100 < ratio && read_count < READ_MAX) {
				reqbuf[i].opcode = SEARCH;
				reqbuf[i].search = rand();
				read_count++;
			} else {
				reqbuf[i].opcode = INSERT;
				reqbuf[i].insert.key = rand();
				reqbuf[i].insert.value.data = rand();
				write_count++;
			}
		}
		fwrite(reqbuf, sizeof(Request), ENTRY_LIMIT, fout);
		fclose(fout);
		for (uint_fast8_t i = 1; i <= MAX_PARALLELISM; ++i) {
			const uint_fast8_t total = (1<<i);
			for (uint_fast8_t j = 0; j < total; ++j) {
				sprintf(strbuf, "mixed-rw_%d-%d_%d-of-%d_req.bin", ratio, 100-ratio, j+1, 1<<i);
				fout = fopen(strbuf, "wb");
				fwrite(reqbuf + i*ENTRY_LIMIT/total, sizeof(Request), ENTRY_LIMIT/total, fout);
				fclose(fout);
			}
		}
	}

	return 0;
}
