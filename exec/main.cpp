#include "defs.h"
#include "node.h"
#include "thread-runner.hpp"
#include <iostream>
#include <cstring>
#include <cstdio>


FILE *log_stream = fopen("main.log", "w");
Node *memory;


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
		std::cerr << "Usage:\n\t"
			<< argv[0] << " exe [Request File(s)]" << std::endl;
		return 1;
	}
	return exitstatus;
}
