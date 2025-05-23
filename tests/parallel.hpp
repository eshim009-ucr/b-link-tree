#ifndef TESTS_PARALLEL_HPP
#define TESTS_PARALLEL_HPP


extern "C" {
#include "../io.h"
#include "../memory.h"
#include "../node.h"
#include "../validate.h"
};
#include "helpers.hpp"
#include <gtest/gtest.h>

// Multithreading bugs are often dependent on random ways that threads can
// interleave. Testing multiple times gives more chances for these bugs to
// manifest themselves.
#ifndef PARALLEL_RERUNS
#define PARALLEL_RERUNS 1024
#endif

extern FILE *log_stream;
extern Node memory[MEM_SIZE];


TEST(ParallelTest, InterleavedAscending) {
	const testing::TestInfo* const test_info =
		testing::UnitTest::GetInstance()->current_test_info();
	fprintf(log_stream, "=== %s.%s ===\n",
		test_info->test_suite_name(), test_info->name()
	);

	pthread_t thread_even, thread_odd;
	bptr_t root = 0;
	si_args odd_args = {
		.start = 1,
		.end = (TREE_ORDER/2)*(MAX_LEAVES+1),
		.stride = 2,
		.root = &root
	};
	si_args even_args = odd_args;
	even_args.start = 2;

	for (uint_fast8_t i = 0; i < PARALLEL_RERUNS; ++i) {
		fprintf(log_stream, "Run %d\n", i+1);
		root = 0;
		mem_reset_all(memory);

		pthread_create(&thread_even, NULL, stride_insert, (void*) &even_args);
		pthread_create(&thread_odd, NULL, stride_insert, (void*) &odd_args);
		pthread_join(thread_even, NULL);
		pthread_join(thread_odd, NULL);
		ASSERT_TRUE(even_args.pass);
		ASSERT_TRUE(odd_args.pass);

		dump_node_list(log_stream, memory);

		ASSERT_TRUE(check_inserted_leaves());

		ASSERT_TRUE(validate(root, log_stream, memory));
	}
	fprintf(log_stream, "\n\n");
}

TEST(ParallelTest, InterleavedDescending) {
	const testing::TestInfo* const test_info =
		testing::UnitTest::GetInstance()->current_test_info();
	fprintf(log_stream, "=== %s.%s ===\n",
		test_info->test_suite_name(), test_info->name()
	);

	pthread_t thread_even, thread_odd;
	bptr_t root = 0;
	si_args odd_args = {
		.end = 1,
		.stride = -2,
		.root = &root
	};
	si_args even_args = odd_args;
	if ((TREE_ORDER/2)*(MAX_LEAVES+1) % 2 == 0) {
		even_args.start = (TREE_ORDER/2)*(MAX_LEAVES+1);
		odd_args.start = even_args.start - 1;
	} else {
		odd_args.start = (TREE_ORDER/2)*(MAX_LEAVES+1);
		even_args.start = even_args.start - 1;
	}

	for (uint_fast8_t i = 0; i < PARALLEL_RERUNS; ++i) {
		fprintf(log_stream, "Run %d\n", i+1);
		root = 0;
		mem_reset_all(memory);

		pthread_create(&thread_even, NULL, stride_insert, (void*) &even_args);
		pthread_create(&thread_odd, NULL, stride_insert, (void*) &odd_args);
		pthread_join(thread_even, NULL);
		pthread_join(thread_odd, NULL);
		ASSERT_TRUE(even_args.pass);
		ASSERT_TRUE(odd_args.pass);

		dump_node_list(log_stream, memory);

		ASSERT_TRUE(check_inserted_leaves());

		ASSERT_TRUE(validate(root, log_stream, memory));
	}
	fprintf(log_stream, "\n\n");
}

TEST(ParallelTest, CrossfadeInsert) {
	const testing::TestInfo* const test_info =
		testing::UnitTest::GetInstance()->current_test_info();
	fprintf(log_stream, "=== %s.%s ===\n",
		test_info->test_suite_name(), test_info->name()
	);

	pthread_t thread_even, thread_odd;
	bptr_t root = 0;
	si_args odd_args = {
		.start = 1,
		.end = (TREE_ORDER/2)*(MAX_LEAVES+1),
		.stride = 2,
		.root = &root
	};
	si_args even_args = odd_args;
	even_args.start = odd_args.end;
	even_args.end = odd_args.start % 2 == 0 ? odd_args.start : (odd_args.start + 1);
	even_args.stride = -odd_args.stride;

	for (uint_fast8_t i = 0; i < PARALLEL_RERUNS; ++i) {
		fprintf(log_stream, "Run %d\n", i+1);
		root = 0;
		mem_reset_all(memory);

		pthread_create(&thread_even, NULL, stride_insert, (void*) &even_args);
		pthread_create(&thread_odd, NULL, stride_insert, (void*) &odd_args);
		pthread_join(thread_even, NULL);
		pthread_join(thread_odd, NULL);
		ASSERT_TRUE(even_args.pass);
		ASSERT_TRUE(odd_args.pass);

		dump_node_list(log_stream, memory);

		ASSERT_TRUE(check_inserted_leaves());

		ASSERT_TRUE(validate(root, log_stream, memory));
	}
	fprintf(log_stream, "\n\n");
}


#endif
