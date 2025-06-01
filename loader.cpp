#include "loader.hpp"
#include <iostream>
#include <fstream>


int read_req_file(const char *filename, std::vector<Request>& reqbuf) {
	std::ifstream fin;
	Request tmp_req;

	fin.open(filename, std::ifstream::binary);
	if (!fin.is_open()) {
		return 1;
	}

	// Read request stream
	std::cout << "Reading request file \"" << filename << "\"..." << std::flush;
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


int write_resp_file(const char *filename, std::vector<Response> const& respbuf) {
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
