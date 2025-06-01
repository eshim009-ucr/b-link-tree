#include "insert.h"
#include "insert-helpers.h"
#include "memory.h"
#include "node.h"
#include "split.h"
#include "tree-helpers.h"
#include <string.h>


static bstatusval_t scan_node(Node const *A, bkey_t v) {
	return find_next(A, v);
}

static void move_right(AddrNode *A_current, bkey_t v, bptr_t *t, Node *memory) {
	bstatusval_t sv = scan_node(&A_current->node, v);
	if (sv.status != SUCCESS) return;
	while ((*t = sv.value.ptr) == A_current->node.next) {
		(void) mem_read_lock(*t, memory);
		mem_unlock(A_current->addr, memory);
		A_current->addr = *t;
		A_current->node = mem_read(A_current->addr, memory);
	}
}


#ifdef OPTIMISTIC_LOCK
static ErrorCode optimistic_insert(bptr_t *root, bkey_t key, bval_t value, Node *memory) {
#else
ErrorCode insert(bptr_t *root, bkey_t key, bval_t value, Node *memory) {
#endif
	uint_fast8_t stack_top = 0;
	bstatusval_t sv;
	bptr_t stack[MAX_LEVELS];
	bkey_t v = key;
	bptr_t t, y, w, old_node;
	AddrNode Bu;
	AddrNode A_current = {.node=mem_read(*root, memory), .addr=*root};

	while (!is_leaf(A_current.addr)) {
		t = A_current.addr;
		sv = scan_node(&A_current.node, v);
		if (sv.status == NOT_FOUND && *root == 0) {
			break;
		} else {
			A_current.addr = sv.value.ptr;
		}
		if (t != A_current.node.next) {
			stack[stack_top++] = t;
		}
		A_current.node = mem_read(A_current.addr, memory);
	}

	A_current.node = mem_read_lock(A_current.addr, memory);
	move_right(&A_current, v, &t, memory);
	if (find_value(&A_current.node, v).status != NOT_FOUND) {
		return KEY_EXISTS;
	}

	w = A_current.addr;
	do_insertion:
	if (!is_full(&A_current.node)) {
		if (insert_nonfull(&A_current.node, v, value) != SUCCESS) assert(0);
		mem_write_unlock(&A_current, memory);
	} else {
		alloc_sibling(root, &A_current, &Bu, memory);
		y = max(&A_current.node);
		mem_write(&Bu, memory);
		mem_write(&A_current, memory);
		old_node = A_current.addr;
		v = y;
		w = Bu.addr;
		A_current.addr = stack[stack_top--];
		A_current.node = mem_read_lock(A_current.addr, memory);
		move_right(&A_current, v, &t, memory);
		mem_unlock(old_node, memory);
		goto do_insertion;
	}

	return SUCCESS;
}

#ifdef OPTIMISTIC_LOCK
ErrorCode insert(bptr_t *root, bkey_t key, bval_t value, Node *memory) {
	ErrorCode status;
	do {
		status = optimistic_insert(root, key, value, memory);
	} while (status == RESTART);
	return status;
}
#endif
