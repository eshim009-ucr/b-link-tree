#ifndef MEMORY_H
#define MEMORY_H


#include "types.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct Node Node;
typedef struct AddrNode AddrNode;


//! @brief Read a node from memory without touching any locks
Node mem_read(bptr_t address, Node const *memory);

//! @brief Write a node to memory without touching any locks
void mem_write(bptr_t address, Node const *node, Node *memory);

void mem_lock(bptr_t address, Node *memory);

bool mem_trylock(bptr_t address, Node *memory);

#ifdef OPTIMISTIC_LOCK
//! @brief Unlock a node in memory
//! @return true on success, false on failure
bool mem_unlock(bptr_t address, Node *memory);
#else
//! @brief Unlock a node in memory
void mem_unlock(bptr_t address, Node *memory);
#endif

bptr_t alloc_node(bptr_t level, Node *memory);

//! @brief Reset memory to a slate of blank nodes
//!
//! All data is 1s except for locks
void mem_reset_all(Node *memory);


#endif
