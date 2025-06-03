#ifndef UTIL_H
#define UTIL_H


#include "types.h"


typedef struct Node Node;


bstatusval_t get_value(bkey_t v, Node const *A);
bptr_t scannode(bkey_t v, Node const *A);
Node empty_node();


#endif
