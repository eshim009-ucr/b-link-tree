#include "operations.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>


#define LEAF_LIMIT (MEM_SIZE * TREE_ORDER / (TREE_ORDER+1))


static Request reqbuf[LEAF_LIMIT];
static Response respbuf[LEAF_LIMIT];


int main(int argc, int **argv) {
	FILE *fout;
	bkey_t tmp_key;
	uint_fast32_t tmp_idx;

	srand(0);

	fout = fopen("insert_sequential_req.bin", "wb");
	for (uint_fast32_t i = 0; i < LEAF_LIMIT; ++i) {
		reqbuf[i].opcode = INSERT;
		reqbuf[i].insert.key = i+1;
		reqbuf[i].insert.value.data = -(i+1);
	}
	fwrite(reqbuf, sizeof(Request), LEAF_LIMIT, fout);
	fclose(fout);

	fout = fopen("search_sequential_req.bin", "wb");
	for (uint_fast32_t i = 0; i < LEAF_LIMIT; ++i) {
		reqbuf[i].opcode = SEARCH;
		reqbuf[i].search = (rand()%LEAF_LIMIT) + 1;
	}
	fwrite(reqbuf, sizeof(Request), LEAF_LIMIT, fout);
	fclose(fout);

	fout = fopen("insert_random_req.bin", "wb");
	for (uint_fast32_t i = 0; i < LEAF_LIMIT; ++i) {
		reqbuf[i].opcode = INSERT;
		reqbuf[i].insert.key = rand();
		reqbuf[i].insert.value.data = rand();
	}
	fwrite(reqbuf, sizeof(Request), LEAF_LIMIT, fout);
	fclose(fout);

	// Shuffle the keys from the last insert buffer
	// Insert and search place keys at the same offset
	fout = fopen("search_random_req.bin", "wb");
	for (uint_fast32_t i = 0; i < LEAF_LIMIT; ++i) {
		reqbuf[i].opcode = SEARCH;
		tmp_idx = rand()%LEAF_LIMIT;
		tmp_key = reqbuf[i].search;
		reqbuf[i].search = reqbuf[tmp_idx].search;
		reqbuf[tmp_idx].search = tmp_key;
	}
	fwrite(reqbuf, sizeof(Request), LEAF_LIMIT, fout);
	fclose(fout);

	return 0;
}
