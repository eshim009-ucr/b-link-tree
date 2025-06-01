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
	uint_fast8_t offset = 0;
	std::vector<uint_fast8_t> then_splits;
	std::vector<pthread_t> threads(argc-2);
	std::vector<ThreadArgs> threads_args(argc-2);
	// Tree data
	std::vector<std::vector<Request>> reqbufs(argc-2);
	std::vector<std::vector<Response>> respbufs(argc-2);
	bptr_t root = 0;
	// Timing
	clock_t timer;

	if (argc < 3) {
		std::cerr << "Missing request file" << std::endl;
		return 1;
	}

	for (uint_fast8_t i = 0; i < argc-2; ++i) {
		if (std::string(argv[i+2]) == "then") {
			then_splits.push_back(i);
			offset++;
		} else {
			if (read_req_file(argv[i+2], reqbufs.at(i-offset))) {
				std::cerr << "Failed to read file " << argv[i+2] << std::endl;
				return 1;
			}
			respbufs.at(i-offset).resize(reqbufs.at(i-offset).size());
			threads_args.at(i-offset).thread_id = i-offset;
			threads_args.at(i-offset).op_count = reqbufs.at(i-offset).size();
			threads_args.at(i-offset).requests = reqbufs.at(i-offset).data();
			threads_args.at(i-offset).responses = respbufs.at(i-offset).data();
			threads_args.at(i-offset).root = &root;
		}
	}
	then_splits.push_back(argc-2-offset);
	uint_fast8_t last = 0;
	for (uint_fast8_t i = 0; i < then_splits.size(); ++i) {
		const uint_fast8_t sub_count = then_splits.at(i) - last;
		for (uint_fast8_t j = last; j < then_splits.at(i); ++j) {
			threads_args.at(j).thread_count = sub_count;
		}
		last += sub_count;
	}

	// Execute requests
	mem_reset_all(memory);
	uint_fast8_t i = 0;
	for (uint_fast8_t split : then_splits) {
		std::cout << "Executing on " << (int) threads_args.at(i).thread_count << " threads..." << std::flush;
		timer = clock();
		for (; i < split; ++i) {
			pthread_create(&threads.at(i), NULL, tree_thread, (void*) &threads_args.at(i));
		}
		for (; i < split; ++i) {
			pthread_join(threads.at(i), NULL);
		}
		timer = clock() - timer;
		std::cout << "\ncompleted in " << (1000.0d * timer/CLOCKS_PER_SEC) << "ms" << std::endl;
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
