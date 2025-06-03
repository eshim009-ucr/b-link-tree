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

static bkey_t divide(Node *A, bval_t w, bkey_t v, Node *B, bptr_t u) {
	// Adjust next node pointers
	B->next = A->next;
	A->next = u;
	// Move half of old node's contents to new node
	for (li_t i = 0; i < TREE_ORDER/2; ++i) {
		B->keys[i] = A->keys[i + (TREE_ORDER/2)];
		B->values[i] = A->values[i + (TREE_ORDER/2)];
		A->keys[i + (TREE_ORDER/2)] = INVALID;
	}
	if (A->keys[TREE_ORDER/2] < v) {
		node_insert(B, w, v);
		return A->keys[TREE_ORDER/2];
	} else {
		node_insert(A, w, v);
		return A->keys[(TREE_ORDER/2)+1];
	}
}


ErrorCode insert(bptr_t *root, bkey_t v, bval_t w, Node *memory) {
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
		bptr_t u = alloc_node();
		/* A, B <- rearrange old A, adding v and w, to make 2 nodes,
		 * where (link ptr of A, linkptr of B) <- (u, linkptr of old A); */
		/* y <- max value stored in new A; *//* For insertion into parent */
		bptr_t y = divide(&A, w, v, &B, u);
		/* Insert B before A */
		mem_write(u, &B, memory);
		/* Instantaneous change of 2 nodes */
		mem_write(current, &A, memory);
		/* Now insert pointer in parent */
		bptr_t oldnode = current;
		v = y;
		w.ptr = u;
		assert(stack_ptr > 0);
		/* Backtrack */
		current = stack[--stack_ptr];
		/* Well ordered */
		mem_lock(current, memory);
		A = mem_read(current, memory);
		move_right(&t, &v, &A, &current, memory);
		mem_unlock(oldnode, memory);
		/* And repeat procedure for parent */
		goto Doinsertion;
	}
	return -1;
}
