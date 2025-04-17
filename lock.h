#ifndef LOCK_H
#define LOCK_H

#include "defs.h"
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>

#ifdef HLS
typedef bool lock_t;
#define LOCK_INIT 0
#else
typedef pthread_mutex_t lock_t;
#define LOCK_INIT ((pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER)
#endif


//! @brief Perform an atomic test-and-set operation on the given lock
static inline bool test_and_set(lock_t *lock) {
#ifdef HLS
	bool old = *lock;
	*lock = true;
	return old;
#else
	return pthread_mutex_trylock(lock);
#endif
}

static inline bool lock_test(lock_t const *lock) {
	//! @todo Do this better.
	//! x86 defines atomic_flag as 0 for unset,
	//! but this is not a guarantee for all architectures
	return *((bool *) lock);
}


//! @brief Set the given lock to held
static inline void lock_p(lock_t *lock) {
	#ifdef HLS
	while (test_and_set(lock));
	#else
	pthread_mutex_lock(lock);
	#endif
}

//! @brief Release the given lock
static inline void lock_v(lock_t *lock) {
	assert(lock_test(lock));
	#ifdef HLS
	*lock = 0;
	#else
	pthread_mutex_unlock(lock);
	#endif
}

#endif
