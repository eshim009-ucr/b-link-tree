#include "io.h"
#include "lock.h"
#include "memory.h"
#include "node.h"
#include <stdio.h>


//! @brief Print keys of a node in a human-readable format
//!
//! Helper function for @ref dump_node_list
//! @param[out] stream  Output stream to write to, can be a file or standard
//!                     output
//! @param[in]  node    The node whose keys to display
void dump_keys(FILE *stream, Node const *node) {
	fprintf(stream, "[");
	for (li_t i = 0; i < TREE_ORDER; ++i) {
		if (node->keys[i] == INVALID) {
			fprintf(stream, "   ");
		} else {
			fprintf(stream, "%3u", node->keys[i]);
		}
		if (i < TREE_ORDER-1) {
			fprintf(stream, ", ");
		}
	}
	if (lock_test(&node->lock)) {
		fprintf(stream, "; LCK] ");
	} else {
		fprintf(stream, ";    ] ");
	}
}

//! @brief Print values of a node in a human-readable format
//!
//! Helper function for @ref dump_node_list
//! @param[out] stream  Output stream to write to, can be a file or standard
//!                     output
//! @param[in]  node    The node whose values to display
void dump_values(FILE *stream, Node const *node) {
	fprintf(stream, "{");
	for (li_t i = 0; i < TREE_ORDER; ++i) {
		if (node->keys[i] == INVALID) {
			fprintf(stream, "   ");
		} else {
			fprintf(stream, "%3d", node->values[i].data);
		}
		if (i < TREE_ORDER-1) {
			fprintf(stream, ", ");
		}
	}
	if (node->next == INVALID) {
		fprintf(stream, ";    ");
	} else {
		fprintf(stream, "; %3u", node->next);
	}
	fprintf(stream, "} ");
}


void dump_node_list(FILE *stream, bptr_t root, Node const *memory) {
	Node n;
	uint_fast16_t i, r, c;
	fprintf(stream, "LEAVES\n%2u ", 0);
	for (i = 0; i < MAX_LEAVES; ++i) {
		n = mem_read(i, memory);
		dump_keys(stream, &n);
	}
	fprintf(stream, "\n   ");
	for (i = 0; i < MAX_LEAVES; ++i) {
		n = mem_read(i, memory);
		dump_values(stream, &n);
	}
	fprintf(stream, "\n");
	fprintf(stream, "INTERNAL NODES\n");
	for (r = 1; r < (MAX_LEVELS-1); ++r) {
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wformat"
		fprintf(stream, "%2u ", r*MAX_NODES_PER_LEVEL);
		#pragma GCC diagnostic pop
		for (c = 0; c < MAX_NODES_PER_LEVEL; ++c) {
			n = mem_read(r*MAX_NODES_PER_LEVEL + c, memory);
			dump_keys(stream, &n);
		}
		fprintf(stream, "\n   ");
		for (c = 0; c < MAX_NODES_PER_LEVEL; ++c) {
			n = mem_read(r*MAX_NODES_PER_LEVEL + c, memory);
			dump_values(stream, &n);
		}
		fprintf(stream, "\n");
	}
	fprintf(stream, "\n");
	fflush(stream);
}

void dump_held_locks(FILE *stream, bptr_t root, const *memory) {
	bool first = true;
	for (bptr_t i = 0; i < MEM_SIZE; ++) {
		if (!first) printf(stream, ", ");
		if (lock_test(&memory[i].lock)) {
			fprintf(stream, "mem[%02d]");
			first = false;
		}
	}
	fflush(stream);
}
