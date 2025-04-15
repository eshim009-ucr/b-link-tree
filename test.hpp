extern "C" {
#include "insert.h"
#include "io.h"
#include "memory.h"
#include "node.h"
#include "search.h"
#include "validate.h"
};

#include <pthread.h>
#include <gtest/gtest.h>


FILE *log_stream = fopen("log.txt", "w");
extern Node memory[MEM_SIZE];


TEST(InitTest, Tree) {
	const testing::TestInfo* const test_info =
		testing::UnitTest::GetInstance()->current_test_info();
	fprintf(log_stream, "=== %s.%s ===\n",
		test_info->test_suite_name(), test_info->name()
	);

	bptr_t root = 0;
	mem_reset_all(memory);

	for (bptr_t i = 0; i < MAX_LEVELS * MAX_NODES_PER_LEVEL; ++i) {
		Node n = mem_read(i, memory);
		for (li_t j = 0; j < TREE_ORDER; ++j) {
			EXPECT_EQ(n.keys[j], INVALID);
		}
	}

	EXPECT_TRUE(is_unlocked(root, log_stream, memory));
	fprintf(log_stream, "\n\n");
}

TEST(ValidateTest, RootOneChild) {
	const testing::TestInfo* const test_info =
		testing::UnitTest::GetInstance()->current_test_info();
	fprintf(log_stream, "=== %s.%s ===\n",
		test_info->test_suite_name(), test_info->name()
	);

	AddrNode root;
	root.addr = MAX_LEAVES;
	mem_reset_all(memory);
	root.node = mem_read_lock(root.addr, memory);
	AddrNode lchild = {.node = mem_read_lock(0, memory), .addr = 0};

	root.node.keys[0] = 6; root.node.values[0].ptr = 0;
	lchild.node.keys[0] = 1; lchild.node.values[0].data = -1;
	lchild.node.keys[1] = 2; lchild.node.values[1].data = -2;
	lchild.node.keys[2] = 4; lchild.node.values[2].data = -4;
	lchild.node.keys[3] = 5; lchild.node.values[3].data = -5;
	mem_write_unlock(&root, memory);
	mem_write_unlock(&lchild, memory);
	dump_node_list(log_stream, root.addr, memory);

	EXPECT_FALSE(validate(root.addr, log_stream, memory));
	EXPECT_TRUE(is_unlocked(root.addr, log_stream, memory));
	fprintf(log_stream, "\n\n");
}


#if TREE_ORDER >= 4
TEST(SearchTest, RootIsLeaf) {
	const testing::TestInfo* const test_info =
		testing::UnitTest::GetInstance()->current_test_info();
	fprintf(log_stream, "=== %s.%s ===\n",
		test_info->test_suite_name(), test_info->name()
	);

	AddrNode root;
	root.addr = 0;
	mem_reset_all(memory);
	root.node = mem_read_lock(root.addr, memory);
	bstatusval_t result;

	root.node.keys[0] = 1; root.node.values[0].data = -1;
	root.node.keys[1] = 2; root.node.values[1].data = -2;
	root.node.keys[2] = 4; root.node.values[2].data = -4;
	root.node.keys[3] = 5; root.node.values[3].data = -5;
	mem_write_unlock(&root, memory);
	dump_node_list(log_stream, root.addr, memory);
	EXPECT_EQ(search(root.addr, 0, memory).status, NOT_FOUND);
	EXPECT_EQ(search(root.addr, 3, memory).status, NOT_FOUND);
	EXPECT_EQ(search(root.addr, 6, memory).status, NOT_FOUND);
	result = search(root.addr, 1, memory);
	EXPECT_EQ(result.status, SUCCESS);
	EXPECT_EQ(result.value.data, -1);
	result = search(root.addr, 2, memory);
	EXPECT_EQ(result.status, SUCCESS);
	EXPECT_EQ(result.value.data, -2);
	result = search(root.addr, 4, memory);
	EXPECT_EQ(result.status, SUCCESS);
	EXPECT_EQ(result.value.data, -4);
	result = search(root.addr, 5, memory);
	EXPECT_EQ(result.status, SUCCESS);
	EXPECT_EQ(result.value.data, -5);

	EXPECT_TRUE(validate(root.addr, log_stream, memory));
	EXPECT_TRUE(is_unlocked(root.addr, log_stream, memory));
	fprintf(log_stream, "\n\n");
}
#endif

#if TREE_ORDER == 4
TEST(SearchTest, OneInternal) {
	const testing::TestInfo* const test_info =
		testing::UnitTest::GetInstance()->current_test_info();
	fprintf(log_stream, "=== %s.%s ===\n",
		test_info->test_suite_name(), test_info->name()
	);

	AddrNode root;
	root.addr = MAX_LEAVES;
	mem_reset_all(memory);
	root.node = mem_read_lock(root.addr, memory);
	AddrNode lchild = {.node = mem_read_lock(0, memory), .addr = 0};
	AddrNode rchild = {.node = mem_read_lock(1, memory), .addr = 1};
	bstatusval_t result;

	root.node.keys[0] = 6; root.node.values[0].ptr = 0;
	root.node.keys[1] = 12; root.node.values[1].ptr = 1;
	lchild.node.keys[0] = 1; lchild.node.values[0].data = -1;
	lchild.node.keys[1] = 2; lchild.node.values[1].data = -2;
	lchild.node.keys[2] = 4; lchild.node.values[2].data = -4;
	lchild.node.keys[3] = 5; lchild.node.values[3].data = -5;
	lchild.node.next = 1;
	rchild.node.keys[0] = 7; rchild.node.values[0].data = -7;
	rchild.node.keys[1] = 8; rchild.node.values[1].data = -8;
	rchild.node.keys[2] = 10; rchild.node.values[2].data = -10;
	rchild.node.keys[3] = 11; rchild.node.values[3].data = -11;
	mem_write_unlock(&root, memory);
	mem_write_unlock(&lchild, memory);
	mem_write_unlock(&rchild, memory);
	dump_node_list(log_stream, root.addr, memory);
	EXPECT_EQ(search(root.addr, 0, memory).status, NOT_FOUND);
	EXPECT_EQ(search(root.addr, 3, memory).status, NOT_FOUND);
	EXPECT_EQ(search(root.addr, 6, memory).status, NOT_FOUND);
	EXPECT_EQ(search(root.addr, 9, memory).status, NOT_FOUND);
	EXPECT_EQ(search(root.addr, 12, memory).status, NOT_FOUND);
	result = search(root.addr, 1, memory);
	EXPECT_EQ(result.status, SUCCESS);
	EXPECT_EQ(result.value.data, -1);
	result = search(root.addr, 2, memory);
	EXPECT_EQ(result.status, SUCCESS);
	EXPECT_EQ(result.value.data, -2);
	result = search(root.addr, 4, memory);
	EXPECT_EQ(result.status, SUCCESS);
	EXPECT_EQ(result.value.data, -4);
	result = search(root.addr, 5, memory);
	EXPECT_EQ(result.status, SUCCESS);
	EXPECT_EQ(result.value.data, -5);
	result = search(root.addr, 7, memory);
	EXPECT_EQ(result.status, SUCCESS);
	EXPECT_EQ(result.value.data, -7);
	result = search(root.addr, 8, memory);
	EXPECT_EQ(result.status, SUCCESS);
	EXPECT_EQ(result.value.data, -8);
	result = search(root.addr, 10, memory);
	EXPECT_EQ(result.status, SUCCESS);
	EXPECT_EQ(result.value.data, -10);
	result = search(root.addr, 11, memory);
	EXPECT_EQ(result.status, SUCCESS);
	EXPECT_EQ(result.value.data, -11);

	EXPECT_TRUE(validate(root.addr, log_stream, memory));
	EXPECT_TRUE(is_unlocked(root.addr, log_stream, memory));
	fprintf(log_stream, "\n\n");
}
#endif


TEST(InsertTest, LeafNode) {
	const testing::TestInfo* const test_info =
		testing::UnitTest::GetInstance()->current_test_info();
	fprintf(log_stream, "=== %s.%s ===\n",
		test_info->test_suite_name(), test_info->name()
	);

	bptr_t root = 0;
	mem_reset_all(memory);
	bval_t value;

	value.data = 2;
	EXPECT_EQ(insert(&root, 0, value, memory), SUCCESS);
	EXPECT_EQ(mem_read(root, memory).keys[0], 0);
	EXPECT_EQ(mem_read(root, memory).values[0].data, 2);
	dump_node_list(log_stream, root, memory);
	ASSERT_TRUE(is_unlocked(root, log_stream, memory));

	value.data = 3;
	EXPECT_EQ(insert(&root, 5, value, memory), SUCCESS);
	EXPECT_EQ(mem_read(root, memory).keys[1], 5);
	EXPECT_EQ(mem_read(root, memory).values[1].data, 3);
	dump_node_list(log_stream, root, memory);
	ASSERT_TRUE(is_unlocked(root, log_stream, memory));

	value.data = 1;
	EXPECT_EQ(insert(&root, 3, value, memory), SUCCESS);
	EXPECT_EQ(mem_read(root, memory).keys[1], 3);
	EXPECT_EQ(mem_read(root, memory).values[1].data, 1);
	EXPECT_EQ(mem_read(root, memory).keys[2], 5);
	EXPECT_EQ(mem_read(root, memory).values[2].data, 3);
	dump_node_list(log_stream, root, memory);
	ASSERT_TRUE(is_unlocked(root, log_stream, memory));

	EXPECT_TRUE(validate(root, log_stream, memory));
	fprintf(log_stream, "\n\n");
}

TEST(InsertTest, SplitRoot) {
	const testing::TestInfo* const test_info =
		testing::UnitTest::GetInstance()->current_test_info();
	fprintf(log_stream, "=== %s.%s ===\n",
		test_info->test_suite_name(), test_info->name()
	);

	bptr_t root = 0;
	bptr_t lchild;
	bval_t value;
	mem_reset_all(memory);

	value.data = 0;
	EXPECT_EQ(insert(&root, -value.data, value, memory), SUCCESS);
	dump_node_list(log_stream, root, memory);
	EXPECT_EQ(mem_read(root, memory).keys[0], -value.data);
	EXPECT_EQ(mem_read(root, memory).values[0].data, value.data);
	ASSERT_TRUE(is_unlocked(root, log_stream, memory));
	value.data = -5;
	EXPECT_EQ(insert(&root, -value.data, value, memory), SUCCESS);
	dump_node_list(log_stream, root, memory);
	EXPECT_EQ(mem_read(root, memory).keys[1], -value.data);
	EXPECT_EQ(mem_read(root, memory).values[1].data, value.data);
	ASSERT_TRUE(is_unlocked(root, log_stream, memory));
	value.data = -3;
	EXPECT_EQ(insert(&root, -value.data, value, memory), SUCCESS);
	dump_node_list(log_stream, root, memory);
	EXPECT_EQ(mem_read(root, memory).keys[1], -value.data);
	EXPECT_EQ(mem_read(root, memory).values[1].data, value.data);
	ASSERT_TRUE(is_unlocked(root, log_stream, memory));
	// This one causes a split
	value.data = -1;
	EXPECT_EQ(insert(&root, -value.data, value, memory), SUCCESS);
	dump_node_list(log_stream, root, memory);
	lchild = mem_read(root, memory).values[0].ptr;
	EXPECT_EQ(mem_read(lchild, memory).keys[1], -value.data);
	EXPECT_EQ(mem_read(lchild, memory).values[1].data, value.data);
	ASSERT_TRUE(is_unlocked(root, log_stream, memory));
	value.data = -4;
	EXPECT_EQ(insert(&root, -value.data, value, memory), SUCCESS);
	dump_node_list(log_stream, root, memory);
	ASSERT_TRUE(is_unlocked(root, log_stream, memory));
	ASSERT_TRUE(is_unlocked(lchild, log_stream, memory));

	EXPECT_TRUE(validate(root, log_stream, memory));
	fprintf(log_stream, "\n\n");
}

TEST(InsertTest, SequentialInsert) {
	const testing::TestInfo* const test_info =
		testing::UnitTest::GetInstance()->current_test_info();
	fprintf(log_stream, "=== %s.%s ===\n",
		test_info->test_suite_name(), test_info->name()
	);

	bptr_t root = 0;
	bval_t value;
	mem_reset_all(memory);

	// Insert values
	for (uint_fast8_t i = 1; i <= (TREE_ORDER/2)*(MAX_LEAVES+1); ++i) {
		value.data = -i;
		ASSERT_EQ(insert(&root, i, value, memory), SUCCESS);
		dump_node_list(log_stream, root, memory);
		ASSERT_TRUE(is_unlocked(root, log_stream, memory));
	}
	// Check that they're instantiated in memory correctly
	uint_fast8_t next = 1;
	for (bptr_t i = 0; i < MAX_LEAVES; ++i) {
		for (li_t j = 0; j < TREE_ORDER; ++j) {
			if (mem_read(i, memory).keys[j] == INVALID) {
				break;
			} else {
				EXPECT_EQ(mem_read(i, memory).keys[j], next);
				EXPECT_EQ(mem_read(i, memory).values[j].data, -next);
				next++;
			}
		}
	}

	EXPECT_TRUE(validate(root, log_stream, memory));
	fprintf(log_stream, "\n\n");
}


struct si_args {
	uint_fast16_t start;
	uint_fast16_t end;
	int_fast16_t stride;
	bptr_t *root;
};

void *stride_insert(void *argv){
	si_args args = *(si_args *)argv;
	bval_t value;
	if (args.stride > 0 && args.end > args.start) {
		for (uint_fast16_t i = args.start; i <= args.end; i += args.stride) {
			value.data = -i;
			insert(args.root, i, value, memory);
		}
	} else if (args.stride < 0 && args.end < args.start) {
		for (uint_fast16_t i = args.start; i >= args.end; i += args.stride) {
			value.data = -i;
			insert(args.root, i, value, memory);
			dump_node_list(log_stream, *args.root, memory);
		}
	}
	pthread_exit(NULL);
}

TEST(ParallelTest, InterleavedInsert) {
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

	mem_reset_all(memory);

	pthread_create(&thread_even, NULL, stride_insert, (void*) &even_args);
	pthread_create(&thread_odd, NULL, stride_insert, (void*) &odd_args);
	pthread_join(thread_even, NULL);
	pthread_join(thread_odd, NULL);

	dump_node_list(log_stream, root, memory);

	// Check that they're instantiated in memory correctly
	uint_fast8_t next = 1;
	for (bptr_t i = 0; i < MAX_LEAVES; ++i) {
		for (li_t j = 0; j < TREE_ORDER; ++j) {
			if (mem_read(i, memory).keys[j] == INVALID) {
				break;
			} else {
				EXPECT_EQ(mem_read(i, memory).keys[j], next);
				EXPECT_EQ(mem_read(i, memory).values[j].data, -next);
				next++;
			}
		}
	}

	EXPECT_TRUE(validate(root, log_stream, memory));
	fprintf(log_stream, "\n\n");
}
