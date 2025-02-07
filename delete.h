#ifndef DELETE_H
#define DELETE_H

#include "tree.h"

//! @brief Insert a new value into the tree with the given key and value
//! @param[in] tree   The tree to insert the value into
//! @param[in] key    The key to remove
//! @return An error code representing the success or type of failure of the
//!         operation
ErrorCode delete(Tree *tree, bkey_t key);

#endif
