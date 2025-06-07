#ifndef PRIMITIVES_H
#define PRIMITIVES_H


#include "defs.h"
#include <stdint.h>


//! Datatype of keys
typedef uint32_t bkey_t;
//! Datatype of pointers within the tree
#if MEM_SIZE < (1ULL << 8)
typedef uint_fast8_t bptr_t;
#define INVALID ((bkey_t) 0xFF)
#elif MEM_SIZE < (1ULL << 16)
typedef uint_fast16_t bptr_t;
#define INVALID ((bkey_t) 0xFFFF)
#elif MEM_SIZE < (1ULL << 32)
typedef uint_fast32_t bptr_t;
#define INVALID ((bkey_t) 0xFFFFFFFF)
#elif MEM_SIZE <= (0xFFFFFFFFFFFFFFFFULL)
typedef uint_fast64_t bptr_t;
#define INVALID ((bkey_t) 0xFFFFFFFFFFFFFFFF)
#else
#error Address space too big!
#endif
//! Datatype of leaf data
typedef int32_t bdata_t;
//! @brief Datatype of node values, which can be either data or pointers within
//!        the tree
typedef union {
	bptr_t ptr;   //!< Internal node value which points to another node
	bdata_t data; //!< Leaf node value which holds data
} bval_t;
// Leaf index type
#if TREE_ORDER < (1 << 8)
typedef uint_fast8_t li_t;
#elif TREE_ORDER < (1 << 16)
typedef uint_fast16_t li_t;
#elif TREE_ORDER < (1 << 32)
typedef uint_fast32_t li_t;
#endif
//! Explanation: https://en.wikipedia.org/wiki/X_macro
#define ERROR_CODE_XMACRO \
	X(SUCCESS, 0) \
	X(KEY_EXISTS, 1) \
	X(NOT_IMPLEMENTED, 2) \
	X(NOT_FOUND, 3) \
	X(INVALID_ARGUMENT, 4) \
	X(OUT_OF_MEMORY, 5) \
	X(PARENT_FULL, 6) \
	X(RESTART, 7)
#define MAX_ERR 7
//! @brief Status codes returned from tree functions
enum {
#define X(codename, codeval) codename = codeval,
ERROR_CODE_XMACRO
#undef X
};
//! HLS has a bad habit of mangling enums,
//! so this is needed to prevent getting junk values
typedef uint_least8_t ErrorCode;
//! @brief Names of status codes, used for error messages
static const char *const ERROR_CODE_NAMES[] = {
#define X(codename, codeval) #codename,
ERROR_CODE_XMACRO
#undef X
};
//! @brief Result of an operation returning a value and a return code
typedef struct {
	ErrorCode status;
	bval_t value;
} bstatusval_t;


//! @brief Key/value pair
typedef struct {
	bkey_t key;
	bval_t value;
} KvPair;

typedef bkey_t search_in_t;
typedef bstatusval_t search_out_t;
typedef KvPair insert_in_t;
typedef ErrorCode insert_out_t;


#endif
