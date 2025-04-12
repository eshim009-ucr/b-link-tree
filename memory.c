#ifndef __SYNTHESIS__
#include "core/lock.h"
#include "core/memory.h"
#include "core/node.h"
#include <string.h>
#include <assert.h>


static Node memory[MEM_SIZE];


Node mem_read(bptr_t address) {
	assert(address < MEM_SIZE);
	return memory[address];
}

Node mem_read_lock(bptr_t address) {
	assert(address < MEM_SIZE);
	lock_p(&memory[address].lock);
	return mem_read(address);
}

void mem_write_unlock(AddrNode *node) {
	assert(node->addr < MEM_SIZE);
	lock_v(&node->node.lock);
	memory[node->addr] = node->node;
}

void mem_unlock(bptr_t address) {
	assert(address < MEM_SIZE);
	lock_v(&memory[address].lock);
}

void mem_reset_all() {
	memset(memory, INVALID, MEM_SIZE*sizeof(Node));
	for (bptr_t i = 0; i < MEM_SIZE; i++) {
		init_lock(&memory[i].lock);
	}
}

bptr_t ptr_to_addr(void *ptr) {
	return (ptr-(void*)memory)/sizeof(Node);
}


#endif
