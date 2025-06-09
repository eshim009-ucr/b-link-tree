#include "defs.h"
#include "misc.hpp"
#include "search.hpp"
#include "insert.hpp"
#include "parallel.hpp"
#include "operations.hpp"
#include <iostream>
#include <cstdio>


FILE *log_stream = fopen("main.log", "w");
Node *memory;


static int run_gtests(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	int status = RUN_ALL_TESTS();
	fclose(log_stream);
	return status;
}


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
	memory = (Node*) malloc(MEM_SIZE * sizeof(Node));
	if (memory == nullptr) {
		on_alloc_fail();
		return 1;
	} else {
		exitstatus = run_gtests(argc, argv);
		free(memory);
	}
	return exitstatus;
}
