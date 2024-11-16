#include <stdint.h>
#include <stdbool.h>
#include "tree.h"

#define DIV2CEIL(x) (((x) & 1) ? (((x)/2) + 1) : ((x)/2))

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

ErrorCode search(Tree *tree, bkey_t key, bptr_t *lineage) {
	ErrorCode status;
	lineage[0] = tree->root;
	// If root is a leaf node
	if (root_is_leaf(tree)) {
		return SUCCESS;
	}
	// Root is an inner node
	for (li_t i = 1; i < MAX_LEVELS; ++i) {
		lineage[i] = search_inner(&tree->memory[lineage[i-1]].inner, key, &status);
		if (status != SUCCESS) {
			return status;
		} else if (bptr_is_leaf(tree, lineage[i])) {
			return SUCCESS;
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
			// Scoot nodes if necessary to maintain ordering
			for (; i_insert < i; i--) {
				leaf->keys[i] = leaf->keys[i-1];
				leaf->data[i] = leaf->data[i-1];
			}
			// Do the actual insertion
			leaf->keys[i_insert] = key;
			leaf->data[i_insert] = value;
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

ErrorCode split_leaf(Tree *tree, bptr_t old_leaf_idx, bptr_t *new_leaf_idx, InnerNode *parent) {
	LeafNode *old_leaf_node, *new_leaf_node;

	// Find an empty spot for the new leaf
	for (*new_leaf_idx = 0; *new_leaf_idx < MAX_NODES_PER_LEVEL; ++(*new_leaf_idx)) {
		// Found an empty slot
		if (tree->leaves[*new_leaf_idx].keys[0] == INVALID) {
			break;
		}
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
	// Move half of old leaf's contents to new leaf
	for (li_t i = 0; i < MAX_KEYS/2; ++i) {
		new_leaf_node->keys[i] = old_leaf_node->keys[i + (MAX_KEYS/2)];
		new_leaf_node->data[i] = old_leaf_node->data[i + (MAX_KEYS/2)];
		old_leaf_node->keys[i - (MAX_KEYS/2)] = INVALID;
		old_leaf_node->data[i - (MAX_KEYS/2)] = INVALID;
	}
	// If this is the root node
	// We need to create the first inner node
	if (parent == NULL) {
		// Make a new root node
		tree->root = MAX_LEAVES;
		InnerNode *root = &tree->memory[tree->root].inner;
		init_node((Node*) root);

		root->keys[0] = old_leaf_node->keys[DIV2CEIL(MAX_KEYS)-1];
		root->children[0] = old_leaf_idx;
		root->keys[1] = new_leaf_node->keys[(MAX_KEYS/2)-1];
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
		status = split_leaf(tree, leaf_addr, &new_leaf, parent);
		if (status == OUT_OF_MEMORY) {
			// No leaf slots left
			return status;
		}
		// Try inserting again with one of the new leaves
		if (i_insert > (MAX_CHILDREN+1)/2) {
			leaf_addr = new_leaf;
		} // Else, leaf_addr stays the same
	} while(true);

	return NOT_IMPLEMENTED;
}

ErrorCode insert(Tree *tree, bkey_t key, bval_t value) {
	if (root_is_leaf(tree)) {
		return insert_at_leaf(tree, tree->root, NULL, key, value);
	} else {
		bptr_t lineage[MAX_LEVELS];
		memset(lineage, INVALID, sizeof(lineage));
		search(tree, key, lineage);
		// Adjust parents if new max
		for (int i = 0; i < MAX_LEVELS; ++i) {
			if (lineage[i] == INVALID) {
				printf("---, ");
			} else {
				printf("%u, ", lineage[i]);
			}
		}
		printf("\n");
		// Adjust parent high keys if new max
		li_t i_max;
		for (int i = 0; i < MAX_LEVELS; ++i) {
			if (bptr_is_leaf(tree, lineage[i])) {
				break;
			}
			i_max = max(&tree->memory[lineage[i]]);
			if (key <= i_max) {
				break;
			} else {
				tree->memory[lineage[i]].inner.keys[i_max] = key;
			}
		}

		return NOT_IMPLEMENTED;
	}
}
