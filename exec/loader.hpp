#ifndef LOADER_HPP
#define LOADER_HPP


extern "C" {
#include "operations.h"
};
#include <vector>


int read_req_file(const char *filename, std::vector<Request>& reqbuf);
int write_resp_file(const char *filename, std::vector<Response> const& respbuf);


#endif
