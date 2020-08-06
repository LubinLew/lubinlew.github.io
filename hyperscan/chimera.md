# Chimera [kaɪˈmɪrə]

This section describes Chimera library.

## Introduction

Chimera 是一个软件的正则表达式匹配引擎, 他是Hyperscan和PCRE的混合体. Chimera 的设计目的是完全支持 PCRE 语法, 同时又能有 Hyperscan 的高性能.
Chimera inherits the design guideline of Hyperscan with C APIs for compilation
and scanning.

The Chimera API itself is composed of two major components:

### Compilation

These functions take a group of regular expressions, along with identifiers and
option flags, and compile them into an immutable database that can be used by
the Chimera scanning API. This compilation process performs considerable
analysis and optimization work in order to build a database that will match
the given expressions efficiently.

See [Compiling Patterns](http://intel.github.io/hyperscan/dev-reference/chimera.html#chcompile) for more details

### Scanning

Once a Chimera database has been created, it can be used to scan data in memory.
Chimera only supports block mode in which we scan a single contiguous block in
memory.

Matches are delivered to the application via a user-supplied callback function
that is called synchronously for each match.

For a given database, Chimera provides several guarantees:

- No memory allocations occur at runtime with the exception of scratch space
  allocation, it should be done ahead of time for performance-critical
  applications:
  
  - **Scratch space**: temporary memory used for internal data at scan time.
    Structures in scratch space do not persist beyond the end of a single scan
    call.

- The size of the scratch space required for a given database is fixed and
  determined at database compile time. This means that the memory requirement
  of the application are known ahead of time, and the scratch space can be
  pre-allocated if required for performance reasons.

- Any pattern that has successfully been compiled by the Chimera compiler can
  be scanned against any input. There could be internal resource limits or
  other limitations caused by PCRE at runtime that could cause a scan call to
  return an error.

Note

Chimera is designed to have the same matching behavior as PCRE,
including greedy/ungreedy, capturing, etc. Chimera reports both **start offset** and **end offset** for each match like PCRE. Different
from the fashion of reporting all matches in Hyperscan, Chimera only reports
non-overlapping matches. For example, the pattern `/foofoo/` will
match `foofoofoofoo` at offsets (0, 6) and (6, 12).

Note

Since Chimera is a hybrid of Hyperscan and PCRE in order to support
full PCRE syntax, there will be extra performance overhead compared to
Hyperscan-only solution. Please always use Hyperscan for better performance
unless you must need full PCRE syntax support.

See [Scanning for Patterns](http://intel.github.io/hyperscan/dev-reference/chimera.html#chruntime) for more details

## Requirements

The PCRE library (http://pcre.org/) version 8.41 is required for Chimera.

Note

Since Chimera needs to reference PCRE internal function, please place PCRE source
directory under Hyperscan root directory in order to build Chimera.

Beside this, both hardware and software requirements of Chimera are the same to Hyperscan.
See [Hardware](http://intel.github.io/hyperscan/dev-reference/getting_started.html#hardware) and [Software](http://intel.github.io/hyperscan/dev-reference/getting_started.html#software) for more details.

Note

Building Hyperscan will automatically generate Chimera library.
Currently only static library is supported for Chimera, so please
use static build type when configure CMake build options.

## Compiling Patterns

### Building a Database

The Chimera compiler API accepts regular expressions and converts them into a
compiled pattern database that can then be used to scan data.

The API provides two functions that compile regular expressions into
databases:

1. [`ch_compile()`](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_compile "ch_compile"): compiles a single expression into a pattern database.

2. [`ch_compile_multi()`](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_compile_multi "ch_compile_multi"): compiles an array of expressions into a pattern
   database. All of the supplied patterns will be scanned for concurrently at
   scan time, with user-supplied identifiers returned when they match.

3. [`ch_compile_ext_multi()`](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_compile_ext_multi "ch_compile_ext_multi"): compiles an array of expressions as above,
   but allows PCRE match limits to be specified for each expression.

Compilation allows the Chimera library to analyze the given pattern(s) and
pre-determine how to scan for these patterns in an optimized fashion using
Hyperscan and PCRE.

### Pattern Support

Chimera fully supports the pattern syntax used by the PCRE library (“libpcre”),
described at <[PCRE - Perl Compatible Regular Expressions](http://www.pcre.org/)>.The version of PCRE used to validate
Chimera’s interpretation of this syntax is 8.41.

### Semantics

Chimera supports the exact same semantics of PCRE library. Moreover, it supports
multiple simultaneous pattern matching like Hyperscan and the multiple matches
will be reported in order by end offset.

## Scanning for Patterns

Chimera provides scan function with `ch_scan`.

### Handling Matchessupplied callback function when a match

> *typedef* [ch_callback_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_callback_t "ch_callback_t")`( * ch_match_event_handler)`(unsigned int *id*, unsigned long long *from*, unsigned long long *to*, unsigned int *flags*, unsigned int *size*, const [ch_capture_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_capture_t "ch_capture_t") **captured*, void **ctx*)

The *id* argument will be set to the identifier for the matching expression
provided at compile time, and the *from* argument will be set to the
start-offset of the match the *to* argument will be set to the end-offset
of the match. The *captured* stores offsets of entire pattern match as well as
captured subexpressions. The *size* will be set to the number of valid entries in
the *captured*.

The match callback function has the capability to continue or halt scanning
by returning different values.

See [`ch_match_event_handler`](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_match_event_handler "ch_match_event_handler") for more information.

### Handling Runtime Errors

`ch_scan` will call a user-supplied callback function when a runtime error
occurs in libpcre. This function has the following signature:

> *typedef* [ch_callback_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_callback_t "ch_callback_t")`( * ch_error_event_handler)`([ch_error_event_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_event_t "ch_error_event_t") *error_type*, unsigned int *id*, void **info*, void **ctx*)

The *id* argument will be set to the identifier for the matching expression
provided at compile time.

The match callback function has the capability to either halt scanning or
continue scanning for the next pattern.

See [`ch_error_event_handler`](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_event_handler "ch_error_event_handler") for more information.

### Scratch Space

While scanning data, Chimera needs a small amount of temporary memory to store
on-the-fly internal data. This amount is unfortunately too large to fit on the
stack, particularly for embedded applications, and allocating memory dynamically
is too expensive, so a pre-allocated “scratch” space must be provided to the
scanning functions.

The function [`ch_alloc_scratch()`](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_alloc_scratch "ch_alloc_scratch") allocates a large enough region of
scratch space to support a given database. If the application uses multiple
databases, only a single scratch region is necessary: in this case, calling [`ch_alloc_scratch()`](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_alloc_scratch "ch_alloc_scratch") on each database (with the same `scratch` pointer)
will ensure that the scratch space is large enough to support scanning against
any of the given databases.

While the Chimera library is re-entrant, the use of scratch spaces is not.
For example, if by design it is deemed necessary to run recursive or nested
scanning (say, from the match callback function), then an additional scratch
space is required for that context.

In the absence of recursive scanning, only one such space is required per thread
and can (and indeed should) be allocated before data scanning is to commence.

In a scenario where a set of expressions are compiled by a single “master”
thread and data will be scanned by multiple “worker” threads, the convenience
function [`ch_clone_scratch()`](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_clone_scratch "ch_clone_scratch") allows multiple copies of an existing
scratch space to be made for each thread (rather than forcing the caller to pass
all the compiled databases through [`ch_alloc_scratch()`](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_alloc_scratch "ch_alloc_scratch") multiple times).

For example:

ch_error_t err;
ch_scratch_t *scratch_prototype = NULL;
err = ch_alloc_scratch(db, &scratch_prototype);
if (err != CH_SUCCESS) {
    printf("ch_alloc_scratch failed!");
    exit(1);
}

ch_scratch_t *scratch_thread1 = NULL;
ch_scratch_t *scratch_thread2 = NULL;

err = ch_clone_scratch(scratch_prototype, &scratch_thread1);
if (err != CH_SUCCESS) {
    printf("ch_clone_scratch failed!");
    exit(1);
}
err = ch_clone_scratch(scratch_prototype, &scratch_thread2);
if (err != CH_SUCCESS) {
    printf("ch_clone_scratch failed!");
    exit(1);
}

ch_free_scratch(scratch_prototype);

/* Now two threads can both scan against database db,
 each with its own scratch space. */

### Custom Allocators

By default, structures used by Chimera at runtime (scratch space, etc) are
allocated with the default system allocators, usually `malloc()` and `free()`.

The Chimera API provides a facility for changing this behaviour to support
applications that use custom memory allocators.

These functions are:

- [`ch_set_database_allocator()`](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_set_database_allocator "ch_set_database_allocator"), which sets the allocate and free functions
  used for compiled pattern databases.

- [`ch_set_scratch_allocator()`](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_set_scratch_allocator "ch_set_scratch_allocator"), which sets the allocate and free
  functions used for scratch space.

- [`ch_set_misc_allocator()`](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_set_misc_allocator "ch_set_misc_allocator"), which sets the allocate and free functions
  used for miscellaneous data, such as compile error structures and
  informational strings.

The [`ch_set_allocator()`](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_set_allocator "ch_set_allocator") function can be used to set all of the custom
allocators to the same allocate/free pair.

## API Reference: Constants

### Error Codes

`CH_SUCCESS`

The engine completed normally.

`CH_INVALID`

A parameter passed to this function was invalid.

`CH_NOMEM`

A memory allocation failed.

`CH_SCAN_TERMINATED`

The engine was terminated by callback.

This return value indicates that the target buffer was partially 
scanned, but that the callback function requested that scanning cease 
after a match was located.

`CH_COMPILER_ERROR`

The pattern compiler failed, and the [ch_compile_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a8a5035f7767f59b039981a37ce8572da) should be inspected for more detail.

`CH_DB_VERSION_ERROR`

The given database was built for a different version of the Chimera matcher.

`CH_DB_PLATFORM_ERROR`

The given database was built for a different platform (i.e., CPU type).

`CH_DB_MODE_ERROR`

The given database was built for a different mode of operation. 
This error is returned when streaming calls are used with a 
non-streaming database and vice versa.

`CH_BAD_ALIGN`

A parameter passed to this function was not correctly aligned.

`CH_BAD_ALLOC`

The memory allocator did not correctly return memory suitably aligned for the largest representable data type on this platform.

`CH_SCRATCH_IN_USE`

The scratch region was already in use.

This error is returned when Chimera is able to detect that the 
scratch region given is already in use by another Chimera API call.

A separate scratch region, allocated with [ch_alloc_scratch()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a3c7d88639ce32646f9aaf5b57b2d0b1d) or [ch_clone_scratch()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a15e83801bb74dfff8c4d8623f64f967c), is required for every concurrent caller of the Chimera API.

For example, this error might be returned when [ch_scan()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a2a297a612f301abfb5042b680381df6e) has been called inside a callback delivered by a currently-executing [ch_scan()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a2a297a612f301abfb5042b680381df6e) call using the same scratch region.

Note: Not all concurrent uses of scratch regions may be detected. 
This error is intended as a best-effort debugging tool, not a guarantee.

`CH_FAIL_INTERNAL`

Returned when pcre_exec (called for some expressions internally from [ch_scan](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a2a297a612f301abfb5042b680381df6e)) failed due to a fatal error.

### Pattern flags

`CH_FLAG_CASELESS`

Compile flag: Set case-insensitive matching.

This flag sets the expression to be matched case-insensitively by default. The expression may still use PCRE tokens (notably `(?i)` and `(?-i)`) to switch case-insensitive matching on and off.

`CH_FLAG_DOTALL`

Compile flag: Matching a `.` will not exclude newlines.

This flag sets any instances of the `.` token to match newline characters as well as all other characters. The PCRE specification states that the `.` token does not match newline characters by default, so without this flag the `.` token will not cross line boundaries.

`CH_FLAG_MULTILINE`

Compile flag: Set multi-line anchoring.

This flag instructs the expression to make the `^` and `$` tokens match newline characters as well as the start and end of the stream. If this flag is not specified, the `^` token will only ever match at the start of a stream, and the `$` token will only ever match at the end of a stream within the guidelines of the PCRE specification.

`CH_FLAG_SINGLEMATCH`

Compile flag: Set single-match only mode.

This flag sets the expression’s match ID to match at most once, only the first match for each invocation of [ch_scan()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a2a297a612f301abfb5042b680381df6e) will be returned.

`CH_FLAG_UTF8`

Compile flag: Enable UTF-8 mode for this expression.

This flag instructs Chimera to treat the pattern as a sequence of 
UTF-8 characters. The results of scanning invalid UTF-8 sequences with a
 Chimera library that has been compiled with one or more patterns using 
this flag are undefined.

`CH_FLAG_UCP`

Compile flag: Enable Unicode property support for this expression.

This flag instructs Chimera to use Unicode properties, rather than 
the default ASCII interpretations, for character mnemonics like `\w` and `\s` as well as the POSIX character classes. It is only meaningful in conjunction with [CH_FLAG_UTF8](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__PATTERN__FLAG_1ga007859d8e84ef50bd6547af87e13d7e4).

### Compile mode flags

`CH_MODE_NOGROUPS`

Compiler mode flag: Disable capturing groups.

`CH_MODE_GROUPS`

Compiler mode flag: Enable capturing groups.

## API Reference: Files

### File: ch.h

The complete Chimera API definition.

Chimera is a hybrid solution of Hyperscan and PCRE.

This header includes both the Chimera compiler and runtime components. See the individual component headers for documentation.

### File: ch_common.h

The Chimera common API definition.

Chimera is a hybrid of Hyperscan and PCRE.

This header contains functions available to both the Chimera compiler and runtime.

Defines

`CH_SUCCESS`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_SUCCESS "Permalink to this definition")

The engine completed normally.

`CH_INVALID`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_INVALID "Permalink to this definition")

A parameter passed to this function was invalid.

`CH_NOMEM`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_NOMEM "Permalink to this definition")

A memory allocation failed.

`CH_SCAN_TERMINATED`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_SCAN_TERMINATED "Permalink to this definition")

The engine was terminated by callback.

This return value indicates that the target buffer was partially 
scanned, but that the callback function requested that scanning cease 
after a match was located.

`CH_COMPILER_ERROR`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_COMPILER_ERROR "Permalink to this definition")

The pattern compiler failed, and the [ch_compile_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a8a5035f7767f59b039981a37ce8572da) should be inspected for more detail.

`CH_DB_VERSION_ERROR`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_DB_VERSION_ERROR "Permalink to this definition")

The given database was built for a different version of the Chimera matcher.

`CH_DB_PLATFORM_ERROR`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_DB_PLATFORM_ERROR "Permalink to this definition")

The given database was built for a different platform (i.e., CPU type).

`CH_DB_MODE_ERROR`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_DB_MODE_ERROR "Permalink to this definition")

The given database was built for a different mode of operation. 
This error is returned when streaming calls are used with a 
non-streaming database and vice versa.

`CH_BAD_ALIGN`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_BAD_ALIGN "Permalink to this definition")

A parameter passed to this function was not correctly aligned.

`CH_BAD_ALLOC`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_BAD_ALLOC "Permalink to this definition")

The memory allocator did not correctly return memory suitably aligned for the largest representable data type on this platform.

`CH_SCRATCH_IN_USE`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_SCRATCH_IN_USE "Permalink to this definition")

The scratch region was already in use.

This error is returned when Chimera is able to detect that the 
scratch region given is already in use by another Chimera API call.

A separate scratch region, allocated with [ch_alloc_scratch()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a3c7d88639ce32646f9aaf5b57b2d0b1d) or [ch_clone_scratch()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a15e83801bb74dfff8c4d8623f64f967c), is required for every concurrent caller of the Chimera API.

For example, this error might be returned when [ch_scan()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a2a297a612f301abfb5042b680381df6e) has been called inside a callback delivered by a currently-executing [ch_scan()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a2a297a612f301abfb5042b680381df6e) call using the same scratch region.

Note: Not all concurrent uses of scratch regions may be detected. 
This error is intended as a best-effort debugging tool, not a guarantee.

`CH_FAIL_INTERNAL`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_FAIL_INTERNAL "Permalink to this definition")

Returned when pcre_exec (called for some expressions internally from [ch_scan](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a2a297a612f301abfb5042b680381df6e)) failed due to a fatal error.

Typedefs

*typedef* struct ch_database `ch_database_t`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_database_t "Permalink to this definition")

A Chimera pattern database.

Generated by one of the Chimera compiler functions:

- [ch_compile()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a6a013051b2cff9ab317f09ff99efc4ce)

- [ch_compile_multi()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a42e42ceae9e88e63bddfd66b80ea8ee6)

- [ch_compile_ext_multi()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1aa74568707164b8efc98cd88c5f332db2)

*typedef* int `ch_error_t`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "Permalink to this definition")

A type for errors returned by Chimera functions.

*typedef* void*`( * ch_alloc_t)`(size_t *size*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_alloc_t "Permalink to this definition")

The type of the callback function that will be used by Chimera to allocate more memory at runtime as required.

If Chimera is to be used in a multi-threaded, or similarly concurrent
 environment, the allocation function will need to be re-entrant, or 
similarly safe for concurrent use.

**Return**

A pointer to the region of memory allocated, or NULL on error.

**Parameters**

- `size`: The number of bytes to allocate.

*typedef* void`( * ch_free_t)`(void **ptr*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_free_t "Permalink to this definition")

The type of the callback function that will be used by Chimera to free memory regions previously allocated using the [ch_alloc_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a3a2d4e06ecf58339d4bccde1fae4c698) function.

**Parameters**

- `ptr`: The region of memory to be freed.

Functions

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_free_database`([ch_database_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_database_t "ch_database_t") * *db*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_free_database "Permalink to this definition")

Free a compiled pattern database.

The free callback set by [ch_set_allocator()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a06b7bdbcc2e56c57771271025e33b93b)) will be used by this function.

**Return**

[CH_SUCCESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga2ac91c9a68ace90cec58294dde94bc67) on success, other values on failure.

**Parameters**

- `db`: A compiled pattern database. NULL may also be safely provided, in which case the function does nothing.

const char* `ch_version`(void)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_version "Permalink to this definition")

Utility function for identifying this release version.

**Return**

A string containing the version 
number of this release build and the date of the build. It is allocated 
statically, so it does not need to be freed by the caller.

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_database_size`(const [ch_database_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_database_t "ch_database_t") * *database*, size_t * *database_size*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_database_size "Permalink to this definition")

Returns the size of the given database.

**Return**

[CH_SUCCESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga2ac91c9a68ace90cec58294dde94bc67) on success, other values on failure.

**Parameters**

- `database`: Pointer to compiled expression database.

- `database_size`: On success, the size of the compiled database in bytes is placed in this parameter.

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_database_info`(const [ch_database_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_database_t "ch_database_t") * *database*, char ** *info*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_database_info "Permalink to this definition")

Utility function providing information about a database.

**Return**

[CH_SUCCESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga2ac91c9a68ace90cec58294dde94bc67) on success, other values on failure.

**Parameters**

- `database`: Pointer to a compiled database.

- `info`:
   On success, a string containing the version and platform information 
  for the supplied database is placed in the parameter. The string is 
  allocated using the allocator supplied in [hs_set_allocator()](http://intel.github.io/hyperscan/dev-reference/api_files.html#hs__common_8h_1a5ca2c6de1dac5bb462e5cf4ab14de3bb) (or malloc() if no allocator was set) and should be freed by the caller.

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_set_allocator`([ch_alloc_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_alloc_t "ch_alloc_t") *alloc_func*, [ch_free_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_free_t "ch_free_t") *free_func*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_set_allocator "Permalink to this definition")

Set the allocate and free functions used by Chimera for 
allocating memory at runtime for stream state, scratch space, database 
bytecode, and various other data structure returned by the Chimera API.

The function is equivalent to calling [ch_set_scratch_allocator()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a1a1378a43cfb81f8628a3452f5cc2fce), [ch_set_database_allocator()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a518e61120b451f8eca1334b114827331) and [ch_set_misc_allocator()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a6a5003ba83834fce7f7fb7e7eb535509) with the provided parameters.

This call will override any previous allocators that have been set.

Note: there is no way to change the allocator used for temporary objects created during the various compile calls ([ch_compile()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a6a013051b2cff9ab317f09ff99efc4ce) and [ch_compile_multi()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a42e42ceae9e88e63bddfd66b80ea8ee6)).

**Return**

[CH_SUCCESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga2ac91c9a68ace90cec58294dde94bc67) on success, other values on failure.

**Parameters**

- `alloc_func`:
   A callback function pointer that allocates memory. This function must 
  return memory suitably aligned for the largest representable data type 
  on this platform.

- `free_func`: A callback function pointer that frees allocated memory.

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_set_database_allocator`([ch_alloc_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_alloc_t "ch_alloc_t") *alloc_func*, [ch_free_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_free_t "ch_free_t") *free_func*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_set_database_allocator "Permalink to this definition")

Set the allocate and free functions used by Chimera for allocating memory for database bytecode produced by the compile calls ([ch_compile()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a6a013051b2cff9ab317f09ff99efc4ce) and [ch_compile_multi()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a42e42ceae9e88e63bddfd66b80ea8ee6)).

If no database allocation functions are set, or if NULL is used in 
place of both parameters, then memory allocation will default to 
standard methods (such as the system malloc() and free() calls).

This call will override any previous database allocators that have been set.

Note: the database allocator may also be set by calling [ch_set_allocator()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a06b7bdbcc2e56c57771271025e33b93b).

Note: there is no way to change how temporary objects created during the various compile calls ([ch_compile()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a6a013051b2cff9ab317f09ff99efc4ce) and [ch_compile_multi()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a42e42ceae9e88e63bddfd66b80ea8ee6)) are allocated.

**Return**

[HS_SUCCESS](http://intel.github.io/hyperscan/dev-reference/api_files.html#group__HS__ERROR_1ga2d533509db8628a7e0eb794e2b3e77b0) on success, other values on failure.

**Parameters**

- `alloc_func`:
   A callback function pointer that allocates memory. This function must 
  return memory suitably aligned for the largest representable data type 
  on this platform.

- `free_func`: A callback function pointer that frees allocated memory.

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_set_misc_allocator`([ch_alloc_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_alloc_t "ch_alloc_t") *alloc_func*, [ch_free_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_free_t "ch_free_t") *free_func*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_set_misc_allocator "Permalink to this definition")

Set the allocate and free functions used by Chimera for allocating memory for items returned by the Chimera API such as [ch_compile_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a8a5035f7767f59b039981a37ce8572da).

If no misc allocation functions are set, or if NULL is used in place 
of both parameters, then memory allocation will default to standard 
methods (such as the system malloc() and free() calls).

This call will override any previous misc allocators that have been set.

Note: the misc allocator may also be set by calling [ch_set_allocator()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a06b7bdbcc2e56c57771271025e33b93b).

**Return**

[CH_SUCCESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga2ac91c9a68ace90cec58294dde94bc67) on success, other values on failure.

**Parameters**

- `alloc_func`:
   A callback function pointer that allocates memory. This function must 
  return memory suitably aligned for the largest representable data type 
  on this platform.

- `free_func`: A callback function pointer that frees allocated memory.

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_set_scratch_allocator`([ch_alloc_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_alloc_t "ch_alloc_t") *alloc_func*, [ch_free_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_free_t "ch_free_t") *free_func*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_set_scratch_allocator "Permalink to this definition")

Set the allocate and free functions used by Chimera for allocating memory for scratch space by [ch_alloc_scratch()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a3c7d88639ce32646f9aaf5b57b2d0b1d) and [ch_clone_scratch()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a15e83801bb74dfff8c4d8623f64f967c).

If no scratch allocation functions are set, or if NULL is used in 
place of both parameters, then memory allocation will default to 
standard methods (such as the system malloc() and free() calls).

This call will override any previous scratch allocators that have been set.

Note: the scratch allocator may also be set by calling [ch_set_allocator()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a06b7bdbcc2e56c57771271025e33b93b).

**Return**

[CH_SUCCESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga2ac91c9a68ace90cec58294dde94bc67) on success, other values on failure.

**Parameters**

- `alloc_func`:
   A callback function pointer that allocates memory. This function must 
  return memory suitably aligned for the largest representable data type 
  on this platform.

- `free_func`: A callback function pointer that frees allocated memory.

### File: ch_compile.h

The Chimera compiler API definition.

Chimera is a hybrid solution of Hyperscan and PCRE.

This header contains functions for compiling regular expressions into
 Chimera databases that can be used by the Chimera runtime.

Defines

`CH_FLAG_CASELESS`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_FLAG_CASELESS "Permalink to this definition")

Compile flag: Set case-insensitive matching.

This flag sets the expression to be matched case-insensitively by default. The expression may still use PCRE tokens (notably `(?i)` and `(?-i)`) to switch case-insensitive matching on and off.

`CH_FLAG_DOTALL`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_FLAG_DOTALL "Permalink to this definition")

Compile flag: Matching a `.` will not exclude newlines.

This flag sets any instances of the `.` token to match newline characters as well as all other characters. The PCRE specification states that the `.` token does not match newline characters by default, so without this flag the `.` token will not cross line boundaries.

`CH_FLAG_MULTILINE`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_FLAG_MULTILINE "Permalink to this definition")

Compile flag: Set multi-line anchoring.

This flag instructs the expression to make the `^` and `$` tokens match newline characters as well as the start and end of the stream. If this flag is not specified, the `^` token will only ever match at the start of a stream, and the `$` token will only ever match at the end of a stream within the guidelines of the PCRE specification.

`CH_FLAG_SINGLEMATCH`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_FLAG_SINGLEMATCH "Permalink to this definition")

Compile flag: Set single-match only mode.

This flag sets the expression’s match ID to match at most once, only the first match for each invocation of [ch_scan()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a2a297a612f301abfb5042b680381df6e) will be returned.

`CH_FLAG_UTF8`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_FLAG_UTF8 "Permalink to this definition")

Compile flag: Enable UTF-8 mode for this expression.

This flag instructs Chimera to treat the pattern as a sequence of 
UTF-8 characters. The results of scanning invalid UTF-8 sequences with a
 Chimera library that has been compiled with one or more patterns using 
this flag are undefined.

`CH_FLAG_UCP`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_FLAG_UCP "Permalink to this definition")

Compile flag: Enable Unicode property support for this expression.

This flag instructs Chimera to use Unicode properties, rather than 
the default ASCII interpretations, for character mnemonics like `\w` and `\s` as well as the POSIX character classes. It is only meaningful in conjunction with [CH_FLAG_UTF8](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__PATTERN__FLAG_1ga007859d8e84ef50bd6547af87e13d7e4).

`CH_MODE_NOGROUPS`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_MODE_NOGROUPS "Permalink to this definition")

Compiler mode flag: Disable capturing groups.

`CH_MODE_GROUPS`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_MODE_GROUPS "Permalink to this definition")

Compiler mode flag: Enable capturing groups.

Typedefs

*typedef* struct [ch_compile_error](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_compile_error "ch_compile_error") `ch_compile_error_t`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_compile_error_t "Permalink to this definition")

A type containing error details that is returned by the compile calls ([ch_compile()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a6a013051b2cff9ab317f09ff99efc4ce) and [ch_compile_multi()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a42e42ceae9e88e63bddfd66b80ea8ee6) on failure. The caller may inspect the values returned in this type to determine the cause of failure.

Functions

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_compile`(const char * *expression*, unsigned int *flags*, unsigned int *mode*, const [hs_platform_info_t](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_platform_info_t "hs_platform_info_t") * *platform*, [ch_database_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_database_t "ch_database_t") ** *db*, [ch_compile_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_compile_error_t "ch_compile_error_t") ** *compile_error*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_compile "Permalink to this definition")

The basic regular expression compiler.

This is the function call with which an expression is compiled into a
 Chimera database which can be passed to the runtime function ( [ch_scan()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a2a297a612f301abfb5042b680381df6e))

**Return**

[CH_SUCCESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga2ac91c9a68ace90cec58294dde94bc67) is returned on successful compilation; [CH_COMPILER_ERROR](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga1f5afef70e16c03380b518063346af4b) on failure, with details provided in the error parameter.

**Parameters**

- `expression`:
   The NULL-terminated expression to parse. Note that this string must 
  represent ONLY the pattern to be matched, with no delimiters or flags; 
  any global flags should be specified with the *flags* argument. For example, the expression `/abc?def/i` should be compiled by providing `abc?def` as the *expression*, and [CH_FLAG_CASELESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__PATTERN__FLAG_1gac5aa4ab282c6758aaac1001525d7695e) as the *flags*.

- `flags`: Flags which modify the behaviour of the expression. Multiple flags may be used by ORing them together. Valid values are:
  
  - CH_FLAG_CASELESS - Matching will be performed case-insensitively.
  
  - CH_FLAG_DOTALL - Matching a `.` will not exclude newlines.
  
  - CH_FLAG_MULTILINE - `^` and `$` anchors match any newlines in data.
  
  - CH_FLAG_SINGLEMATCH - Only one match will be generated for the expression per stream.
  
  - CH_FLAG_UTF8 - Treat this pattern as a sequence of UTF-8 characters.
  
  - CH_FLAG_UCP - Use Unicode properties for character classes.

- `mode`:
   Compiler mode flag that affect the database as a whole for capturing 
  groups. One of CH_MODE_NOGROUPS or CH_MODE_GROUPS must be supplied. See 
  Compile mode flags for more details.

- `platform`:
   If not NULL, the platform structure is used to determine the target 
  platform for the database. If NULL, a database suitable for running on 
  the current host platform is produced.

- `db`:
   On success, a pointer to the generated database will be returned in 
  this parameter, or NULL on failure. The caller is responsible for 
  deallocating the buffer using the [ch_free_database()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a1e87569ac35ac18c4212c0ced9a7323b) function.

- `compile_error`: If the compile fails, a pointer to a [ch_compile_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a8a5035f7767f59b039981a37ce8572da) will be returned, providing details of the error condition. The caller is responsible for deallocating the buffer using the [ch_free_compile_error()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1ad381fe6cf9daade9e7bf5f12d5037c6c) function.

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_compile_multi`(const char *const * *expressions*, const unsigned int * *flags*, const unsigned int * *ids*, unsigned int *elements*, unsigned int *mode*, const [hs_platform_info_t](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_platform_info_t "hs_platform_info_t") * *platform*, [ch_database_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_database_t "ch_database_t") ** *db*, [ch_compile_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_compile_error_t "ch_compile_error_t") ** *compile_error*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_compile_multi "Permalink to this definition")

The multiple regular expression compiler.

This is the function call with which a set of expressions is compiled
 into a database which can be passed to the runtime function ([ch_scan()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a2a297a612f301abfb5042b680381df6e)).
 Each expression can be labelled with a unique integer which is passed 
into the match callback to identify the pattern that has matched.

**Return**

[CH_SUCCESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga2ac91c9a68ace90cec58294dde94bc67) is returned on successful compilation; [CH_COMPILER_ERROR](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga1f5afef70e16c03380b518063346af4b) on failure, with details provided in the *error* parameter.

**Parameters**

- `expressions`: Array of NULL-terminated expressions to compile. Note that (as for [ch_compile()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a6a013051b2cff9ab317f09ff99efc4ce)) these strings must contain only the pattern to be matched, with no delimiters or flags. For example, the expression `/abc?def/i` should be compiled by providing `abc?def` as the first string in the *expressions* array, and [CH_FLAG_CASELESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__PATTERN__FLAG_1gac5aa4ab282c6758aaac1001525d7695e) as the first value in the *flags* array.

- `flags`:
   Array of flags which modify the behaviour of each expression. Multiple 
  flags may be used by ORing them together. Specifying the NULL pointer in
   place of an array will set the flags value for all patterns to zero. 
  Valid values are:
  
  - CH_FLAG_CASELESS - Matching will be performed case-insensitively.
  
  - CH_FLAG_DOTALL - Matching a `.` will not exclude newlines.
  
  - CH_FLAG_MULTILINE - `^` and `$` anchors match any newlines in data.
  
  - CH_FLAG_SINGLEMATCH - Only one match will be generated by patterns with this match id per stream.
  
  - CH_FLAG_UTF8 - Treat this pattern as a sequence of UTF-8 characters.
  
  - CH_FLAG_UCP - Use Unicode properties for character classes.

- `ids`:
   An array of integers specifying the ID number to be associated with the
   corresponding pattern in the expressions array. Specifying the NULL 
  pointer in place of an array will set the ID value for all patterns to 
  zero.

- `elements`: The number of elements in the input arrays.

- `mode`:
   Compiler mode flag that affect the database as a whole for capturing 
  groups. One of CH_MODE_NOGROUPS or CH_MODE_GROUPS must be supplied. See 
  Compile mode flags for more details.

- `platform`:
   If not NULL, the platform structure is used to determine the target 
  platform for the database. If NULL, a database suitable for running on 
  the current host platform is produced.

- `db`:
   On success, a pointer to the generated database will be returned in 
  this parameter, or NULL on failure. The caller is responsible for 
  deallocating the buffer using the [ch_free_database()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a1e87569ac35ac18c4212c0ced9a7323b) function.

- `compile_error`: If the compile fails, a pointer to a [ch_compile_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a8a5035f7767f59b039981a37ce8572da) will be returned, providing details of the error condition. The caller is responsible for deallocating the buffer using the [ch_free_compile_error()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1ad381fe6cf9daade9e7bf5f12d5037c6c) function.

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_compile_ext_multi`(const char *const * *expressions*, const unsigned int * *flags*, const unsigned int * *ids*, unsigned int *elements*, unsigned int *mode*, unsigned long int *match_limit*, unsigned long int *match_limit_recursion*, const [hs_platform_info_t](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_platform_info_t "hs_platform_info_t") * *platform*, [ch_database_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_database_t "ch_database_t") ** *db*, [ch_compile_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_compile_error_t "ch_compile_error_t") ** *compile_error*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_compile_ext_multi "Permalink to this definition")

The multiple regular expression compiler with extended match limits support.

This is the function call with which a set of expressions is compiled into a database in the same way as [ch_compile_multi()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a42e42ceae9e88e63bddfd66b80ea8ee6),
 but allows additional parameters to be specified via match_limit and 
match_limit_recursion to define match limits for PCRE runtime.

**Return**

[CH_SUCCESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga2ac91c9a68ace90cec58294dde94bc67) is returned on successful compilation; [CH_COMPILER_ERROR](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga1f5afef70e16c03380b518063346af4b) on failure, with details provided in the *error* parameter.

**Parameters**

- `expressions`: Array of NULL-terminated expressions to compile. Note that (as for [ch_compile()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a6a013051b2cff9ab317f09ff99efc4ce)) these strings must contain only the pattern to be matched, with no delimiters or flags. For example, the expression `/abc?def/i` should be compiled by providing `abc?def` as the first string in the *expressions* array, and [CH_FLAG_CASELESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__PATTERN__FLAG_1gac5aa4ab282c6758aaac1001525d7695e) as the first value in the *flags* array.

- `flags`:
   Array of flags which modify the behaviour of each expression. Multiple 
  flags may be used by ORing them together. Specifying the NULL pointer in
   place of an array will set the flags value for all patterns to zero. 
  Valid values are:
  
  - CH_FLAG_CASELESS - Matching will be performed case-insensitively.
  
  - CH_FLAG_DOTALL - Matching a `.` will not exclude newlines.
  
  - CH_FLAG_MULTILINE - `^` and `$` anchors match any newlines in data.
  
  - CH_FLAG_SINGLEMATCH - Only one match will be generated by patterns with this match id per stream.
  
  - CH_FLAG_UTF8 - Treat this pattern as a sequence of UTF-8 characters.
  
  - CH_FLAG_UCP - Use Unicode properties for character classes.

- `ids`:
   An array of integers specifying the ID number to be associated with the
   corresponding pattern in the expressions array. Specifying the NULL 
  pointer in place of an array will set the ID value for all patterns to 
  zero.

- `elements`: The number of elements in the input arrays.

- `mode`:
   Compiler mode flag that affect the database as a whole for capturing 
  groups. One of CH_MODE_NOGROUPS or CH_MODE_GROUPS must be supplied. See 
  Compile mode flags for more details.

- `match_limit`: A limit from pcre_extra on the amount of match function called in PCRE to limit backtracking that can take place.

- `match_limit_recursion`: A limit from pcre_extra on the recursion depth of match function in PCRE.

- `platform`:
   If not NULL, the platform structure is used to determine the target 
  platform for the database. If NULL, a database suitable for running on 
  the current host platform is produced.

- `db`:
   On success, a pointer to the generated database will be returned in 
  this parameter, or NULL on failure. The caller is responsible for 
  deallocating the buffer using the [ch_free_database()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a1e87569ac35ac18c4212c0ced9a7323b) function.

- `compile_error`: If the compile fails, a pointer to a [ch_compile_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a8a5035f7767f59b039981a37ce8572da) will be returned, providing details of the error condition. The caller is responsible for deallocating the buffer using the [ch_free_compile_error()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1ad381fe6cf9daade9e7bf5f12d5037c6c) function.

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_free_compile_error`([ch_compile_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_compile_error_t "ch_compile_error_t") * *error*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_free_compile_error "Permalink to this definition")

Free an error structure generated by [ch_compile()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a6a013051b2cff9ab317f09ff99efc4ce), [ch_compile_multi()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a42e42ceae9e88e63bddfd66b80ea8ee6).

**Return**

[CH_SUCCESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga2ac91c9a68ace90cec58294dde94bc67) on success, other values on failure.

**Parameters**

- `error`: The [ch_compile_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a8a5035f7767f59b039981a37ce8572da) to be freed. NULL may also be safely provided.

*struct* `ch_compile_error`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_compile_error "Permalink to this definition")

*#include <ch_compile.h>*

A type containing error details that is returned by the compile calls ([ch_compile()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a6a013051b2cff9ab317f09ff99efc4ce) and [ch_compile_multi()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a42e42ceae9e88e63bddfd66b80ea8ee6) on failure. The caller may inspect the values returned in this type to determine the cause of failure.

Public Members

char* `message`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_compile_error::message "Permalink to this definition")

A human-readable error message describing the error.

int `expression`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_compile_error::expression "Permalink to this definition")

The zero-based number of the expression that caused the error (if
 this can be determined). If the error is not specific to an expression,
 then this value will be less than zero.

### File: ch_runtime.h

The Chimera runtime API definition.

Chimera is a hybrid of Hyperscan and PCRE regular expression engine.

This header contains functions for using compiled Chimera databases for scanning data at runtime.

Defines

`CH_CALLBACK_CONTINUE`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_CALLBACK_CONTINUE "Permalink to this definition")

Continue matching.

`CH_CALLBACK_TERMINATE`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_CALLBACK_TERMINATE "Permalink to this definition")

Terminate matching.

`CH_CALLBACK_SKIP_PATTERN`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_CALLBACK_SKIP_PATTERN "Permalink to this definition")

Skip remaining matches for this ID and continue.

`CH_ERROR_MATCHLIMIT`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_ERROR_MATCHLIMIT "Permalink to this definition")

PCRE hits its match limit and reports PCRE_ERROR_MATCHLIMIT.

`CH_ERROR_RECURSIONLIMIT`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_ERROR_RECURSIONLIMIT "Permalink to this definition")

PCRE hits its recursion limit and reports PCRE_ERROR_RECURSIONLIMIT.

`CH_CAPTURE_FLAG_INACTIVE`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_CAPTURE_FLAG_INACTIVE "Permalink to this definition")

Flag indicating that a particular capture group is inactive, used in [ch_capture_t::flags](http://intel.github.io/hyperscan/dev-reference/chimera.html#structch__capture_1a61a703e15c96a4e2af0e1773fdfe546d).

`CH_CAPTURE_FLAG_ACTIVE`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.CH_CAPTURE_FLAG_ACTIVE "Permalink to this definition")

Flag indicating that a particular capture group is active, used in [ch_capture_t::flags](http://intel.github.io/hyperscan/dev-reference/chimera.html#structch__capture_1a61a703e15c96a4e2af0e1773fdfe546d).

Typedefs

*typedef* struct ch_scratch `ch_scratch_t`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_scratch_t "Permalink to this definition")

A Chimera scratch space.

*typedef* int `ch_callback_t`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_callback_t "Permalink to this definition")

Callback return value used to tell the Chimera matcher what to do after processing this match.

*typedef* int `ch_error_event_t`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_event_t "Permalink to this definition")

Type used to differentiate the errors raised with the [ch_error_event_handler](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1ac9af00c32b98ae018cd0fa8f67fbe16c) callback.

*typedef* struct [ch_capture](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_capture "ch_capture") `ch_capture_t`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_capture_t "Permalink to this definition")

Structure representing a captured subexpression within a match. 
An array of these structures corresponding to capture groups in order is
 passed to the callback on match, with active structures identified by 
the CH_CAPTURE_FLAG_ACTIVE flag.

*typedef* [ch_callback_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_callback_t "ch_callback_t")`( * ch_match_event_handler)`(unsigned int *id*, unsigned long long *from*, unsigned long long *to*, unsigned int *flags*, unsigned int *size*, const [ch_capture_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_capture_t "ch_capture_t") **captured*, void **ctx*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_match_event_handler "Permalink to this definition")

Definition of the match event callback function type.

A callback function matching the defined type must be provided by the application calling the [ch_scan()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a2a297a612f301abfb5042b680381df6e)

This callback function will be invoked whenever a match is located in
 the target data during the execution of a scan. The details of the 
match are passed in as parameters to the callback function, and the 
callback function should return a value indicating whether or not 
matching should continue on the target data. If no callbacks are desired
 from a scan call, NULL may be provided in order to suppress match 
production.

**Return**

The callback can return [CH_CALLBACK_TERMINATE](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__CALLBACK_1ga0078f0e41041a72b729bb188c45f8350) to stop matching. Otherwise, a return value of [CH_CALLBACK_CONTINUE](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__CALLBACK_1ga279a7c4abe841baabecfaabc7c2ff9ee) will continue, with the current pattern if configured to produce multiple matches per pattern, while a return value of [CH_CALLBACK_SKIP_PATTERN](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__CALLBACK_1ga398fe37f7470d895a57b1206e4246c4b) will cease matching this pattern but continue matching the next pattern.

**Parameters**

- `id`: The ID number of the expression that matched. If the expression was a single expression compiled with [ch_compile()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a6a013051b2cff9ab317f09ff99efc4ce), this value will be zero.

- `from`: The offset of the first byte that matches the expression.

- `to`: The offset after the last byte that matches the expression.

- `flags`: This is provided for future use and is unused at present.

- `size`: The number of valid entries pointed to by the captured parameter.

- `captured`: A pointer to an array of [ch_capture_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a472f261071c00e9710939866e0001f07) structures that contain the start and end offsets of entire pattern match and each captured subexpression.

- `ctx`: The pointer supplied by the user to the [ch_scan()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a2a297a612f301abfb5042b680381df6e) function.

*typedef* [ch_callback_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_callback_t "ch_callback_t")`( * ch_error_event_handler)`([ch_error_event_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_event_t "ch_error_event_t") *error_type*, unsigned int *id*, void **info*, void **ctx*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_event_handler "Permalink to this definition")

Definition of the Chimera error event callback function type.

A callback function matching the defined type may be provided by the application calling the [ch_scan](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a2a297a612f301abfb5042b680381df6e) function. This callback function will be invoked when an error event 
occurs during matching; this indicates that some matches for a given 
expression may not be reported.

**Return**

The callback can return [CH_CALLBACK_SKIP_PATTERN](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__CALLBACK_1ga398fe37f7470d895a57b1206e4246c4b) to cease matching this pattern but continue matching the next pattern. Otherwise, we stop matching for all patterns with [CH_CALLBACK_TERMINATE](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__CALLBACK_1ga0078f0e41041a72b729bb188c45f8350).

**Parameters**

- `error_type`: The type of error event that occurred. Currently these errors correspond to resource limits on PCRE backtracking [CH_ERROR_MATCHLIMIT](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR__EVENT_1ga7cdec5f63af4becb36267fd092eb38a5) and [CH_ERROR_RECURSIONLIMIT](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR__EVENT_1ga5208dff2cf2886273c7075f2ea4ec056).

- `id`: The ID number of the expression that matched.

- `info`: Event-specific data, for future use. Currently unused.

- `ctx`: The context pointer supplied by the user to the [ch_scan](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a2a297a612f301abfb5042b680381df6e) function.

Functions

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_scan`(const [ch_database_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_database_t "ch_database_t") * *db*, const char * *data*, unsigned int *length*, unsigned int *flags*, [ch_scratch_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_scratch_t "ch_scratch_t") * *scratch*, [ch_match_event_handler](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_match_event_handler "ch_match_event_handler") *onEvent*, [ch_error_event_handler](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_event_handler "ch_error_event_handler") *onError*, void * *context*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_scan "Permalink to this definition")

The block regular expression scanner.

This is the function call in which the actual pattern matching takes place for block-mode pattern databases.

**Return**

Returns [CH_SUCCESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga2ac91c9a68ace90cec58294dde94bc67) on success; [CH_SCAN_TERMINATED](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga97fa0c8edae135770162a30d95f4d827) if the match callback indicated that scanning should stop; other values on error.

**Parameters**

- `db`: A compiled pattern database.

- `data`: Pointer to the data to be scanned.

- `length`: The number of bytes to scan.

- `flags`: Flags modifying the behaviour of this function. This parameter is provided for future use and is unused at present.

- `scratch`: A per-thread scratch space allocated by [ch_alloc_scratch()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a3c7d88639ce32646f9aaf5b57b2d0b1d) for this database.

- `onEvent`: Pointer to a match event callback function. If a NULL pointer is given, no matches will be returned.

- `onError`: Pointer to a error event callback function. If a NULL pointer is given, [CH_ERROR_MATCHLIMIT](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR__EVENT_1ga7cdec5f63af4becb36267fd092eb38a5) and [CH_ERROR_RECURSIONLIMIT](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR__EVENT_1ga5208dff2cf2886273c7075f2ea4ec056) errors will be ignored and match will continue.

- `context`: The user defined pointer which will be passed to the callback function.

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_alloc_scratch`(const [ch_database_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_database_t "ch_database_t") * *db*, [ch_scratch_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_scratch_t "ch_scratch_t") ** *scratch*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_alloc_scratch "Permalink to this definition")

Allocate a “scratch” space for use by Chimera.

This is required for runtime use, and one scratch space per thread, 
or concurrent caller, is required. Any allocator callback set by [ch_set_scratch_allocator()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a1a1378a43cfb81f8628a3452f5cc2fce) or [ch_set_allocator()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a06b7bdbcc2e56c57771271025e33b93b) will be used by this function.

**Return**

[CH_SUCCESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga2ac91c9a68ace90cec58294dde94bc67) on successful allocation; [CH_NOMEM](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga3950bacd58097c01ad05091e07e47318) if the allocation fails. Other errors may be returned if invalid parameters are specified.

**Parameters**

- `db`: The database, as produced by [ch_compile()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__compile_8h_1a6a013051b2cff9ab317f09ff99efc4ce).

- `scratch`:
   On first allocation, a pointer to NULL should be provided so a new 
  scratch can be allocated. If a scratch block has been previously 
  allocated, then a pointer to it should be passed back in to see if it is
   valid for this database block. If a new scratch block is required, the 
  original will be freed and the new one returned, otherwise the previous 
  scratch block will be returned. On success, the scratch block will be 
  suitable for use with the provided database in addition to any databases
   that original scratch space was suitable for.

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_clone_scratch`(const [ch_scratch_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_scratch_t "ch_scratch_t") * *src*, [ch_scratch_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_scratch_t "ch_scratch_t") ** *dest*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_clone_scratch "Permalink to this definition")

Allocate a scratch space that is a clone of an existing scratch space.

This is useful when multiple concurrent threads will be using the 
same set of compiled databases, and another scratch space is required. 
Any allocator callback set by [ch_set_scratch_allocator()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a1a1378a43cfb81f8628a3452f5cc2fce) or [ch_set_allocator()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a06b7bdbcc2e56c57771271025e33b93b) will be used by this function.

**Return**

[CH_SUCCESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga2ac91c9a68ace90cec58294dde94bc67) on success; [CH_NOMEM](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga3950bacd58097c01ad05091e07e47318) if the allocation fails. Other errors may be returned if invalid parameters are specified.

**Parameters**

- `src`: The existing [ch_scratch_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a4d106e0306f1d4b802d1929365893207) to be cloned.

- `dest`: A pointer to the new scratch space will be returned here.

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_scratch_size`(const [ch_scratch_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_scratch_t "ch_scratch_t") * *scratch*, size_t * *scratch_size*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_scratch_size "Permalink to this definition")

Provides the size of the given scratch space.

**Return**

[CH_SUCCESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga2ac91c9a68ace90cec58294dde94bc67) on success, other values on failure.

**Parameters**

- `scratch`: A per-thread scratch space allocated by [ch_alloc_scratch()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a3c7d88639ce32646f9aaf5b57b2d0b1d) or [ch_clone_scratch()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a15e83801bb74dfff8c4d8623f64f967c).

- `scratch_size`: On success, the size of the scratch space in bytes is placed in this parameter.

[ch_error_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_error_t "ch_error_t") `ch_free_scratch`([ch_scratch_t](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_scratch_t "ch_scratch_t") * *scratch*)[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_free_scratch "Permalink to this definition")

Free a scratch block previously allocated by [ch_alloc_scratch()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a3c7d88639ce32646f9aaf5b57b2d0b1d) or [ch_clone_scratch()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__runtime_8h_1a15e83801bb74dfff8c4d8623f64f967c).

The free callback set by [ch_set_scratch_allocator()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a1a1378a43cfb81f8628a3452f5cc2fce) or [ch_set_allocator()](http://intel.github.io/hyperscan/dev-reference/chimera.html#ch__common_8h_1a06b7bdbcc2e56c57771271025e33b93b) will be used by this function.

**Return**

[CH_SUCCESS](http://intel.github.io/hyperscan/dev-reference/chimera.html#group__CH__ERROR_1ga2ac91c9a68ace90cec58294dde94bc67) on success, other values on failure.

**Parameters**

- `scratch`: The scratch block to be freed. NULL may also be safely provided.

*struct* `ch_capture`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_capture "Permalink to this definition")

*#include <ch_runtime.h>*

Structure representing a 
captured subexpression within a match. An array of these structures 
corresponding to capture groups in order is passed to the callback on 
match, with active structures identified by the CH_CAPTURE_FLAG_ACTIVE 
flag.

Public Members

unsigned int `flags`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_capture::flags "Permalink to this definition")

The flags indicating if this structure is active.

unsigned long long `from`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_capture::from "Permalink to this definition")

offset at which this capture group begins.

unsigned long long `to`[](http://intel.github.io/hyperscan/dev-reference/chimera.html#c.ch_capture::to "Permalink to this definition")

offset at which this capture group ends.
