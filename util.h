#ifndef UTIL_H
#define UTIL_H


#include "types.h"
#include <stdbool.h>


typedef struct Node Node;


bstatusval_t get_value(bkey_t v, Node const *A);
bool scannode(bkey_t v, Node const *A, bptr_t *result);
Node empty_node();


#endif
