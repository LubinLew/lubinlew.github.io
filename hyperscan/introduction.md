# Introduction

Hyperscan is a software regular expression matching engine designed with
high performance and flexibility in mind. It is implemented as a library that
exposes a straightforward C API.

The Hyperscan API itself is composed of two major components:

## Compilation

These functions take a group of regular expressions, along with identifiers and
option flags, and compile them into an immutable database that can be used by
the Hyperscan scanning API. This compilation process performs considerable
analysis and optimization work in order to build a database that will match the
given expressions efficiently.

If a pattern cannot be built into a database for any reason (such as the use of
an unsupported expression construct, or the overflowing of a resource limit),
an error will be returned by the pattern compiler.

Compiled databases can be serialized and relocated, so that they can be stored
to disk or moved between hosts. They can also be targeted to particular
platform features (for example, the use of Intel® Advanced Vector Extensions
2 (Intel® AVX2) instructions).

See [Compiling Patterns](http://intel.github.io/hyperscan/dev-reference/compilation.html#compilation) for more detail.

## Scanning

Once a Hyperscan database has been created, it can be used to scan data in
memory. Hyperscan provides several scanning modes, depending on whether the
data to be scanned is available as a single contiguous block, whether it is
distributed amongst several blocks in memory at the same time, or whether it is
to be scanned as a sequence of blocks in a stream.

Matches are delivered to the application via a user-supplied callback function
that is called synchronously for each match.

For a given database, Hyperscan provides several guarantees:

- No memory allocations occur at runtime with the exception of two
  fixed-size allocations, both of which should be done ahead of time for
  performance-critical applications:
  
  - **Scratch space**: temporary memory used for internal data at scan time.
    Structures in scratch space do not persist beyond the end of a single scan
    call.
  
  - **Stream state**: in streaming mode only, some state space is required to
    store data that persists between scan calls for each stream. This allows
    Hyperscan to track matches that span multiple blocks of data.

- The sizes of the scratch space and stream state (in streaming mode) required
  for a given database are fixed and determined at database compile time. This
  means that the memory requirements of the application are known ahead of
  time, and these structures can be pre-allocated if required for performance
  reasons.

- Any pattern that has successfully been compiled by the Hyperscan compiler can
  be scanned against any input. There are no internal resource limits or other
  limitations at runtime that could cause a scan call to return an error.

See [Scanning for Patterns](http://intel.github.io/hyperscan/dev-reference/runtime.html#runtime) for more detail.

## Tools

Some utilities for testing and benchmarking Hyperscan are included with the
library. See [Tools](http://intel.github.io/hyperscan/dev-reference/tools.html#tools) for more information.

## Example Code[¶](http://intel.github.io/hyperscan/dev-reference/intro.html#example-code "Permalink to this headline")

Some simple example code demonstrating the use of the Hyperscan API is
available in the `examples/` subdirectory of the Hyperscan distribution.
