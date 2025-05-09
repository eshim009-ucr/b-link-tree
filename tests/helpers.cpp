extern "C" {
#include "../insert.h"
#include "../memory.h"
#include "../node.h"
};
#include "helpers.hpp"
#include <assert.h>
#include <pthread.h>
#include <gtest/gtest.h>


void *stride_insert(void *argv) {
	si_args args = *(si_args *)argv;
	args.pass = true;
	bval_t value;
	if (args.stride > 0 && args.end > args.start) {
		for (int_fast32_t i = args.start; i <= args.end; i += args.stride) {
			value.data = -i;
			if (insert(args.root, i, value, memory) != SUCCESS) {
				args.pass = false;
				pthread_exit(NULL);
			}
		}
	} else if (args.stride < 0 && args.end < args.start) {
		for (int_fast32_t i = args.start; i >= args.end; i += args.stride) {
			value.data = -i;
			if (insert(args.root, i, value, memory) != SUCCESS) {
				args.pass = false;
				pthread_exit(NULL);
			}
		}
	} else {
		// Invalid arguments supplied
		// Arguments should be explicit in the test harness,
		// so this is a programmer error
		args.pass = false;
	}
	pthread_exit(NULL);
}


bool check_inserted_leaves() {
	uint_fast8_t next_val = 1;
	AddrNode node = {.addr = 0};

	while (node.addr != INVALID) {
		node.node = mem_read(node.addr, memory);
		for (li_t j = 0; j < TREE_ORDER; ++j) {
			if (node.node.keys[j] == INVALID) {
				break;
			} else {
				if (
					node.node.keys[j] != next_val ||
					node.node.values[j].data != -next_val
				) return false;
				next_val++;
			}
		}
		node.addr = node.node.next;
	}
	return true;
}
