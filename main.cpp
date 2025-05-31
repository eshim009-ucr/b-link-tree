#include "tests/misc.hpp"
#include "tests/search.hpp"
#include "tests/insert.hpp"
#include "tests/parallel.hpp"
#include "tests/operations.hpp"
#include <cstring>
#include <fstream>


FILE *log_stream = fopen("log.txt", "w");
Node memory[MEM_SIZE];


struct ThreadArgs {
	uint_fast8_t thread_id;
	uint_fast8_t thread_count;
	uint_fast32_t op_count;
	Request* requests;
	Response* responses;
	bptr_t* root;
};

static void *tree_thread(void *argv) {
	ThreadArgs* args = (ThreadArgs *)argv;
#ifdef STRIDED
	for (uint_fast64_t i = args->thread_id; i < args->op_count; i += args->thread_count) {
#else
	for (uint_fast64_t i = 0; i < args->op_count/args->thread_count; ++i) {
#endif
		args->responses[i] = execute_req(args->requests[i], args->root, memory);
	}
	pthread_exit(NULL);
}


static int read_req_file(const char *filename, std::vector<Request>& reqbuf) {
	std::ifstream fin;
	Request tmp_req;

	fin.open(filename, std::ifstream::binary);
	if (!fin.is_open()) {
		return 1;
	}

	// Read request stream
	std::cout << "Reading request file..." << std::flush;
	while (fin.good() && !fin.eof()) {
		fin.read((char*) &tmp_req, sizeof(Request));
		reqbuf.push_back(tmp_req);
		if (reqbuf.size() % 1000000 == 0) {
			std::cout << "\n\tLoaded " << reqbuf.size()
				<< " requests..." << std::flush;
		}
	}
	std::cout << "\nDone!" << std::endl;
	fin.close();

	return 0;
}

static int write_resp_file(const char *filename, std::vector<Response> const& respbuf) {
	std::ofstream fout;
	std::cout << "Writing response file..." << std::flush;
	fout.open(filename, std::ofstream::binary);
	for (Response resp : respbuf) {
		fout.write((char*) &resp, sizeof(Response));
	}
	fout.close();
	std::cout << "Done!" << std::endl;

	return 0;
}


static int run_gtests(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	int status = RUN_ALL_TESTS();
	fclose(log_stream);
	return status;
}


static int run_from_file(int argc, char **argv) {
	// File I/O
	std::string fname_in, fname_out;
	// Tree data
	std::vector<Request> reqbuf;
	std::vector<Response> respbuf;
	bptr_t root = 0;
	// Iterators
	uint_fast8_t thread_count;
	Request tmp_req;
	std::vector<pthread_t> threads;
	std::vector<ThreadArgs> threads_args;

	if (argc < 3) {
		std::cerr << "Missing thread count" << std::endl;
		return 1;
	}
	thread_count = atoi(argv[2]);
	if (thread_count == 0) {
		std::cerr << "Invalid thread count \"" << argv[2]
			<< "\", running with only one 1 thread" << std::endl;
		thread_count = 1;
	}

	if (argc < 4) {
		std::cerr << "Missing request file" << std::endl;
		return 1;
	}

	if (argc < 5) {
		fname_out = "/dev/null";
	} else {
		fname_out = argv[4];
	}
	if (read_req_file(argv[3], reqbuf)) return 1;
	// "Zero terminate" the request buffer
	tmp_req.opcode = NOP;
	for (uint_fast8_t i = 0; i < thread_count; ++i) {
		reqbuf.push_back(tmp_req);
	}

	// Execute requests
	std::cout << "Executing " << reqbuf.size() << " requests on "
		<< (int) thread_count << " threads..." << std::flush;
	mem_reset_all(memory);
	respbuf.resize(reqbuf.size());
	threads.resize(thread_count);
	threads_args.resize(thread_count);
	for (uint_fast8_t i = 0; i < thread_count; ++i) {
		threads_args[i].thread_id = i;
		threads_args[i].thread_count = thread_count;
		threads_args[i].op_count = reqbuf.size();
		threads_args[i].requests = reqbuf.data();
		threads_args[i].responses = respbuf.data();
		threads_args[i].root = &root;
		pthread_create(&threads[i], NULL, tree_thread, (void*) &threads_args[i]);
	}
	for (uint_fast8_t i = 0; i < thread_count; ++i) {
		pthread_join(threads[i], NULL);
	}
	std::cout << "\nDone!" << std::endl;

	// Write output if requested
	if (fname_out != "/dev/null") {
		write_resp_file(fname_out.c_str(), respbuf);
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
