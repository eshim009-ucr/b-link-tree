#ifndef TESTS_HELPERS_HPP
#define TESTS_HELPERS_HPP


#include "../types.h"


typedef struct Node Node;

//! @brief Argument to @ref stride_insert thread handler
struct si_args {
	//! First value to insert
	int_fast32_t start;
	//! Last value to insert,
	//! exact value may not be inserted if stride is misaligned
	int_fast32_t end;
	//! Difference between the current inserted value and the next inserted
	//! value
	int_fast32_t stride;
	//! Pointer to the tree root
	bptr_t *root;
};


extern Node memory[MEM_SIZE];

//! @brief Thread function for inserting values over a range with a set interval
//! between them
//!
//! Can handle negative start, end, and stride values.
//! Invalid combinations of start, end, and stride will result in a test failure
//!
//! Keys are have values of 1 <= key <= n, Values are assigned to -1 times their
//! key
void *stride_insert(void *argv);
//! @brief Check after insertion of keys with GoogleTest assertions
//!
//! Keys are have values of 1 <= key <= n,
//! Values are assigned to -1 times their key
void check_inserted_leaves();


#endif
