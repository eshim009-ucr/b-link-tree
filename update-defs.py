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
	MIN_LEAVES = ceil(ENTRY_COUNT/TREE_ORDER)
	print(f"necessary_leaves={MIN_LEAVES}")
	MAX_LEVELS=1+ceil(log(MIN_LEAVES, TREE_ORDER))
	print(f"MAX_LEVELS={MAX_LEVELS}")
	print(TREE_ORDER**MAX_LEVELS)
	MAX_NODES_PER_LEVEL = (TREE_ORDER**(MAX_LEVELS-1))
	print(f"MAX_NODES_PER_LEVEL={MAX_LEVELS}")
	MEM_SIZE = MAX_NODES_PER_LEVEL * MAX_LEVELS
	print(f"MEM_SIZE={MEM_SIZE}")
	MEM_SIZE = "(MAX_NODES_PER_LEVEL * MAX_LEVELS)"
	MAX_LEAVES = "MAX_NODES_PER_LEVEL"

	# Update file in place
	print("Updating file...", end="")
	with open("defs.h", "r") as fin:
		defs_h_str = fin.read()
	defs_h_str = sub(r"#\s*define\s+MEM_SIZE\s+.+",
		f"#define MEM_SIZE {MEM_SIZE}", defs_h_str)
	defs_h_str = sub(r"#\s*define\s+MAX_LEVELS\s+.+",
		f"#define MAX_LEVELS {MAX_LEVELS}", defs_h_str)
	defs_h_str = sub(r"#\s*define\s+MAX_NODES_PER_LEVEL\s+.+",
		f"#define MAX_NODES_PER_LEVEL {MAX_NODES_PER_LEVEL}", defs_h_str)
	defs_h_str = sub(r"#\s*define\s+MAX_LEAVES\s+.+",
		f"#define MAX_LEAVES {MAX_LEAVES}", defs_h_str)
	with open("defs.h", "w") as fout:
		fout.write(defs_h_str)
	print("Done!")

if __name__ == "__main__":
	main()
