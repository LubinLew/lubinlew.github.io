# 模式扫描(Scanning for Patterns)

Hyperscan 提供了三种不同的扫描模式。每种扫描模式都有各自的扫描函数。

## 处理扫描命中

三种模式的扫描命中处理都是由用户自定义的回调函数处理。

```c
/** 扫描命中的处理函数定义
 *  这个类型的callback是向 hs_scan(), hs_scan_vector(), hs_scan_stream() 这三个函数注册的
 * This callback function will be invoked whenever a match is located in the
 * target data during the execution of a scan. The details of the match are
 * passed in as parameters to the callback function, and the callback function
 * should return a value indicating whether or not matching should continue on
 * the target data. If no callbacks are desired from a scan call, NULL may be
 * provided in order to suppress match production.
 *
 * 回调函数中,不能再调用其他 Hyperscan API functions on
 * the same stream nor should it attempt to reuse the scratch space allocated
 * for the API calls that caused it to be triggered. Making another call to the
 * Hyperscan library with completely independent parameters should work (for
 * example, scanning a different database in a new stream and with new scratch
 * space), but reusing data structures like stream state and/or scratch space
 * will produce undefined behavior.
 *
 * @param from
 *      - If a start of match flag is enabled for the current pattern, this
 *        argument will be set to the start of match for the pattern assuming
 *        that that start of match value lies within the current 'start of match
 *        horizon' chosen by one of the SOM_HORIZON mode flags.

 *      - If the start of match value lies outside this horizon (possible only
 *        when the SOM_HORIZON value is not @ref HS_MODE_SOM_HORIZON_LARGE),
 *        the @p from value will be set to @ref HS_OFFSET_PAST_HORIZON.

 *      - This argument will be set to zero if the Start of Match flag is not
 *        enabled for the given pattern.
 *
 * @return
 *      Non-zero if the matching should cease, else zero. If scanning is
 *      performed in streaming mode and a non-zero value is returned, any
 *      subsequent calls to @ref hs_scan_stream() for that stream will
 *      immediately return with @ref HS_SCAN_TERMINATED.
 */
typedef int (*match_event_handler)(
    unsigned int id,          /* 命中pattern的编号 */
    unsigned long long from,
    unsigned long long to,
    unsigned int flags,       /* 未使用 */
    void *context);           /* 注册callback时,用户指定的参数 */
```

The *id* argument will be set to the identifier for the matching expression
provided at compile time, and the *to* argument will be set to the end-offset
of the match. If SOM was requested for the pattern (see [Start of Match](http://intel.github.io/hyperscan/dev-reference/compilation.html#som)), the *from* argument will be set to the leftmost possible start-offset for the match.

The match callback function has the capability to halt scanning by returning a non-zero value.

### 流模式(Streaming Mode)

The core of the Hyperscan streaming runtime API consists of functions to open,
scan, and close Hyperscan data streams:

- [`hs_open_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_open_stream "hs_open_stream"): allocates and initializes a new stream for scanning.

- [`hs_scan_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_scan_stream "hs_scan_stream"): scans a block of data in a given stream, raising
  matches as they are detected.

- [`hs_close_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_close_stream "hs_close_stream"): completes scanning of a given stream (raising any
  matches that occur at the end of the stream) and frees the stream state. After
  a call to [`hs_close_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_close_stream "hs_close_stream"), the stream handle is invalid and should
  not be used again for any purpose.

Any matches detected in the data as it is scanned are returned to the calling
application via a function pointer callback.

The match callback function has the capability to halt scanning of the current
data stream by returning a non-zero value. In streaming mode, the result of
this is that the stream is then left in a state where no more data can be
scanned, and any subsequent calls to [`hs_scan_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_scan_stream "hs_scan_stream") for that stream
will return immediately with [`HS_SCAN_TERMINATED`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_SCAN_TERMINATED "HS_SCAN_TERMINATED"). The caller must
still call [`hs_close_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_close_stream "hs_close_stream") to complete the clean-up process for that
stream.

Streams exist in the Hyperscan library so that pattern matching state can be
maintained across multiple blocks of target data – without maintaining this
state, it would not be possible to detect patterns that span these blocks of
data. This, however, does come at the cost of requiring an amount of storage
per-stream (the size of this storage is fixed at compile time), and a slight
performance penalty in some cases to manage the state.

While Hyperscan does always support a strict ordering of multiple matches,
streaming matches will not be delivered at offsets before the current stream
write, with the exception of zero-width asserts, where constructs such as `\b` and `$` can cause a match on the final character of a
stream write to be delayed until the next stream write or stream close
operation.

#### 流管理(Stream Management)

In addition to [`hs_open_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_open_stream "hs_open_stream"), [`hs_scan_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_scan_stream "hs_scan_stream"), and [`hs_close_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_close_stream "hs_close_stream"), the Hyperscan API provides a number of other
functions for the management of streams:

- [`hs_reset_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_reset_stream "hs_reset_stream"): resets a stream to its initial state; this is
  equivalent to calling [`hs_close_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_close_stream "hs_close_stream") but will not free the memory
  used for stream state.

- [`hs_copy_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_copy_stream "hs_copy_stream"): constructs a (newly allocated) duplicate of a
  stream.

- [`hs_reset_and_copy_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_reset_and_copy_stream "hs_reset_and_copy_stream"): constructs a duplicate of a stream into
  another, resetting the destination stream first. This call avoids the
  allocation done by [`hs_copy_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_copy_stream "hs_copy_stream").

#### 流压缩(Stream Compression)

A stream object is allocated as a fixed size region of memory which has been
sized to ensure that no memory allocations are required during scan
operations. When the system is under memory pressure, it may be useful to reduce
the memory consumed by streams that are not expected to be used soon. The
Hyperscan API provides calls for translating a stream to and from a compressed
representation for this purpose. The compressed representation differs from the
full stream object as it does not reserve space for components which are not
required given the current stream state. The Hyperscan API functions for this
functionality are:

- [`hs_compress_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compress_stream "hs_compress_stream"): fills the provided buffer with a compressed
  representation of the stream and returns the number of bytes consumed by the
  compressed representation. If the buffer is not large enough to hold the
  compressed representation, [`HS_INSUFFICIENT_SPACE`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_INSUFFICIENT_SPACE "HS_INSUFFICIENT_SPACE") is returned along
  with the required size. This call does not modify the original stream in any
  way: it may still be written to with [`hs_scan_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_scan_stream "hs_scan_stream"), used as part of
  the various reset calls to reinitialise its state, or [`hs_close_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_close_stream "hs_close_stream") may be called to free its resources.

- [`hs_expand_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_expand_stream "hs_expand_stream"): creates a new stream based on a buffer containing
  a compressed representation.

- [`hs_reset_and_expand_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_reset_and_expand_stream "hs_reset_and_expand_stream"): constructs a stream based on a buffer
  containing a compressed representation on top of an existing stream, resetting
  the existing stream first. This call avoids the allocation done by [`hs_expand_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_expand_stream "hs_expand_stream").

Note: it is not recommended to use stream compression between every call to scan
for performance reasons as it takes time to convert between the compressed
representation and a standard stream.

### 块模式(Block Mode)

The block mode runtime API consists of a single function: [`hs_scan()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_scan "hs_scan"). Using
the compiled patterns this function identifies matches in the target data,
using a function pointer callback to communicate with the application.

This single [`hs_scan()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_scan "hs_scan") function is essentially equivalent to calling [`hs_open_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_open_stream "hs_open_stream"), making a single call to [`hs_scan_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_scan_stream "hs_scan_stream"), and
then [`hs_close_stream()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_close_stream "hs_close_stream"), except that block mode operation does not
incur all the stream related overhead.

### 向量模式(Vectored Mode)

The vectored mode runtime API, like the block mode API, consists of a single
function: [`hs_scan_vector()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_scan_vector "hs_scan_vector"). This function accepts an array of data
pointers and lengths, facilitating the scanning in sequence of a set of data
blocks that are not contiguous in memory.

From the caller’s perspective, this mode will produce the same matches as if
the set of data blocks were (a) scanned in sequence with a series of streaming
mode scans, or (b) copied in sequence into a single block of memory and then
scanned in block mode.

### Scratch Space

当扫描数据时，Hyperscan需要少量临时内存来存储即时数据。很不幸的是这个数据量对于栈来说还是太大，尤其是嵌入式系统。并且动态申请内存的操作消耗, 所以必须预申请一个 “scratch” 空间提供给扫描函数。

函数 [`hs_alloc_scratch()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_alloc_scratch "hs_alloc_scratch") 申请一个给定数据库的 scratch space。如果一个应用使用多个数据库，只需要使用一个 scratch region，但是需要保证空间的大小能够扫描所有给定的数据库。这种情况侠, 调用 `hs_alloc_scratch()` on each database (with the same `scratch` pointer)
will ensure that the scratch space is large enough to support scanning against
any of the given databases.

While the Hyperscan library is re-entrant, the use of scratch spaces is not.
例如，如果设计有递归或者嵌套的需求，那么还需要一个额外的scratch
space  (say, from the match callback function), then an additional scratch
space is required for that context.

In the absence of recursive scanning, only one such space is required per thread
and can (and indeed should) be allocated before data scanning is to commence.

In a scenario where a set of expressions are compiled by a single “master”
thread and data will be scanned by multiple “worker” threads, the convenience
function [`hs_clone_scratch()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_clone_scratch "hs_clone_scratch") allows multiple copies of an existing
scratch space to be made for each thread (rather than forcing the caller to pass
all the compiled databases through [`hs_alloc_scratch()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_alloc_scratch "hs_alloc_scratch") multiple times).

For example:

```c
hs_error_t err;
hs_scratch_t *scratch_prototype = NULL;
err = hs_alloc_scratch(db, &scratch_prototype);
if (err != HS_SUCCESS) {
    printf("hs_alloc_scratch failed!");
    exit(1);
}

hs_scratch_t *scratch_thread1 = NULL;
hs_scratch_t *scratch_thread2 = NULL;

err = hs_clone_scratch(scratch_prototype, &scratch_thread1);
if (err != HS_SUCCESS) {
    printf("hs_clone_scratch failed!");
    exit(1);
}
err = hs_clone_scratch(scratch_prototype, &scratch_thread2);
if (err != HS_SUCCESS) {
    printf("hs_clone_scratch failed!");
    exit(1);
}

hs_free_scratch(scratch_prototype);

/* 现在2个线程可以同时进行扫描， 每个扫描都有自已的 scratch space */
```

### 用户自定义内存申请

默认情况了，Hyperscan在运行时申请内存(scratch space, stream state 等) 都是使用系统默认的API，通常是 `malloc()` 和`free()`函数。Hyperscan提供了API支持用户自义定申请内存。

```c
/* 申请内存函数原型定义 */
typedef void *(HS_CDECL *hs_alloc_t)(size_t size);

/* 释放内存函数原型定义 */
typedef void  (HS_CDECL *hs_free_t)(void *ptr);

/* 通用内存 申请/释放 注册接口 */
hs_error_t HS_CDECL hs_set_allocator(
        hs_alloc_t alloc_func,
        hs_free_t free_func);

/* 编译数据库时 申请/释放 注册接口 */
hs_error_t HS_CDECL hs_set_database_allocator(
        hs_alloc_t alloc_func,
        hs_free_t free_func);

/* scratch space 申请/释放 注册接口 */
hs_error_t HS_CDECL hs_set_scratch_allocator(
        hs_alloc_t alloc_func,
        hs_free_t free_func);

/* * Set the allocate and free functions used by Hyperscan for allocating memory
 * for items returned by the Hyperscan API such as @ref hs_compile_error_t, @ref
 * hs_expr_info_t and serialized databases.*/
hs_error_t HS_CDECL hs_set_misc_allocator(
        hs_alloc_t alloc_func,
        hs_free_t free_func);


```
