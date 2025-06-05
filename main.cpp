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
Node memory[MEM_SIZE];


#ifndef NO_GTEST
static int run_gtests(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	int status = RUN_ALL_TESTS();
	fclose(log_stream);
	return status;
}
#endif


int main(int argc, char **argv) {
	if (argc > 2 && strcmp(argv[1], "exe") == 0) {
		return run_from_file(argc, argv);
	} else {
#ifdef NO_GTEST
		std::cerr << "Usage:\n\t"
			<< argv[0] << " exe [Request File(s)]" << std::endl;
		return 1;
#else
		return run_gtests(argc, argv);
#endif
	}
}
