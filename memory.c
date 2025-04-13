#include "core/memory.h"
#include "core/lock.h"
#include "core/node.h"
#include <assert.h>
#include <string.h>


static Node memory[MEM_SIZE];


Node mem_read(bptr_t address, mread_req_stream_t *req_stream, mread_resp_stream_t *resp_stream) {
	assert(address < MEM_SIZE);
	return memory[address];
}

Node mem_read_lock(bptr_t address, mread_req_stream_t *req_stream, mread_resp_stream_t *resp_stream) {
	assert(address < MEM_SIZE);
	lock_p(&memory[address].lock);
	return mem_read(address, req_stream, resp_stream);
}

void mem_write_unlock(AddrNode *node, mwrite_stream_t *req_stream) {
	assert(node->addr < MEM_SIZE);
	lock_v(&node->node.lock);
	memory[node->addr] = node->node;
}

void mem_unlock(bptr_t address, mwrite_stream_t *req_stream) {
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
	return (ptr - (void *) memory) / sizeof(Node);
}
