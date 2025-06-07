#include "operations.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>


static Request reqbuf[ENTRY_MAX];
static Response respbuf[ENTRY_MAX];


int main(int argc, int **argv) {
	FILE *fout;
	bkey_t tmp_key;
	uint_fast32_t tmp_idx;
	char strbuf[64];

	srand(0);

	fout = fopen("insert_sequential_req.bin", "wb");
	for (uint_fast32_t i = 0; i < ENTRY_MAX; ++i) {
		reqbuf[i].opcode = INSERT;
		reqbuf[i].insert.key = i+1;
		reqbuf[i].insert.value.data = -(i+1);
	}
	fwrite(reqbuf, sizeof(Request), ENTRY_MAX, fout);
	fclose(fout);

	fout = fopen("search_sequential_req.bin", "wb");
	for (uint_fast32_t i = 0; i < ENTRY_MAX; ++i) {
		reqbuf[i].opcode = SEARCH;
		reqbuf[i].search = (rand()%ENTRY_MAX) + 1;
	}
	fwrite(reqbuf, sizeof(Request), ENTRY_MAX, fout);
	fclose(fout);

	fout = fopen("insert_random_req.bin", "wb");
	for (uint_fast32_t i = 0; i < ENTRY_MAX; ++i) {
		reqbuf[i].opcode = INSERT;
		reqbuf[i].insert.key = rand();
		reqbuf[i].insert.value.data = rand();
	}
	fwrite(reqbuf, sizeof(Request), ENTRY_MAX, fout);
	fclose(fout);
	for (uint_fast8_t i = 0; i < 5; ++i) {
		sprintf(strbuf, "insert_random_%d-of-5_req.bin", i+1);
		fout = fopen(strbuf, "wb");
		fwrite(reqbuf + i*ENTRY_MAX/5, sizeof(Request), ENTRY_MAX/5, fout);
		fclose(fout);
	}
	for (uint_fast8_t i = 1; i <= 3; ++i) {
		const uint_fast8_t total = (1<<i);
		for (uint_fast8_t j = 0; j < total; ++j) {
			sprintf(strbuf, "insert_random_%d-of-%d_req.bin", j+1, total);
			fout = fopen(strbuf, "wb");
			fwrite(reqbuf + i*ENTRY_MAX/total, sizeof(Request), ENTRY_MAX/total, fout);
			fclose(fout);
		}
	}

	// Shuffle the keys from the last insert buffer
	// Insert and search place keys at the same offset
	fout = fopen("search_random_req.bin", "wb");
	for (uint_fast32_t i = 0; i < ENTRY_MAX; ++i) {
		reqbuf[i].opcode = SEARCH;
		tmp_idx = rand()%ENTRY_MAX;
		tmp_key = reqbuf[i].search;
		reqbuf[i].search = reqbuf[tmp_idx].search;
		reqbuf[tmp_idx].search = tmp_key;
	}
	fwrite(reqbuf, sizeof(Request), ENTRY_MAX, fout);
	fclose(fout);
	for (uint_fast8_t i = 0; i < 5; ++i) {
		sprintf(strbuf, "search_random_%d-of-5_req.bin", i+1);
		fout = fopen(strbuf, "wb");
		fwrite(reqbuf + i*ENTRY_MAX/5, sizeof(Request), ENTRY_MAX/5, fout);
		fclose(fout);
	}
	for (uint_fast8_t i = 1; i <= 3; ++i) {
		const uint_fast8_t total = (1<<i);
		for (uint_fast8_t j = 0; j < total; ++j) {
			sprintf(strbuf, "search_random_%d-of-%d_req.bin", j+1, 1<<i);
			fout = fopen(strbuf, "wb");
			fwrite(reqbuf + i*ENTRY_MAX/total, sizeof(Request), ENTRY_MAX/total, fout);
			fclose(fout);
		}
	}

	return 0;
}
