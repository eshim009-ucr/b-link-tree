#include <stdint.h>
#include <stdbool.h>
#include "tree.h"

#define DIV2CEIL(x) (((x) & 1) ? (((x)/2) + 1) : ((x)/2))

inline static ErrorCode search_leaf(LeafNode *leaf, bkey_t key, li_t *index) {
	for (li_t i = 0; i < MAX_KEYS; ++i) {
		if (leaf->keys[i] == key) {
			if (index != NULL) *index = i;
			return SUCCESS;
		}
	}
	return NOT_FOUND;
}

// Assumes first key is valid
static bptr_t search_inner(InnerNode *node, bkey_t key, ErrorCode *status) {
	for (li_t i = 0; i < MAX_KEYS; ++i) {
		// We overshot the maximum bound, so the last one is what we want
		if (node->keys[i] == INVALID || node->keys[i] >= key) {
			if (status != NULL) *status = SUCCESS;
			return node->children[i-1];
		}
	}
	if (status != NULL) *status = NOT_FOUND;
	return INVALID;
}

static inline void insert_at_idx(bkey_t *keys, bval_t *values, bkey_t new_key, bkey_t new_value, li_t i_insert, li_t i_empty) {
	// Scoot nodes if necessary to maintain ordering
	for (li_t i = i_empty; i_insert < i; i--) {
		keys[i] = keys[i-1];
		values[i] = values[i-1];
	}
	// Do the actual insertion
	keys[i_insert] = new_key;
	values[i_insert] = new_value;
}

bptr_t search(Tree *tree, bkey_t key, InnerNode *parent, ErrorCode *status) {
	Node *child;
	bptr_t child_idx;
	if (root_is_leaf(tree)) {
		return search_leaf(&get_root(tree)->leaf, key, NULL);
	}
	// Root is an inner node
	parent = &get_root(tree)->inner;
	for (li_t i = 0; i < MAX_LEVELS; ++i) {
		child_idx = search_inner(parent, key, status);
		child = &tree->memory[child_idx];
		if (status != NULL && *status != SUCCESS) {
			return INVALID;
		} else if (is_leaf(tree, child)) {
			if (status != NULL) {
				*status = search_leaf(&get_root(tree)->leaf, key, NULL);
			}
			return child_idx;
		} else {
			parent = &child->inner;
		}
	}
	return NOT_FOUND;
}

bptr_t insert_at_leaf_nonfull(Tree *tree, bptr_t leaf_addr, bkey_t key, bval_t value, ErrorCode *status) {
	li_t i_insert = 0;
	LeafNode *leaf = &tree->memory[leaf_addr].leaf;
	if (!is_leaf(tree, (Node*) leaf)) return INVALID_ARGUMENT;

	for (li_t i = 0; i < MAX_KEYS; ++i) {
		if (leaf->keys[i] == INVALID) {
			insert_at_idx(leaf->keys, leaf->data, key, value, i_insert, i);
			*status = SUCCESS;
			return i_insert;
		} else if (leaf->keys[i] == key) {
			*status = KEY_EXISTS;
			return i_insert;
		} else if (leaf->keys[i] < key) {
			i_insert++;
		}
	}
	*status = OUT_OF_MEMORY;
	return i_insert;
}

ErrorCode insert_and_split_leaf(Tree *tree, bptr_t old_leaf_idx, bkey_t key, bval_t value, bptr_t *new_leaf_idx, InnerNode *parent, li_t i_insert) {
	LeafNode *old_leaf_node, *new_leaf_node;
	bkey_t all_keys[MAX_KEYS+1];
	bval_t all_data[MAX_KEYS+1];

	// Find an empty spot for the new leaf
	for (*new_leaf_idx = 0; *new_leaf_idx < MAX_NODES_PER_LEVEL; ++(*new_leaf_idx)) {
		// Found an empty slot
		if (tree->leaves[*new_leaf_idx].keys[0] == INVALID) break;
	}
	// If we didn't break, we didn't find an empty slot
	if (*new_leaf_idx == MAX_NODES_PER_LEVEL) {
		return OUT_OF_MEMORY;
	}
	// Adjust next leaf pointers
	old_leaf_node = &tree->leaves[old_leaf_idx];
	new_leaf_node = &tree->leaves[*new_leaf_idx];
	new_leaf_node->next_leaf = old_leaf_node->next_leaf;
	old_leaf_node->next_leaf = *new_leaf_idx;

	// Copy old leaf data into temp buffers, insert there
	memcpy(all_keys, old_leaf_node->keys, sizeof(bkey_t)*MAX_KEYS);
	memcpy(all_data, old_leaf_node->data, sizeof(bval_t)*MAX_KEYS);
	insert_at_idx(all_keys, all_data, key, value, i_insert, MAX_KEYS);

	// Copy data out of buffers back to leaves
	li_t new_start = DIV2CEIL(MAX_KEYS+1);
	for (li_t i = new_start; i < MAX_KEYS+1; ++i) {
		new_leaf_node->keys[i-new_start] = all_keys[i];
		new_leaf_node->data[i-new_start] = all_data[i];
		if (i < MAX_KEYS) {
			old_leaf_node->keys[i] = INVALID;
			old_leaf_node->data[i] = INVALID;
		}
	}

	// If this is the root node
	// We need to create the first inner node
	if (parent == NULL) {
		// Make a new root node
		tree->root = MAX_LEAVES;
		InnerNode *root = &tree->memory[tree->root].inner;
		init_node((Node*) root);

		root->keys[0] = all_keys[(MAX_KEYS+1)/2];
		root->children[0] = old_leaf_idx;
		root->keys[1] = all_keys[MAX_KEYS];
		root->children[1] = *new_leaf_idx;
		parent = root;
		return SUCCESS;
	} else {
		return NOT_IMPLEMENTED;
	}
}

ErrorCode insert_at_leaf(Tree *tree, bptr_t leaf_addr, InnerNode *parent, bkey_t key, bval_t value) {
	ErrorCode status;
	bptr_t new_leaf;
	li_t i_insert;

	do {
		i_insert = insert_at_leaf_nonfull(tree, leaf_addr, key, value, &status);

		if (status != OUT_OF_MEMORY) {
			return status;
		}
		// Leaf is full, split and try again
		return insert_and_split_leaf(tree, leaf_addr, key, value, &new_leaf, parent, i_insert);
	} while(true);

	return NOT_IMPLEMENTED;
}

ErrorCode insert(Tree *tree, bkey_t key, bval_t value) {
	if (root_is_leaf(tree)) {
		return insert_at_leaf(tree, tree->root, NULL, key, value);
	} else {
		ErrorCode status;
		InnerNode parent;
		bptr_t leaf_idx = search(tree, key, &parent, &status);
		return insert_at_leaf(tree, leaf_idx, &parent, key, value);
	}
}
