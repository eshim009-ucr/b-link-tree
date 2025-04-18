#include "memory.h"
#include "lock.h"
#include "node.h"
#include <assert.h>
#include <string.h>


Node mem_read(bptr_t address, Node const *memory) {
	assert(address < MEM_SIZE);
	return memory[address];
}

//! The HLS interface to HBM does not support atomic test-and-set operations.
//! Atomic test-and-set operations within the FPGA fabric are allowed.
//! Serializing test-and-set operations in memory with mutual exclusion ensures
//! that race conditions do not lead to unexpected concurrent modification.
//!
//! @todo Set up multiple locks for specific regions of memory, such as by
//! address ranges or hashes to allow higher write bandwidth.
Node mem_read_lock(bptr_t address, Node *memory) {
	static lock_t local_readlock = LOCK_INIT;
	Node tmp;

	assert(address < MEM_SIZE);
	// Read the given address from main memory until its lock is released
	// Then grab the lock
	do {
		lock_p(&local_readlock);
		tmp = memory[address];
		if (test_and_set(&tmp.lock) == 0) {
			break;
		} else {
			lock_v(&local_readlock);
		}
	} while(true);
	// Write back the locked value to main memory
	memory[address] = tmp;
	// Release the local lock for future writers
	lock_v(&local_readlock);
	return tmp;
}

void mem_write_unlock(AddrNode *node, Node *memory) {
	assert(node->addr < MEM_SIZE);
	lock_v(&node->node.lock);
	memory[node->addr] = node->node;
}

void mem_unlock(bptr_t address, Node *memory) {
	assert(address < MEM_SIZE);
	lock_v(&memory[address].lock);
}

void mem_reset_all(Node *memory) {
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
	memset(memory, INVALID, MEM_SIZE*sizeof(Node));
	#pragma GCC diagnostic pop
	for (bptr_t i = 0; i < MEM_SIZE; i++) {
		memory[i].lock = LOCK_INIT;
	}
}

bptr_t ptr_to_addr(void const *ptr, Node const *memory) {
	return (ptr - (void *) memory) / sizeof(Node);
}
