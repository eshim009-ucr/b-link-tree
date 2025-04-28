#ifndef IO_H
#define IO_H

#include "types.h"
#include <stdio.h>

typedef struct Node Node;

//! @brief Print the contents of a tree's memory in a human-readable format
//! @param[out] stream  Output stream to write to, can be a file or standard
//!                     output
void dump_node_list(FILE *stream, Node const *memory);

#endif
