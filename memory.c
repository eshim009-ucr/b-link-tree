#include "memory.h"
#include "lock.h"
#include "node.h"
#include <assert.h>
#include <string.h>


Node mem_read(bptr_t address, Node const *memory) {
	assert(address < MEM_SIZE);
	return memory[address];
}

Node mem_read_lock(bptr_t address, Node *memory) {
	assert(address < MEM_SIZE);
	lock_p(&memory[address].lock);
	return mem_read(address, memory);
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
	memset(memory, INVALID, MEM_SIZE*sizeof(Node));
	for (bptr_t i = 0; i < MEM_SIZE; i++) {
		init_lock(&memory[i].lock);
	}
}

bptr_t ptr_to_addr(void *ptr, Node const *memory) {
	return (ptr - (void *) memory) / sizeof(Node);
}
