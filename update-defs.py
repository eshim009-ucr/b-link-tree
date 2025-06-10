#!/usr/bin/env python3
from sys import argv
from math import log, ceil
from re import sub


def main():
	if len(argv) > 1:
		TREE_ORDER = int(argv[1])
	else:
		print(
f"""Usage:
	{argv[0]} TREE_ORDER [entry_count]""")
		return 1

	if len(argv) > 2:
		ENTRY_COUNT = int(argv[2])
		print(f"entry_count={ENTRY_COUNT}")
	else:
		ENTRY_COUNT = TREE_ORDER * 10
	ENTRY_MAX = TREE_ORDER
	MAX_NODES_PER_LEVEL = 1
	MAX_LEVELS = 1
	while ENTRY_MAX < ENTRY_COUNT:
		MAX_NODES_PER_LEVEL = ENTRY_MAX
		ENTRY_MAX = (ENTRY_MAX+1) * (TREE_ORDER//2)
		MAX_LEVELS += 1
	MAX_LEAVES = MAX_NODES_PER_LEVEL
	print(f"MAX_LEVELS={MAX_LEVELS}")
	print(f"MAX_LEAVES={MAX_LEAVES}")
	print(f"MAX_NODES_PER_LEVEL=MAX_LEAVES")
	MEM_SIZE = MAX_NODES_PER_LEVEL * MAX_LEVELS
	print(f"MEM_SIZE={MEM_SIZE}")
	print(f"ENTRY_MAX={ENTRY_MAX}")

	# Update file in place
	print("Updating file...", end="")
	with open("defs.h", "r") as fin:
		defs_h_str = fin.read()
	defs_h_str = sub(r"#\s*define\s+TREE_ORDER\s+.+",
		f"#define TREE_ORDER {TREE_ORDER}ULL", defs_h_str)
	defs_h_str = sub(r"#\s*define\s+MEM_SIZE\s+.+",
		f"#define MEM_SIZE {MEM_SIZE}", defs_h_str)
	defs_h_str = sub(r"#\s*define\s+MAX_LEVELS\s+.+",
		f"#define MAX_LEVELS ((bptr_t) {MAX_LEVELS})", defs_h_str)
	defs_h_str = sub(r"#\s*define\s+MAX_NODES_PER_LEVEL\s+.+",
		f"#define MAX_NODES_PER_LEVEL ((bptr_t) {MAX_NODES_PER_LEVEL})", defs_h_str)
	defs_h_str = sub(r"#\s*define\s+MAX_LEAVES\s+.+",
		f"#define MAX_LEAVES ((bptr_t) {MAX_LEAVES})", defs_h_str)
	defs_h_str = sub(r"#\s*define\s+ENTRY_MAX\s+.+",
		f"#define ENTRY_MAX {ENTRY_MAX}UL", defs_h_str)
	with open("defs.h", "w") as fout:
		fout.write(defs_h_str)
	print("Done!")

if __name__ == "__main__":
	main()
