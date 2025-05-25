#include "tests/misc.hpp"
#include "tests/search.hpp"
#include "tests/insert.hpp"
#include "tests/parallel.hpp"
#include "tests/operations.hpp"


FILE *log_stream = fopen("log.txt", "w");
Node memory[MEM_SIZE];


int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	int status = RUN_ALL_TESTS();
	fclose(log_stream);
	return status;
}
