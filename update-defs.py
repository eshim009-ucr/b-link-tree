#!/usr/bin/env python3
from sys import argv
from math import log, ceil
from re import sub


MAX = (1<<64) - 1


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
	# Iteratively determine required tree height
	ENTRY_MAX = TREE_ORDER
	MAX_NODES_PER_LEVEL = 1
	MAX_LEVELS = 1
	while ENTRY_MAX < ENTRY_COUNT:
		ENTRY_MAX = (ENTRY_MAX+1) * (TREE_ORDER//2)
		MAX_LEVELS += 1
	# Don't allocate more leaves than strictly required
	# It will take an absurd amount of memory
	MAX_LEAVES = (ENTRY_COUNT+1) * (TREE_ORDER//2)
	# Shrink oversized variables
	if MAX_LEAVES.bit_length() > 64:
		MAX_LEAVES = MAX
	MAX_NODES_PER_LEVEL = MAX_LEAVES
	# Generate level boundary LUTs
	LEVEL_STARTS = [0] * MAX_LEVELS
	LEVEL_WIDTHS = [0] * MAX_LEVELS
	LEVEL_WIDTHS[0] = MAX_LEAVES
	for i in range(1, MAX_LEVELS):
		LEVEL_WIDTHS[i] = TREE_ORDER ** (MAX_LEVELS-1-i)
		if LEVEL_WIDTHS[i] > LEVEL_WIDTHS[i-1]:
			LEVEL_WIDTHS[i] = LEVEL_WIDTHS[i-1]
		LEVEL_STARTS[i] = LEVEL_STARTS[i-1] + LEVEL_WIDTHS[i-1]
	MEM_SIZE = LEVEL_STARTS[-1] + LEVEL_WIDTHS[-1]
	LEVEL_STARTS.append(MEM_SIZE)
	if MEM_SIZE.bit_length() > 64:
		MEM_SIZE = MAX
	# Display to the user
	print(f"MAX_LEVELS={MAX_LEVELS} (needs {MAX_LEVELS.bit_length()} bits)")
	print(f"MAX_LEAVES={MAX_LEAVES}",
		end=" (overflow, shortening to 64 bits)\n"
		if MAX_LEAVES==MAX else
		f" (needs {MAX_LEAVES.bit_length()} bits)\n")
	print(f"MAX_NODES_PER_LEVEL=MAX_LEAVES")
	print(f"MEM_SIZE={MEM_SIZE} (needs {MEM_SIZE.bit_length()} bits)")
	print(f"ENTRY_MAX={ENTRY_MAX} (needs {ENTRY_MAX.bit_length()} bits)")
	print(f"LEVEL_STARTS={LEVEL_STARTS}")
	print(f"LEVEL_WIDTHS={LEVEL_WIDTHS}")

	# Update file in place
	print("Updating file...", end="")
	with open("core/defs.h", "r") as fin:
		defs_h_str = fin.read()
	defs_h_str = sub(r"#\s*define\s+TREE_ORDER\s+.+",
		f"#define TREE_ORDER {TREE_ORDER}ULL", defs_h_str)
	defs_h_str = sub(r"#\s*define\s+MEM_SIZE\s+.+",
		f"#define MEM_SIZE {MEM_SIZE}ULL", defs_h_str)
	defs_h_str = sub(r"#\s*define\s+MAX_LEVELS\s+.+",
		f"#define MAX_LEVELS ((bptr_t) {MAX_LEVELS})", defs_h_str)
	defs_h_str = sub(r"#\s*define\s+MAX_NODES_PER_LEVEL\s+.+",
		f"#define MAX_NODES_PER_LEVEL ((bptr_t) {MAX_NODES_PER_LEVEL})", defs_h_str)
	defs_h_str = sub(r"#\s*define\s+MAX_LEAVES\s+.+",
		f"#define MAX_LEAVES ((bptr_t) {MAX_LEAVES})", defs_h_str)
	defs_h_str = sub(r"#\s*define\s+ENTRY_MAX\s+.+",
		f"#define ENTRY_MAX {ENTRY_MAX}ULL", defs_h_str)
	defs_h_str = sub(r"#\s*define\s+LEVEL_STARTS\s+.+",
		f"#define LEVEL_STARTS {{{str(LEVEL_STARTS)[1:-1]}}}", defs_h_str)
	defs_h_str = sub(r"#\s*define\s+LEVEL_WIDTHS\s+.+",
		f"#define LEVEL_WIDTHS {{{str(LEVEL_WIDTHS)[1:-1]}}}", defs_h_str)
	with open("core/defs.h", "w") as fout:
		fout.write(defs_h_str)
	print("Done!")

if __name__ == "__main__":
	main()
