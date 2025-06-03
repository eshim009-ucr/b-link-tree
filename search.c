#include "search.h"
#include "memory.h"
#include "util.h"
#include "node.h"



bstatusval_t search(bptr_t root, bkey_t v, Node const *memory) {
	bptr_t current = root;
	Node A = mem_read(current, memory);

	/* Scan through tree */
	while (!is_leaf(current)) {
		/* Find correct (maybe link) ptr */
		current = scannode(v, &A);
		/* Read node into memory */
		A = mem_read(current, memory);
	} /* Now we have reached leaves. */

	/* while t <- scannode(v, A) = link ptr of a A do */
	/* Keep moving right if necessary */
	bptr_t t;
	while ((t = scannode(v, &A)) == A.next) {
		current = t;
		A = mem_read(current, memory);
	} /* Now we have the leaf node in which u should exist. */

	/* if v is in A then done "success" else done "failure"; */
	return get_value(v, &A);
}
