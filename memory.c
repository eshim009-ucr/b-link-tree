#include "memory.h"
#include "lock.h"
#include "node.h"
#include <assert.h>
#include <string.h>


#define LCKPTR_AT_ADDR(node_addr) \
	/* Cast byte pointer to lock_t pointer */ \
	/* to ensure write is of correct size */ \
	((lock_t*) ( \
		&( /* Byte pointer */ \
			(uint8_t*) memory \
		)[ \
			/* Address of the lock field of the node who starts at address */ \
			(address+1)*sizeof(Node)-sizeof(lock_t) \
		] \
	))


Node mem_read(bptr_t address, Node const *memory) {
	assert(address < MEM_SIZE);
	return memory[address];
}

void mem_write(bptr_t address, Node *node, Node *memory) {
	assert(address < MEM_SIZE);
	memory[address] = *node;
}


void mem_lock(bptr_t address, Node *memory) {
#ifndef OPTIMISTIC_LOCK
	assert(address < MEM_SIZE);
	lock_p(LCKPTR_AT_ADDR(address));
#endif
}

bool mem_trylock(bptr_t address, Node *memory) {
#ifndef OPTIMISTIC_LOCK
	return test_and_set(LCKPTR_AT_ADDR(address)) == 0;
#endif
}

void mem_unlock(bptr_t address, Node *memory) {
#ifndef OPTIMISTIC_LOCK
	assert(address < MEM_SIZE);
	lock_v(LCKPTR_AT_ADDR(address));
#endif
}

bptr_t alloc_node(Node *node, bptr_t start, bptr_t end, Node *memory) {
	for (bptr_t i = start; i < end; ++i) {
		if (mem_trylock(i, memory)) {
			return i;
		}
	}
	assert(0);
	return INVALID;
}


void mem_reset_all(Node *memory) {
	memset(memory, INVALID, MEM_SIZE*sizeof(Node));
	for (bptr_t i = 0; i < MEM_SIZE; i++) {
		memory[i].lock = LOCK_INIT;
	}
}
