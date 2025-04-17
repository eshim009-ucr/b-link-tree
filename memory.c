#include "memory.h"
#include "lock.h"
#include "node.h"
#include <assert.h>
#include <string.h>


Node mem_read(bptr_t address, volatile Node const *memory) {
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
Node mem_read_lock(bptr_t address, volatile Node *memory) {
	static lock_t local_readlock = 0;
	Node tmp;

	assert(address < MEM_SIZE);
	lock_p(&local_readlock);
	// Read the given address from main memory until its lock is released
	do {
		tmp = memory[address];
	} while(lock_test(&tmp.lock));
	// After the lock is released, we can safely acquire it because no other
	// modules concurrently hold this function's lock
	lock_p(&tmp.lock);
	// Write back the locked value to main memory
	memory[address] = tmp;
	// Release the local lock for future writers
	lock_v(&local_readlock);
	return tmp;
}

void mem_write_unlock(AddrNode *node, volatile Node *memory) {
	assert(node->addr < MEM_SIZE);
	lock_v(&node->node.lock);
	memory[node->addr] = node->node;
}

void mem_unlock(bptr_t address, volatile Node *memory) {
	assert(address < MEM_SIZE);
	lock_v(&memory[address].lock);
}

void mem_reset_all(volatile Node *memory) {
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
	memset(memory, INVALID, MEM_SIZE*sizeof(Node));
	#pragma GCC diagnostic pop
	for (bptr_t i = 0; i < MEM_SIZE; i++) {
		init_lock(&memory[i].lock);
	}
}

bptr_t ptr_to_addr(void *ptr, volatile Node const *memory) {
	return (ptr - (void *) memory) / sizeof(Node);
}
