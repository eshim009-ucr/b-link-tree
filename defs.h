#ifndef DEFS_H
#define DEFS_H

#ifdef STACK_ALLOC
// Allegedly the pow calls get optimized out at high enough optimization levels
#include <math.h>
#endif

//! Number of children in each node of a tree
#define TREE_ORDER (4)
//! Maximum height of a tree
#define MAX_LEVELS (4)
//! Maximum number of leaf nodes in the entire tree
#ifdef STACK_ALLOC
#define MAX_LEAVES ((uint64_t) pow(TREE_ORDER, MAX_LEVELS-1))
#else
#define MAX_LEAVES 8
#endif
//! "Width" of a memory grid
//! Each level of the tree is on its own level
#define MAX_NODES_PER_LEVEL MAX_LEAVES
//! Maximum number of nodes that can be stored in a tree's memory
#ifdef STACK_ALLOC
#define MEM_SIZE ((uint64_t) ((pow(TREE_ORDER, MAX_LEVELS) - 1) / (TREE_ORDER - 1)))
#else
#define MEM_SIZE (MAX_NODES_PER_LEVEL * MAX_LEVELS)
#endif

#endif
