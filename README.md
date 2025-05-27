B-Link Tree
===========

This is a C implementation of a [B-Link tree][1] (a concurrent variant of the
B+ tree) that uses fully static memory allocation. It is designed with the
intent of being used as a component of an FPGA database accelerator built with
[Vitis HLS][2], but it can also be compiled and run as a standalone C program
for a comparison benchmark or software-style testing and debugging to ensure
correctness.


Compile-Time Options
--------------------

### User-Selectable
These options alter how the tree works in order to easily compare different
implementations of the same abstract functionality.

These are most easily set using the `DEFS` variable in the Makefile, which
accepts a whitespace-delimited list of preprocessor definition names.

#### `OPTIMISTIC_LOCK`
Use [optimistic lock coupling][3] instead of traditional lock coupling. Instead
of blocking access to contested data, the program ``optimistcally'' assumes that
contention will never happens. Version numbers are used to detect modifications
from other processes. If a version mismatch is detected, then the process will
restart the operation.

#### `STACK_ALLOC`
Use a stack-based approach to allocating new nodes instead of the default
grid-based approach.

The stack-based approach partitions available memory into a section for leaves
and a section for inner nodes. Each section has a stack pointer that is moved up
as nodes are allocated. When searching for an empty slot, this avoids
re-checking many of the early-allocated slots that are very likely to be full.
If an empty slot is not found between the top of the stack and the end of the
memory section, the check loops back around to the start of the block to look
for holes left by deleted nodes. This allows for faster node allocation at the
cost of worse locality.

The default grid-based approach views the tree as a rectangular matrix of node
slots, where each row corresponds to a level of the tree. Using the default
rectangular grid wastes slots at higher levels of the tree, but eventually I
hope to implement a version that only allocates the necessary number of nodes
per level.

#### `UNLOCKED`
This effectively disables all locking operations. This can be used to determine
locking overhead or for debugging.


### Automatic
These options are set automatically as part of the build process and should not
be change in the build scripts where they appear. They serve to provide manually
configured alternatives to code that will work when compiled as an ordinary CPU
program, but not when run through HLS to produce an FPGA bitstream (or vice
versa).

#### `FPGA`
This option should be set in the Makefile of an HLS project for all files,
_including_ host CPU files. This is used for when a host CPU program must
interact with the same struct as the RTL version. It is convenient to share
these datatype definitions, even if a host CPU would not be able to correctly
operate on them on its own, as is the case for some lock types.

#### `HLS`
This option should be set in the Makefile of an HLS project for all files that
are passed to Vitis HLS. This will be active during CSIM and COSIM. Because CSIM
does not result in RTL generation, it does not on its own imply RTL generation.

#### `__SYNTHESIS__`
This option is set automatically by the Vitis toolchain when running RTL
generation. This is set during COSIM, but _not_ CSIM.

[1]: https://doi.org/10.1145/319628.319663
[2]: https://www.amd.com/en/products/software/adaptive-socs-and-fpgas/vitis/vitis-hls.html
[3]: https://doi.org/10.1145/2933349.2933352
