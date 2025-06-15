#include "operations.h"
#include "node.h"
#include "search.h"
#include "insert.h"


Response execute_req(Request req, bptr_t *root, Node *memory) {
	Response resp = {.opcode = req.opcode};
	switch (req.opcode) {
		case NOP: break;
		case SEARCH:
			resp.search = search(*root, req.search, memory);
			break;
		case INSERT:
			resp.insert = insert(root, req.insert.key, req.insert.value, memory);
			break;
	}
	return resp;
}
