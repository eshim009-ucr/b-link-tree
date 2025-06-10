#include "loader.hpp"
#include "thread-runner.hpp"
extern "C" {
#include "memory.h"
#include "node.h"
#include "operations.h"
};
#include <iostream>
#include <stdint.h>
#include <chrono>


#ifndef VERBOSE
#define VERBOSE 0
#endif


#ifndef FPGA
#define ARG_OFFSET 2
#else
#include "host.hpp"
#define ARG_OFFSET 3
#endif


extern Node memory[MEM_SIZE];


struct ThreadArgs {
	uint_fast8_t thread_id;
	uint_fast8_t thread_count;
	uint_fast32_t op_count;
	Request* requests __attribute__ ((aligned (4096)));
	Response* responses;
	bptr_t* root;
};

#ifndef FPGA
static void *tree_thread(void *argv) {
	ThreadArgs* args = (ThreadArgs *)argv;
#ifdef STRIDED
	for (uint_fast64_t i = args->thread_id; i < args->op_count; i += args->thread_count) {
#else // BLOCKED
	const uint_fast64_t BLOCK_SIZE = args->op_count/args->thread_count;
	for (uint_fast64_t i = args->thread_id * BLOCK_SIZE; i < (args->thread_id+1)*BLOCK_SIZE; ++i) {
#if VERBOSE
		if (i != 0 && i % 10000 == 0) {
			std::cout << "\n\t(" << (int) args->thread_id
				<< ") Executed " << i << '/' << BLOCK_SIZE
				<< " (" << (100.0*i/BLOCK_SIZE) << "%) requests";
		}
#endif
#endif
		args->responses[i] = execute_req(args->requests[i], args->root, memory);
	}
	pthread_exit(NULL);
}
#endif


int run_from_file(int argc, char **argv) {
	// Thread stuff
	uint_fast8_t offset = 0;
	uint_fast8_t last = 0;
	std::vector<uint_fast8_t> then_splits;
	std::vector<pthread_t> threads(argc-ARG_OFFSET);
	std::vector<ThreadArgs> threads_args(argc-ARG_OFFSET);
	// Tree data
	std::vector<std::vector<Request>> reqbufs(argc-ARG_OFFSET);
	std::vector<std::vector<Response>> respbufs(argc-ARG_OFFSET);
	bptr_t root = 0;
	// Timing
	clock_t timer;

	if (argc < 3) {
		std::cerr << "Missing request file" << std::endl;
		return 1;
	}

	for (uint_fast8_t i = 0; i < argc-ARG_OFFSET; ++i) {
		if (std::string(argv[i+ARG_OFFSET]) == "then") {
			then_splits.push_back(i);
			offset++;
		} else {
			if (read_req_file(argv[i+ARG_OFFSET], reqbufs.at(i-offset))) {
				std::cerr << "Failed to read file " << argv[i+ARG_OFFSET] << std::endl;
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
	then_splits.push_back(argc-ARG_OFFSET-offset);
	last = 0;
	for (uint_fast8_t i = 0; i < then_splits.size(); ++i) {
		const uint_fast8_t sub_count = then_splits.at(i) - last;
		for (uint_fast8_t j = last; j < then_splits.at(i); ++j) {
			threads_args.at(j).thread_id -= last;
			threads_args.at(j).thread_count = sub_count;
		}
		last += sub_count;
	}

	// Execute requests
	mem_reset_all(memory);
	last = 0;
	for (uint_fast8_t split : then_splits) {
#ifndef FPGA
		std::cout << "Executing on " << (int) threads_args.at(last).thread_count << " threads..." << std::flush;
#else
		std::vector<Request, aligned_allocator<Request> > requests_vec(threads_args.at(last).op_count);
		std::vector<Response, aligned_allocator<Response> > responses_vec(threads_args.at(last).op_count);
		std::vector<Node, aligned_allocator<Node> > memory_vec(MEM_SIZE);
		std::copy(
			threads_args.at(last).requests,
			threads_args.at(last).requests+threads_args.at(last).op_count,
			requests_vec.begin()
		);
		std::copy(
			threads_args.at(last).responses,
			threads_args.at(last).responses+threads_args.at(last).op_count,
			responses_vec.begin()
		);
		std::copy(memory, memory+MEM_SIZE, memory_vec.begin());
		std::cout << "Executing..." << std::flush;
#endif
		auto start = std::chrono::high_resolution_clock::now();
#ifndef FPGA
		for (uint_fast8_t i = last; i < split; ++i) {
			pthread_create(&threads.at(i), NULL, tree_thread, (void*) &threads_args.at(i));
		}
		for (uint_fast8_t i = last; i < split; ++i) {
			pthread_join(threads.at(i), NULL);
		}
		auto end = std::chrono::high_resolution_clock::now();
		last = split;
		double duration_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() / 1000000.0;
#else
		run_fpga_tree(requests_vec, responses_vec, memory_vec, std::string(argv[2]));
#endif
		std::cout << "\ncompleted in " << duration_ms << "ms" << std::endl;
#ifdef FPGA
		std::copy(
			responses_vec.begin(),
			responses_vec.end(),
			threads_args.at(last).responses
		);
		std::copy(memory_vec.begin(), memory_vec.end(), memory);
#endif
	}

	return 0;
}
