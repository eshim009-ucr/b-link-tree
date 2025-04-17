#ifndef TESTS_MISC_HPP
#define TESTS_MISC_HPP


extern "C" {
#include "../io.h"
#include "../memory.h"
#include "../node.h"
#include "../validate.h"
};
#include <gtest/gtest.h>

extern FILE *log_stream;
extern volatile Node memory[MEM_SIZE];


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

	EXPECT_TRUE(is_unlocked(&root, log_stream, memory));
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
	dump_node_list(log_stream, &root.addr, memory);

	EXPECT_FALSE(validate(&root.addr, log_stream, memory));
	EXPECT_TRUE(is_unlocked(&root.addr, log_stream, memory));
	fprintf(log_stream, "\n\n");
}


#endif
