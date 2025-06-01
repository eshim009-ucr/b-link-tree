#include "tests/misc.hpp"
#include "tests/search.hpp"
#include "tests/insert.hpp"
#include "tests/parallel.hpp"
#include "tests/operations.hpp"
#include "loader.hpp"
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


static int run_gtests(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	int status = RUN_ALL_TESTS();
	fclose(log_stream);
	return status;
}


static int run_from_file(int argc, char **argv) {
	// Thread stuff
	uint_fast8_t thread_count = argc-2;
	std::vector<pthread_t> threads(thread_count);
	std::vector<ThreadArgs> threads_args(thread_count);
	// Tree data
	std::vector<std::vector<Request>> reqbufs(thread_count);
	std::vector<std::vector<Response>> respbufs(thread_count);
	bptr_t root = 0;
	// Timing
	clock_t timer;

	if (argc < 3) {
		std::cerr << "Missing request file" << std::endl;
		return 1;
	}

	for (uint_fast8_t i = 0; i < argc-2; ++i) {
		if (read_req_file(argv[i+2], reqbufs.at(i))) {
			std::cerr << "Failed to read file " << argv[i+2] << std::endl;
			return 1;
		}
		respbufs.at(i).resize(reqbufs.at(i).size());
		threads_args.at(i).thread_id = i;
		threads_args.at(i).thread_count = thread_count;
		threads_args.at(i).op_count = reqbufs.at(i).size();
		threads_args.at(i).requests = reqbufs.at(i).data();
		threads_args.at(i).responses = respbufs.at(i).data();
		threads_args.at(i).root = &root;
	}

	// Execute requests
	std::cout << "Executing on " << thread_count << " threads..." << std::flush;
	mem_reset_all(memory);
	timer = clock();
	for (uint_fast8_t i = 0; i < thread_count; ++i) {
		pthread_create(&threads.at(i), NULL, tree_thread, (void*) &threads_args.at(i));
	}
	for (uint_fast8_t i = 0; i < thread_count; ++i) {
		pthread_join(threads.at(i), NULL);
	}
	timer = clock() - timer;
	std::cout << "\ncompleted in " << (1000.0d * timer/CLOCKS_PER_SEC) << "ms" << std::endl;

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
