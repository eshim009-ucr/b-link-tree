B-Link Tree
===========

This is a C implementation of a [B-Link tree][1] (a concurrent variant of the
B+ tree) that uses fully static memory allocation. It is designed with the
intent of being used as a component of an FPGA database accelerator built with
Vitis HLS, but it can also be compiled and run as a standalone C program for
a comparison benchmark or software-style testing and debugging to ensure
correctness.


Compile-Time Options
--------------------

### User-Selectable
These options alter how the tree works in order to easily compare different
implementations of the same abstract functionality.

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
