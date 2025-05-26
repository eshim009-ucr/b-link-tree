#include "tests/misc.hpp"
#include "tests/search.hpp"
#include "tests/insert.hpp"
#include "tests/parallel.hpp"
#include "tests/operations.hpp"
#include <cstring>
#include <fstream>


FILE *log_stream = fopen("log.txt", "w");
Node memory[MEM_SIZE];


static int run_gtests(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	int status = RUN_ALL_TESTS();
	fclose(log_stream);
	return status;
}


static int run_from_file(int argc, char **argv) {
	// File I/O
	std::string fname_in, fname_out;
	std::ifstream fin;
	std::ofstream fout;
	// Tree data
	Request reqbuf[0x1000];
	Response respbuf[0x1000];
	bptr_t root = 0;
	Node memory[MEM_SIZE];
	// Iterators
	char* bufptr = (char*) reqbuf;
	uint_fast32_t i_req, i_resp = 0;

	if (argc < 3) {
		std::cerr << "Missing request file" << std::endl;
		return 1;
	}

	fname_in = argv[2];
	if (argc < 4) {
		fname_out = "/dev/null";
	} else {
		fname_out = argv[3];
	}

	fin.open(fname_in, std::ifstream::binary);
	if (!fin.is_open()) {
		return 1;
	}

	// Read request stream
	while (fin.good() && !fin.eof()) {
		fin.read(bufptr + (i_req*sizeof(Request)), sizeof(Request));
		i_req++;
	}
	// "Zero terminate" the request buffer
	((Request*) bufptr)->opcode = NOP;
	fin.close();

	// Execute requests
	mem_reset_all(memory);
	for (; i_resp < i_req; ++i_resp) {
		respbuf[i_resp] = execute_req(reqbuf[i_resp], &root, memory);
	}

	// Write output if requested
	if (fname_out != "/dev/null") {
		bufptr = (char*) respbuf;
		fout.open(fname_out, std::ofstream::binary);
		while (fout.good()) {
			fout.write(bufptr, sizeof(Response));
		}
		fout.close();
	}

	return 0;
}


int main(int argc, char **argv) {
	if (argc < 2 || strcmp(argv[1], "gtest") == 0) {
		return run_gtests(argc, argv);
	} else if (strcmp(argv[1], "exe") == 0) {
		return run_from_file(argc, argv);
	} else {
		std::cerr << "Unrecognized option, \""
			<< argv[1] << "\", valid options are: gtest, exe" << std::endl;
		return 1;
	}
}
