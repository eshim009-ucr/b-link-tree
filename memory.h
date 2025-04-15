#ifndef MEMORY_H
#define MEMORY_H


#include "types.h"
#include <stddef.h>

typedef struct Node Node;
typedef struct AddrNode AddrNode;


//! @brief Read a node from memory without grabbing any locks
Node mem_read(bptr_t address, Node const *memory);

//! @brief Read a node from memory, locking it in memory
Node mem_read_lock(bptr_t address, Node *memory);

//! @brief Write a node to memory and unlock it
void mem_write_unlock(AddrNode *node, Node *memory);

//! @brief Unlock a node in memory
//!
//! Used when a node has been read and locked, but should be unlocked without
//! modification. For example, if an error has been detected before a
//! modification operation completes.
void mem_unlock(bptr_t address, Node *memory);

//! @brief Reset memory to a slate of blank nodes
//!
//! All data is 1s except for locks
void mem_reset_all(Node *memory);

bptr_t ptr_to_addr(void *ptr, Node const *memory);


#endif
