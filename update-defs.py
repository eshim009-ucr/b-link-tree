#!/usr/bin/env python3
from sys import argv
from math import log
from re import sub


SUFFIXES = {
	"kB": int(1e3),
	"MB": int(1e6),
	"GB": int(1e9),
	"kiB": int(1<<10),
	"MiB": int(1<<20),
	"GiB": int(1<<30),
}


NODE_SIZE = None
KEY_SIZE = 32//8
VALUE_SIZE = 32//8
LOCK_SIZE = 32//8


def parse_size(size):
	for suffix, multiplier in SUFFIXES.items():
		if size.lower().endswith(suffix.lower()):
			return int(size[:-len(suffix)]) * multiplier // NODE_SIZE
	return int(size)


def main():
	if len(argv) > 1:
		TREE_ORDER = int(argv[1])
		global NODE_SIZE
		NODE_SIZE = KEY_SIZE*TREE_ORDER + VALUE_SIZE*(TREE_ORDER+1) + LOCK_SIZE
	else:
		print(
f"""Usage:
	{argv[0]} TREE_ORDER [MEM_SIZE [MAX_LEVELS [MAX_NODES_PER_LEVEL [MAX_LEAVES]]]]

	MEM_SIZE is a number of nodes by default, but can be interpreted as bytes
	using metric (kB, MB, GB) or binary (kiB, MiB, GiB) suffixes""")
		return 1

	if len(argv) > 2:
		MEM_SIZE_nodes = parse_size(argv[2])
		MEM_SIZE_bytes = MEM_SIZE_nodes
		MEM_SIZE = MEM_SIZE_nodes
	else:
		MEM_SIZE_bytes = parse_size("4KiB")
		MEM_SIZE_nodes = MEM_SIZE_bytes
		MEM_SIZE = MEM_SIZE_nodes
	print(f"MEM_SIZE = {MEM_SIZE}")

	if len(argv) > 3:
		MAX_LEVELS = int(argv[3])
	else:
		MAX_LEVELS = int(log(1 - MEM_SIZE*(1 - TREE_ORDER), TREE_ORDER))
	print(f"MAX_LEVELS = {MAX_LEVELS}")

	if len(argv) > 4:
		MAX_NODES_PER_LEVEL = int(argv[4])
	else:
		MAX_NODES_PER_LEVEL = TREE_ORDER ** (MAX_LEVELS-1)
	print(f"MAX_NODES_PER_LEVEL = {MAX_NODES_PER_LEVEL}")

	if len(argv) > 5:
		MAX_LEAVES = int(argv[5])
	else:
		MAX_LEAVES = "MAX_NODES_PER_LEVEL"
	print(f"MAX_LEAVES = {MAX_LEAVES}")

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
