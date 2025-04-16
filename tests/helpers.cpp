extern "C" {
#include "../insert.h"
};
#include "helpers.hpp"
#include <pthread.h>


void *stride_insert(void *argv) {
	si_args args = *(si_args *)argv;
	bval_t value;
	if (args.stride > 0 && args.end > args.start) {
		for (int_fast32_t i = args.start; i <= args.end; i += args.stride) {
			value.data = -i;
			insert(args.root, i, value, memory);
		}
	} else if (args.stride < 0 && args.end < args.start) {
		for (int_fast32_t i = args.start; i >= args.end; i += args.stride) {
			value.data = -i;
			insert(args.root, i, value, memory);
		}
	}
	pthread_exit(NULL);
}
