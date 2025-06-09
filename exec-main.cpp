#include "defs.h"
#ifndef NO_GTEST
#include "tests/misc.hpp"
#include "tests/search.hpp"
#include "tests/insert.hpp"
#include "tests/parallel.hpp"
#include "tests/operations.hpp"
#else
#include "node.h"
#endif
#include "thread-runner.hpp"
#include <iostream>
#include <cstring>
#include <cstdio>


FILE *log_stream = fopen("main.log", "w");
Node *memory;


#ifndef NO_GTEST
static int run_gtests(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	int status = RUN_ALL_TESTS();
	fclose(log_stream);
	return status;
}
#endif


static void on_alloc_fail() {
	uint_fast64_t tmp = 1000;
	uint_fast8_t i = 0;
	char prefixes[] = {' ', 'k', 'M', 'G', 'T'};
	while (tmp-1 < MEM_SIZE) {
		i++;
		tmp *= 1000;
	}
	std::cerr << "Failed to allocate "
		<< ((double)MEM_SIZE/(tmp/1000)) << prefixes[i] << "B memory" << std::endl;
}


int main(int argc, char **argv) {
	int exitstatus = 0;
	if (argc > 2 && strcmp(argv[1], "exe") == 0) {
		memory = (Node*) malloc(MEM_SIZE * sizeof(Node));
		if (memory == nullptr) {
			on_alloc_fail();
			return 1;
		} else {
			exitstatus = run_from_file(argc, argv);
			free(memory);
		}
	} else {
#ifdef NO_GTEST
		std::cerr << "Usage:\n\t"
			<< argv[0] << " exe [Request File(s)]" << std::endl;
		return 1;
#else
		memory = (Node*) malloc(MEM_SIZE * sizeof(Node));
		if (memory == nullptr) {
			on_alloc_fail();
			return 1;
		} else {
			exitstatus = run_gtests(argc, argv);
			free(memory);
		}
#endif
	}
	return exitstatus;
}
