#include "types.h"
#include <stdio.h>

#pragma once

//! @brief Print the contents of a tree's memory in a human-readable format
//! @param[out] stream  Output stream to write to, can be a file or standard
//!                     output
//! @param[in]  root    The root of the tree whose contents should be displayed
void dump_node_list(FILE *stream, bptr_t root);
