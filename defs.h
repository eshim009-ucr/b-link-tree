#ifndef DEFS_H
#define DEFS_H

//! Number of children in each node of a tree
#ifndef TREE_ORDER
#define TREE_ORDER 8ULL
#endif
//! "Width" of a memory grid
//! Each level of the tree is on its own level
#ifndef MAX_NODES_PER_LEVEL
#define MAX_NODES_PER_LEVEL ((bptr_t) 4000004)
#endif
//! Maximum height of a tree
#ifndef MAX_LEVELS
#define MAX_LEVELS ((bptr_t) 10)
#endif
//! Maximum number of leaf nodes in the entire tree
#ifndef MAX_LEAVES
#define MAX_LEAVES ((bptr_t) 4000004)
#endif
//! Maximum number of nodes that can be stored in a tree's memory
#ifndef MEM_SIZE
#define MEM_SIZE 10396753ULL
#endif
//! Maximum number of entries that can be safely inserted
#ifndef ENTRY_MAX
#define ENTRY_MAX 2446676ULL
#endif

#ifndef LEVEL_STARTS
#define LEVEL_STARTS {0, 4000004, 8000008, 10097160, 10359304, 10392072, 10396168, 10396680, 10396744, 10396752, 10396753}
#endif

#ifndef LEVEL_WIDTHS
#define LEVEL_WIDTHS {4000004, 4000004, 2097152, 262144, 32768, 4096, 512, 64, 8, 1}
#endif

#if !defined NO_GTEST && !__has_include("gtest/gtest.h")
#define NO_GTEST
#endif

#endif
