#include "util.h"
#include "node.h"


bstatusval_t get_value(bkey_t v, Node const *A) {
	bstatusval_t result = {.status=SUCCESS, .value.ptr=INVALID};
	for (li_t i = 0; i < TREE_ORDER; ++i) {
		if (A->keys[i] == v) {
			result.value = A->values[i];
			return result;
		}
	}
	result.status = NOT_FOUND;
	return result;
}


bool scannode(bkey_t v, Node const *A, bptr_t *result) {
	li_t i;
	for (i = 0; i < TREE_ORDER; ++i) {
		if (A->keys[i] >= v) {
			*result = A->values[i].ptr;
			return false;
		} else if (A->keys[i] == INVALID) {
			break;
		}
	}
	assert(i>0);
	/* If the maximum key given is less than v,
	 * then some change has taken place in the current node that had not been
	 * indicated in the parent at the time the parent was examined.
	 * The current node must have been split into two (or more) new nodes.
	 * The search must then rectify the error in its position in the tree by
	 * following the link pointer of the newly split node instead of by
	 * following a child pointer as it would ordinarily do. */
	if (A->keys[i-1] < v) {
		*result = A->next;
		return A->next != INVALID;
	}
	assert(0);
	return false;
}

Node empty_node() {
	Node empty;
	for (li_t i = 0; i < TREE_ORDER; ++i) {
		empty.keys[i] = INVALID;
	}
	return empty;
}
