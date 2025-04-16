#ifndef TESTS_HELPERS_HPP
#define TESTS_HELPERS_HPP


#include "../types.h"


typedef struct Node Node;

struct si_args {
	int_fast32_t start;
	int_fast32_t end;
	int_fast32_t stride;
	bptr_t *root;
};


extern Node memory[MEM_SIZE];

void *stride_insert(void *argv);


#endif
