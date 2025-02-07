#include "delete.h"
#include "tree-helpers.h"
#include <string.h>


static bstatusval_t delete_leaf(Node *node, bkey_t key) {
	bstatusval_t ret = {
		.status=NOT_FOUND,
		.value=INVALID,
	};
	lock_p(&node->lock);
	for (li_t i = 0; i < TREE_ORDER; ++i) {
		if (node->keys[i] == key) {
			node->keys[i] == INVALID;
			node->values[i].data == INVALID;
			ret.status = SUCCESS;
			ret.value.data = i;
			break;
		}
	}
	lock_v(&node->lock);
	return ret;
}


ErrorCode delete(Tree *tree, bkey_t key) {
	bstatusval_t ret;
	li_t i_leaf;
	bptr_t lineage[MAX_LEVELS];

	// Initialize lineage array
	memset(lineage, INVALID, MAX_LEVELS*sizeof(bptr_t));
	// Try to trace lineage
	ret.status = trace_lineage(tree, key, lineage);
	// If that failed, return the relevant error code
	if (ret.status != SUCCESS) return ret.status;

	i_leaf = get_leaf_idx(lineage);

	// @todo
	// Remove key from leaf
	// Remove from parents if leaf ends up empty
	// Continue until root
	// do {
		ret = delete_leaf(&A2S(lineage[i_leaf]), key);
	// 	if (ret.status != SUCCESS) return ret.status;
	// } while (ret.value.data == 0 && --i_leaf > 0);
	return ret.status;
}
