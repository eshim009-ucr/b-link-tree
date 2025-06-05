#include "insert.h"
#include "util.h"
#include "node.h"
#include "memory.h"


inline static void move_right(bkey_t *t, bkey_t *v, Node *A, bkey_t *current, Node *memory) {
	/* Move right if necessary */
	/* while t <- scannode(v, A) is a link pointer of A do */
	while (scannode(*v, A, t)) {
		/* Note left-to-right locking */
		mem_lock(*t, memory);
		mem_unlock(*current, memory); 
		*current = *t;
		*A = mem_read(*current, memory);
	}
}


inline static bool is_safe(Node *const A) {
	// If the last key hasn't been set, then there is still room in this node
	return A->keys[TREE_ORDER-1] == INVALID;
}

static void node_insert(Node *A, bval_t w, bkey_t v) {
	li_t i_insert = 0;

	for (li_t i = 0; i < TREE_ORDER; ++i) {
		// Found an empty slot
		// Will be the last slot
		if (A->keys[i] == INVALID) {
			// Scoot nodes if necessary to maintain ordering
			// Iterate right to left from the last node to the insertion point
			for (; i_insert < i; i--) {
				A->keys[i] = A->keys[i-1];
				A->values[i] = A->values[i-1];
			}
			// Do the actual insertion
			A->keys[i_insert] = v;
			A->values[i_insert] = w;
			return;
		} else if (A->keys[i] == v) {
			assert(0);
		} else if (A->keys[i] < v) {
			i_insert++;
		}
	}
	assert(0);
}


//! @return which node got the new entry
static int divide(Node *A, bval_t w, bkey_t v, Node *B, bptr_t u) {
	// Adjust next node pointers
	B->next = A->next;
	A->next = u;
	// Move half of old node's contents to new node
	for (li_t i = 0; i < TREE_ORDER/2; ++i) {
		B->keys[i] = A->keys[i + (TREE_ORDER/2)];
		B->values[i] = A->values[i + (TREE_ORDER/2)];
		A->keys[i + (TREE_ORDER/2)] = INVALID;
	}
	if (A->keys[(TREE_ORDER/2)-1] < v) {
		node_insert(B, w, v);
		return 1;
	} else {
		node_insert(A, w, v);
		return 0;
	}
}

static void split_key(Node *node, bkey_t new_key, bptr_t old_value) {
	for (li_t i = 0; i < TREE_ORDER; ++i) {
		if (node->values[i].ptr == old_value) {
			node->keys[i] = new_key;
			return;
		}
	}
	assert(0);
}


#ifdef OPTIMISTIC_LOCK
static ErrorCode optimistic_insert(bptr_t *root, bkey_t v, bval_t w, Node *memory) {
#else
ErrorCode insert(bptr_t *root, bkey_t v, bval_t w, Node *memory) {
#endif
	/* For remembering ancestors */
	bptr_t stack[MAX_LEVELS];
	bptr_t stack_ptr = 0;
	bptr_t t;
	bptr_t current = *root;
	Node A = mem_read(current, memory);

	/* Scan down tree */
	while (!is_leaf(current)) {
		t = current;
		if (!scannode(v, &A, &current)) {
			assert(stack_ptr < MAX_LEVELS);
			/* Remember node at that level */
			stack[stack_ptr++] = t;
		}
		A = mem_read(current, memory);
	}
	bool leaf = true;
	
	/* We have a candidate leaf */
	mem_lock(current, memory);
	A = mem_read(current, memory);
	move_right(&t, &v, &A, &current, memory);
	/* if v is in A then stop "v already exists in tree"; */
	/* And t points to its record */
	bstatusval_t result = get_value(v, &A);
	if (result.status != NOT_FOUND) {
		return KEY_EXISTS;
	}
	
	Doinsertion:
	if (is_safe(&A)) {
		/* Exact manner depends if current is a leaf */
		node_insert(&A, w, v);
		mem_write(current, &A, memory);
		/* Success-done backtracking */
		mem_unlock(current, memory);
		return SUCCESS;
	} else {
		Node B = empty_node();
		bptr_t u = leaf ?
			alloc_node(&B, 0, MAX_LEAVES, memory) :
			alloc_node(&B, MAX_LEAVES, MEM_SIZE, memory);
		/* A, B <- rearrange old A, adding v and w, to make 2 nodes,
		 * where (link ptr of A, linkptr of B) <- (u, linkptr of old A); */
		/* y <- max value stored in new A; *//* For insertion into parent */
		bkey_t y, B_max;
		if (divide(&A, w, v, &B, u)) {
			y = A.keys[(TREE_ORDER/2)-1];
			B_max = B.keys[TREE_ORDER/2];
		} else {
			y = A.keys[TREE_ORDER/2];
			B_max = B.keys[(TREE_ORDER/2)-1];
		}
		/* Insert B before A */
		mem_write(u, &B, memory);
		/* Instantaneous change of 2 nodes */
		mem_write(current, &A, memory);
		/* Now insert pointer in parent */
		bptr_t oldnode = current;
		v = B_max;
		w.ptr = u;
		// Out of parents, need to create a new one
		if (stack_ptr == 0) {
			// Initialize blank parent
			Node parent = empty_node();
			// Insert a pointer to A
			node_insert(&parent, (bval_t) current, y);
			// Adjust A for next iteration
			A = parent;
			// Reserve a memory slot for the new parent
			current = alloc_node(&parent, MAX_LEAVES, MEM_SIZE, memory);
			mem_write(current, &A, memory);
			*root = current;
			// Pointer to B will be inserted on next iteration
		} else {
			bptr_t A_addr = current;
			/* Backtrack */
			current = stack[--stack_ptr];
			/* Well ordered */
			mem_lock(current, memory);
			A = mem_read(current, memory);
			move_right(&t, &v, &A, &current, memory);
			split_key(&A, y, A_addr);
		}
		mem_unlock(oldnode, memory);
		leaf = false;
		/* And repeat procedure for parent */
		goto Doinsertion;
	}
	return -1;
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
