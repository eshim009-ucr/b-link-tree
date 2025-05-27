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
	std::vector<Request> reqbuf;
	std::vector<Response> respbuf;
	bptr_t root = 0;
	// Iterators
	Request tmp_req;
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
	std::cout << "Reading request file..." << std::flush;
	while (fin.good() && !fin.eof()) {
		fin.read((char*) &tmp_req, sizeof(Request));
		reqbuf.push_back(tmp_req);
	}
	std::cout << "\nDone!" << std::endl;
	// "Zero terminate" the request buffer
	tmp_req.opcode = NOP;
	reqbuf.push_back(tmp_req);
	fin.close();

	// Execute requests
	std::cout << "Executing " << reqbuf.size() << " requests..." << std::flush;
	mem_reset_all(memory);
	for (Request req : reqbuf) {
		respbuf.push_back(execute_req(req, &root, memory));
		if (respbuf.size() % 10000 == 0) {
			std::cout << "\n\tExecuted " << respbuf.size()
				<< '/' << reqbuf.size()
				<< " (" << 100.0 * respbuf.size() / reqbuf.size() << "%) requests..." << std::flush;
		}
	}
	std::cout << "\nDone!" << std::endl;

	// Write output if requested
	if (fname_out != "/dev/null") {
		std::cout << "Writing response file..." << std::flush;
		fout.open(fname_out, std::ofstream::binary);
		for (Response resp : respbuf) {
			fout.write((char*) &resp, sizeof(Response));
		}
		fout.close();
		std::cout << "Done!" << std::endl;
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
