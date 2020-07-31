# 模式编译(Compiling Patterns)

## 构建数据库(Building a Database)

Hyperscan compiler API 接收正则表达式并将其转换为编译过的模式数据库，这个数据库是用于扫描数据的。Hyperscan 提供了3个函数用于编译模式数据库：

```c
/* hs_compile.h */

hs_error_t HS_CDECL hs_compile(
        const char *expression, 
        unsigned int flags,
        unsigned int mode,
        const hs_platform_info_t *platform,
        hs_database_t **db, 
        hs_compile_error_t **error);


hs_error_t HS_CDECL hs_compile_multi(
        const char *const *expressions,
        const unsigned int *flags,
        const unsigned int *ids,
        unsigned int elements, 
        unsigned int mode,
        const hs_platform_info_t *platform,
        hs_database_t **db,
        hs_compile_error_t **error);


hs_error_t HS_CDECL hs_compile_ext_multi(
        const char *const *expressions,
        const unsigned int *flags,
        const unsigned int *ids,
        const hs_expr_ext_t *const *ext,
        unsigned int elements, 
        unsigned int mode,
        const hs_platform_info_t *platform,
        hs_database_t **db, 
        hs_compile_error_t **error);
```

1. [`hs_compile()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile "hs_compile"): 编译单个正则表达式到模式数据库中。

2. [`hs_compile_multi()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile_multi "hs_compile_multi"): 编译一组正则表达式到模式数据库中。所有的正则表达式在扫描的时候是并发式的多模匹配，当扫描命中时，会返回这条正则表达式的ID(这个ID是用户提供的)。

3. [`hs_compile_ext_multi()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile_ext_multi "hs_compile_ext_multi"): 类似上面的API，编译一组正则表达式，但是允许为每一个正则表达式声明[扩展参数(Extended Parameters)](http://intel.github.io/hyperscan/dev-reference/compilation.html#extparam) 。

编译的作用是允许 Hyperscan库分析给定的正则式并且决定怎样更优化的扫描这些patterns，这些操作如果放到运行时则需要非常高的计算成本。

当编译表达式时, 需要决定用什么模式进行扫描, 模式分为 流模式 块模式 和 向量模式:

- **流模式(Streaming mode)**: 需要扫描的目标数据是一个持续的数据流,并不能一次就能获取到全部的数据;

- **块模式(Block mode)**: 目标数据是分离的数据块, 但是可以一次获取全部的数据, 不需要保存状态.

- **向量模式(Vectored mode)**: 目标数据由一系列不连续的快组成.但是可以一次获取全部的数据,不需要保存状态.

模式是通过参数 `mode` 声明 [`hs_compile()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile "hs_compile") must be set to [`HS_MODE_STREAM`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_MODE_STREAM "HS_MODE_STREAM"); similarly,
block mode requires the use of [`HS_MODE_BLOCK`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_MODE_BLOCK "HS_MODE_BLOCK") and vectored mode
requires the use of [`HS_MODE_VECTORED`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_MODE_VECTORED "HS_MODE_VECTORED"). A pattern database compiled
for one mode (streaming, block or vectored) can only be used in that mode. The
version of Hyperscan used to produce a compiled pattern database must match the
version of Hyperscan used to scan with it.

```c
/* hs_compile.h */

/* Compiler mode flag: Block scan (non-streaming) database.*/
#define HS_MODE_BLOCK           1
/* Compiler mode flag: Streaming database.*/
#define HS_MODE_STREAM          2
/* Compiler mode flag: Vectored scanning database.*/
#define HS_MODE_VECTORED        4
```

Hyperscan provides support for targeting a database at a particular CPU
platform; see [Instruction Set Specialization](http://intel.github.io/hyperscan/dev-reference/compilation.html#instr-specialization) for details.

### 编译纯字符串(Compile Pure Literals)

纯字符串是一种特殊的正则表达式. A character sequence is
regarded as a pure literal if and only if each character is read and
interpreted independently. No syntax association happens between any adjacent
characters.

For example, given an expression written as `/bc?/`. We could say it is
a regluar expression, with the meaning that character `b` followed by nothing
or by one character `c`. On the other view, we could also say it is a pure
literal expression, with the meaning that this is a character sequence of 3-byte
length, containing characters `b`, `c` and `?`. In regular case, the
question mark character `?` has a particular syntax role called 0-1 quantifier,
which has an syntax association with the character ahead of it. Similar
characters exist in regular grammer like `[`, `]`, `(`, `)`, `{`, `}`, `-`, `*`, `+`, `\`, `|`, `/`, `:`, `^`, `.`, `$`.
While in pure literal case, all these meta characters lost extra meanings
expect for that they are just common ASCII codes.

Hyperscan is initially designed to process common regular expressions. It is
hence embedded with a complex parser to do comprehensive regular grammer
interpretion. Particularly, the identification of above meta characters is the
basic step for the interpretion of far more complex regular grammers.

However in real cases, patterns may not always be regular expressions. They
could just be pure literals. Problem will come if the pure literals contain
regular meta characters. Supposing fed directly into traditional Hyperscan
compile API, all these meta characters will be interpreted in predefined ways,
which is unnecessary and the result is totally out of expectation. To avoid
such misunderstanding by traditional API, users have to preprocess these
literal patterns by converting the meta characters into some other formats:
either by adding a backslash `\` before certain meta characters, or by
converting all the characters into a hexadecimal representation.

在`v5.2.0`版本中, 引入了2个新的API用来编译纯字符串模式:

```c
/* hs_compile.h */

hs_error_t HS_CDECL hs_compile_lit(
        const char *expression, unsigned flags,
        const size_t len, unsigned mode,
        const hs_platform_info_t *platform,
        hs_database_t **db,
        hs_compile_error_t **error)

hs_error_t HS_CDECL hs_compile_lit_multi(
        const char * const *expressions,
        const unsigned *flags,
        const unsigned *ids,
        const size_t *lens,
        unsigned elements, 
        unsigned mode,
        const hs_platform_info_t *platform,
        hs_database_t **db,
        hs_compile_error_t **error);
```

1. [`hs_compile_lit()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile_lit "hs_compile_lit"): compiles a single pure literal into a pattern
   database.

2. [`hs_compile_lit_multi()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile_lit_multi "hs_compile_lit_multi"): compiles an array of pure literals into a
   pattern database. All of the supplied patterns will be scanned for
   concurrently at scan time, with user-supplied identifiers returned when they
   match.

These 2 APIs are designed for use cases where all patterns contained in the
target rule set are pure literals. Users can pass the initial pure literal
content directly into these APIs without worrying about writing regular meta
characters in their patterns. No preprocessing work is needed any more.

For new APIs, the `length` of each literal pattern is a newly added parameter.
Hyperscan needs to locate the end position of the input expression via clearly
knowing each literal’s length, not by simply identifying character `\0` of a
string.

Supported flags: [`HS_FLAG_CASELESS`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_CASELESS "HS_FLAG_CASELESS"), [`HS_FLAG_SINGLEMATCH`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_SINGLEMATCH "HS_FLAG_SINGLEMATCH"), [`HS_FLAG_SOM_LEFTMOST`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_SOM_LEFTMOST "HS_FLAG_SOM_LEFTMOST").

Note

字符串编译API不支持扩展参数 [Extended Parameters](http://intel.github.io/hyperscan/dev-reference/compilation.html#extparam). And
for runtime implementation, traditional runtime APIs can still be
used to match pure literal patterns.

Note

If the target rule set contains at least one regular expression,
please use traditional compile APIs [`hs_compile()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile "hs_compile"), [`hs_compile_multi()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile_multi "hs_compile_multi") and [`hs_compile_ext_multi()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile_ext_multi "hs_compile_ext_multi").
The new literal APIs introduced here are designed for rule sets
containing only pure literal expressions.

## Pattern Support

Hyperscan 支持PCRE库的大部分语法, 这些语法可以在 <[PCRE - Perl Compatible Regular Expressions](http://www.pcre.org/)> 中找到, 有一些语法是不支持的,这会导致编译错误 .

The version of PCRE used to validate Hyperscan’s interpretation of this syntax
is 8.41 or above.

### Supported Constructs

The following regex constructs are supported by Hyperscan:

- Literal characters and strings, with all libpcre quoting and character
  escapes.

- Character classes such as `.` (dot), `[abc]`, and `[^abc]`, as well as the predefined character classes `\s`, `\d`, `\w`, `\v`, and `\h` and their
  negated counterparts (`\S`, `\D`, `\W`, `\V`, and `\H`).

- The POSIX named character classes `[[:xxx:]]` and negated named
  character classes `[[:^xxx:]]`.

- Unicode character properties, such as `\p{L}`, `\P{Sc}`, `\p{Greek}`.

- Quantifiers:
  
  - Quantifiers such as `?`, `*` and `+` are supported
    when applied to arbitrary supported sub-expressions.
  
  - Bounded repeat qualifiers such as `{n}`, `{m,n}`, `{n,}` are supported with limitations.
    
    - For arbitrary repeated sub-patterns: *n* and *m* should be either small
      or infinite, e.g. `(a|b}{4}`, `(ab?c?d){4,10}` or `(ab(cd)*){6,}`.
    
    - For single-character width sub-patterns such as `[^\a]` or `.` or `x`, nearly all repeat counts are supported, except
      where repeats are extremely large (maximum bound greater than 32767).
      Stream states may be very large for large bounded repeats, e.g. `a.{2000}b`. Note: such sub-patterns may be considerably
      cheaper if at the beginning or end of patterns and especially if the [`HS_FLAG_SINGLEMATCH`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_SINGLEMATCH "HS_FLAG_SINGLEMATCH") flag is on for that pattern.
  
  - Lazy modifiers (`?` appended to another quantifier, e.g. `\w+?`) are supported but ignored (as Hyperscan reports all
    matches).

- Parenthesization, including the named and unnamed capturing and
  non-capturing forms. However, capturing is ignored.

- Alternation with the `|` symbol, as in `foo|bar`.

- The anchors `^`, `$`, `\A`, `\Z` and `\z`.

- Option modifiers:
  
  These allow behaviour to be switched on (with `(?<option>)`) and off
  (with `(?-<option>)`) for a sub-pattern. The supported options are:
  
  > - `i`: Case-insensitive matching, as per [`HS_FLAG_CASELESS`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_CASELESS "HS_FLAG_CASELESS").
  > 
  > - `m`: Multi-line matching, as per [`HS_FLAG_MULTILINE`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_MULTILINE "HS_FLAG_MULTILINE").
  > 
  > - `s`: Interpret `.` as “any character”, as per [`HS_FLAG_DOTALL`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_DOTALL "HS_FLAG_DOTALL").
  > 
  > - `x`: Extended syntax, which will ignore most whitespace in the
  >   pattern for compatibility with libpcre’s `PCRE_EXTENDED` option.
  
  For example, the expression `foo(?i)bar(?-i)baz` will switch on
  case-insensitive matching *only* for the `bar` portion of the match.

- The `\b` and `\B` zero-width assertions (word boundary and
  ‘not word boundary’, respectively).

- Comments in `(?# comment)` syntax.

- The `(*UTF8)` and `(*UCP)` control verbs at the beginning of a
  pattern, used to enable UTF-8 and UCP mode.

Note

Bounded-repeat quantifiers with large repeat counts of arbitrary
expressions (e.g. `([a-z]|bc*d|xy?z){1000,5000}`) will result in a
“Pattern too large” error at pattern compile time.

Note

At this time, not all patterns can be successfully compiled with the [`HS_FLAG_SOM_LEFTMOST`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_SOM_LEFTMOST "HS_FLAG_SOM_LEFTMOST") flag, which enables per-pattern support for [Start of Match](http://intel.github.io/hyperscan/dev-reference/compilation.html#som). The patterns that support this flag are a subset of patterns that
can be successfully compiled with Hyperscan; notably, many bounded repeat
forms that can be compiled with Hyperscan without the Start of Match flag
enabled cannot be compiled with the flag enabled.

### Unsupported Constructs

The following regex constructs are not supported by Hyperscan:

- Backreferences and capturing sub-expressions.

- Arbitrary zero-width assertions.

- Subroutine references and recursive patterns.

- Conditional patterns.

- Backtracking control verbs.

- The `\C` “single-byte” directive (which breaks UTF-8 sequences).

- The `\R` newline match.

- The `\K` start of match reset directive.

- Callouts and embedded code.

- Atomic grouping and possessive quantifiers.

## 语义学(Semantics)

While Hyperscan follows libpcre syntax, it provides different semantics. The
major departures from libpcre semantics are motivated by the requirements of
streaming and multiple simultaneous pattern matching.

The major departures from libpcre semantics are:

1. **Multiple pattern matching**: Hyperscan allows matches to be reported for
   several patterns simultaneously. This is not equivalent to separating the
   patterns by `|` in libpcre, which evaluates alternations
   left-to-right.

2. **Lack of ordering**: the multiple matches that Hyperscan produces are not
   guaranteed to be ordered, although they will always fall within the bounds of
   the current scan.

3. **End offsets only**: Hyperscan’s default behaviour is only to report the end
   offset of a match. Reporting of the start offset can be enabled with
   per-expression flags at pattern compile time. See [Start of Match](http://intel.github.io/hyperscan/dev-reference/compilation.html#som) for details.

4. **“All matches” reported**: scanning `/foo.*bar/` against `fooxyzbarbar` will return two matches from Hyperscan – at the points
   corresponding to the ends of `fooxyzbar` and `fooxyzbarbar`. In contrast,
   libpcre semantics by default would report only one match at `fooxyzbarbar` (greedy semantics) or, if non-greedy semantics were switched on, one match at `fooxyzbar`. This means that switching between greedy and non-greedy
   semantics is a no-op in Hyperscan.

To support libpcre quantifier semantics while accurately reporting streaming
matches at the time they occur is impossible. For example, consider the pattern
above, `/foo.*bar/`, in streaming mode, against the following
stream (three blocks scanned in sequence):

> | block 1     | block 2 | block 3 |
> | ----------- | ------- | ------- |
> | `fooxyzbar` | `baz`   | `qbar`  |

Since the `.*` repeat in the pattern is a *greedy* repeat in libpcre, it
must match as much as possible without causing the rest of the pattern to fail.
However, in streaming mode, this would require knowledge of data in the stream
beyond the current block being scanned.

In this example, the match at offset 9 in the first block is only the correct
match (under libpcre semantics) if there is no `bar` in a subsequent block –
as in block 3 – which would constitute a better match for the pattern.

### 开始匹配(Start of Match)

In standard operation, Hyperscan will only provide the end offset of a match
when the match callback is called. If the [`HS_FLAG_SOM_LEFTMOST`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_SOM_LEFTMOST "HS_FLAG_SOM_LEFTMOST") flag
is specified for a particular pattern, then the same set of matches is
returned, but each match will also provide the leftmost possible start offset
corresponding to its end offset.

Using the SOM flag entails a number of trade-offs and limitations:

- Reduced pattern support: For many patterns, tracking SOM is complex and can
  result in Hyperscan failing to compile a pattern with a “Pattern too
  large” error, even if the pattern is supported in normal operation.

- Increased stream state: At scan time, state space is required to track
  potential SOM offsets, and this must be stored in persistent stream state in
  streaming mode. Accordingly, SOM will generally increase the stream state
  required to match a pattern.

- Performance overhead: Similarly, there is generally a performance cost
  associated with tracking SOM.

- Incompatible features: Some other Hyperscan pattern flags (such as [`HS_FLAG_SINGLEMATCH`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_SINGLEMATCH "HS_FLAG_SINGLEMATCH") and [`HS_FLAG_PREFILTER`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_PREFILTER "HS_FLAG_PREFILTER")) can not be
  used in combination with SOM. Specifying them together with [`HS_FLAG_SOM_LEFTMOST`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_SOM_LEFTMOST "HS_FLAG_SOM_LEFTMOST") will result in a compilation error.

In streaming mode, the amount of precision delivered by SOM can be controlled
with the SOM horizon flags. These instruct Hyperscan to deliver accurate SOM
information within a certain distance of the end offset, and return a special
start offset of [`HS_OFFSET_PAST_HORIZON`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_OFFSET_PAST_HORIZON "HS_OFFSET_PAST_HORIZON") otherwise. Specifying a
small or medium SOM horizon will usually reduce the stream state required for a
given database.

Note

In streaming mode, the start offset returned for a match may refer to
a point in the stream *before* the current block being scanned. Hyperscan
provides no facility for accessing earlier blocks; if the calling application
needs to inspect historical data, then it must store it itself.

### 扩展参数(Extended Parameters)

In some circumstances, more control over the matching behaviour of a pattern is
required than can be specified easily using regular expression syntax. For
these scenarios, Hyperscan provides the [`hs_compile_ext_multi()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile_ext_multi "hs_compile_ext_multi") function
that allows a set of “extended parameters” to be set on a per-pattern basis.

Extended parameters are specified using an [`hs_expr_ext_t`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_expr_ext_t "hs_expr_ext_t") structure,
which provides the following fields:

- `flags`: Flags governing which of the other fields in the structure are
  used.

- `min_offset`: The minimum end offset in the data stream at which this
  expression should match successfully.

- `max_offset`: The maximum end offset in the data stream at which this
  expression should match successfully.

- `min_length`: The minimum match length (from start to end) required to
  successfully match this expression.

- `edit_distance`: Match this expression within a given Levenshtein distance.

- `hamming_distance`: Match this expression within a given Hamming distance.

These parameters either allow the set of matches produced by a pattern to be
constrained at compile time (rather than relying on the application to process
unwanted matches at runtime), or allow matching a pattern approximately (within
a given edit distance) to produce more matches.

For example, the pattern `/foo.*bar/` when given a `min_offset` of 10
and a `max_offset` of 15 will not produce matches when scanned against `foobar` or `foo0123456789bar` but will produce a match against the data
streams `foo0123bar` or `foo0123456bar`.

Similarly, the pattern `/foobar/` when given an `edit_distance` of 2
will produce matches when scanned against `foobar`, `f00bar`, `fooba`, `fobr`, `fo_baz`, `foooobar`, and anything else that lies within edit
distance of 2 (as defined by Levenshtein distance).

When the same pattern `/foobar/` is given a `hamming_distance` of 2,
it will produce matches when scanned against `foobar`, `boofar`, `f00bar`, and anything else with at most two characters substituted from the
original pattern. For more details, see the [Approximate matching](http://intel.github.io/hyperscan/dev-reference/compilation.html#approximate-matching) section.

### 预过滤模式(Prefiltering Mode)

Hyperscan provides a per-pattern flag, [`HS_FLAG_PREFILTER`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_PREFILTER "HS_FLAG_PREFILTER"), which can
be used to implement a prefilter for a pattern than Hyperscan would not
ordinarily support.

This flag instructs Hyperscan to compile an “approximate” version of this
pattern for use in a prefiltering application, even if Hyperscan does not
support the pattern in normal operation.

The set of matches returned when this flag is used is guaranteed to be a
superset of the matches specified by the non-prefiltering expression.

If the pattern contains pattern constructs not supported by Hyperscan (such as
zero-width assertions, back-references or conditional references) these
constructs will be replaced internally with broader constructs that may match
more often.

For example, the pattern `/(\w+) again \1/` contains the
back-reference `\1`. In prefiltering mode, this pattern might be
approximated by having its back-reference replaced with its referent, forming `/\w+ again \w+/`.

Furthermore, in prefiltering mode Hyperscan may simplify a pattern that would
otherwise return a “Pattern too large” error at compile time, or for performance
reasons (subject to the matching guarantee above).

It is generally expected that the application will subsequently confirm
prefilter matches with another regular expression matcher that can provide exact
matches for the pattern.

Note

The use of this flag in combination with Start of Match mode (using
the [`HS_FLAG_SOM_LEFTMOST`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_SOM_LEFTMOST "HS_FLAG_SOM_LEFTMOST") flag) is not currently supported and
will result in a pattern compilation error.

## Instruction Set Specialization

Hyperscan is able to make use of several modern instruction set features found
on x86 processors to provide improvements in scanning performance.

Some of these features are selected when the library is built; for example,
Hyperscan will use the native `POPCNT` instruction on processors where it is
available and the library has been optimized for the host architecture.

Note

By default, the Hyperscan runtime is built with the `-march=native` compiler flag and (where possible) will make use of all instructions known by
the host’s C compiler.

To use some instruction set features, however, Hyperscan must build a
specialized database to support them. This means that the target platform must
be specified at pattern compile time.

The Hyperscan compiler API functions all accept an optional [`hs_platform_info_t`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_platform_info_t "hs_platform_info_t") argument, which describes the target platform
for the database to be built. If this argument is NULL, the database will be
targeted at the current host platform.

The [`hs_platform_info_t`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_platform_info_t "hs_platform_info_t") structure has two fields:

1. `tune`: This allows the application to specify information about the target
   platform which may be used to guide the optimisation process of the compile.
   Use of this field does not limit the processors that the resulting database
   can run on, but may impact the performance of the resulting database.

2. `cpu_features`: This allows the application to specify a mask of CPU
   features that may be used on the target platform. For example, [`HS_CPU_FEATURES_AVX2`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_CPU_FEATURES_AVX2 "HS_CPU_FEATURES_AVX2") can be specified for Intel® Advanced
   Vector Extensions 2 (Intel® AVX2) instruction set support. If a flag
   for a particular CPU feature is specified, the database will not be usable on
   a CPU without that feature.

An [`hs_platform_info_t`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_platform_info_t "hs_platform_info_t") structure targeted at the current host can be
built with the [`hs_populate_platform()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_populate_platform "hs_populate_platform") function.

See [API Reference: Constants](http://intel.github.io/hyperscan/dev-reference/api_constants.html#api-constants) for the full list of CPU tuning and feature flags.

## 近似匹配(Approximate matching)

Hyperscan provides an experimental approximate matching mode, which will match
patterns within a given edit distance. The exact matching behavior is defined as
follows:

1. **Edit distance** is defined as Levenshtein distance. That is, there are
   three possible edit types considered: insertion, removal and substitution.
   A more formal description can be found on [Wikipedia](https://en.wikipedia.org/wiki/Levenshtein_distance).

2. **Hamming distance** is the number of positions by which two strings of
   equal length differ. That is, it is the number of substitutions required to
   convert one string to the other. There are no insertions or removals when
   approximate matching using a Hamming distance. A more formal description can
   be found on [Wikipedia](https://en.wikipedia.org/wiki/Hamming_distance).

3. **Approximate matching** will match all *corpora* within a given edit or
   Hamming distance. That is, given a pattern, approximate matching will match
   anything that can be edited to arrive at a corpus that exactly matches the
   original pattern.

4. **Matching semantics** are exactly the same as described in [Semantics](http://intel.github.io/hyperscan/dev-reference/compilation.html#semantics).

Here are a few examples of approximate matching:

- Pattern `/foo/` can match `foo` when using regular Hyperscan
  matching behavior. With approximate matching within edit distance 2, the
  pattern will produce matches when scanned against `foo`, `foooo`, `f00`, `f`, and anything else that lies within edit distance 2 of matching corpora
  for the original pattern (`foo` in this case).

- Pattern `/foo(bar)+/` with edit distance 1 will match `foobarbar`, `foobarb0r`, `fooarbar`, `foobarba`, `f0obarbar`, `fobarbar` and
  anything else that lies within edit distance 1 of matching corpora for the
  original pattern (`foobarbar` in this case).

- Pattern `/foob?ar/` with edit distance 2 will match `fooar`, `foo`, `fabar`, `oar` and anything else that lies within edit distance 2
  of matching corpora for the original pattern (`fooar` in this case).

Currently, there are trade-offs and limitations that come with approximate
matching support. Here they are, in a nutshell:

- Reduced pattern support:
  
  - For many patterns, approximate matching is complex and can result in
    Hyperscan failing to compile a pattern with a “Pattern too large” error,
    even if the pattern is supported in normal operation.
  
  - Additionally, some patterns cannot be approximately matched because they
    reduce to so-called “vacuous” patterns (patterns that match everything). For
    example, pattern `/foo/` with edit distance 3, if implemented,
    would reduce to matching zero-length buffers. Such patterns will result in a
    “Pattern cannot be approximately matched” compile error. Approximate
    matching within a Hamming distance does not remove symbols, so will not
    reduce to a vacuous pattern.
  
  - Finally, due to the inherent complexities of defining matching behavior,
    approximate matching implements a reduced subset of regular expression
    syntax. Approximate matching does not support UTF-8 (and other
    multibyte character encodings), and word boundaries (that is, `\b`, `\B` and other equivalent constructs). Patterns containing unsupported constructs
    will result in “Pattern cannot be approximately matched” compile error.
  
  - When using approximate matching in conjunction with SOM, all of the
    restrictions of SOM also apply. See [Start of Match](http://intel.github.io/hyperscan/dev-reference/compilation.html#som) for more
    details.

- Increased stream state/byte code size requirements: due to approximate
  matching byte code being inherently larger and more complex than exact
  matching, the corresponding requirements also increase.

- Performance overhead: similarly, there is generally a performance cost
  associated with approximate matching, both due to increased matching
  complexity, and due to the fact that it will produce more matches.

Approximate matching is always disabled by default, and can be enabled on a
per-pattern basis by using an extended parameter described in [Extended Parameters](http://intel.github.io/hyperscan/dev-reference/compilation.html#extparam).

## Logical Combinations

For situations when a user requires behaviour that depends on the presence or
absence of matches from groups of patterns, Hyperscan provides support for the
logical combination of patterns in a given pattern set, with three operators: `NOT`, `AND` and `OR`.

The logical value of such a combination is based on each expression’s matching
status at a given offset. The matching status of any expression has a boolean
value: *false* if the expression has not yet matched or *true* if the expression
has already matched. In particular, the value of a `NOT` operation at a given
offset is *true* if the expression it refers to is *false* at this offset.

For example, `NOT 101` means that expression 101 has not yet matched at this
offset.

A logical combination is passed to Hyperscan at compile time as an expression.
This combination expression will raise matches at every offset where one of its
sub-expressions matches and the logical value of the whole expression is *true*.

To illustrate, here is an example combination expression:

((301 OR 302) AND 303) AND (304 OR NOT 305)

If expression 301 matches at offset 10, the logical value of 301 is *true* while the other patterns’ values are *false*. Hence, the whole combination’s value is *false*.

Then expression 303 matches at offset 20. Now the values of 301 and 303 are *true* while the other patterns’ values are still *false*. In this case, the
combination’s value is *true*, so the combination expression raises a match at
offset 20.

Finally, expression 305 has matches at offset 30. Now the values of 301, 303 and 305
are *true* while the other patterns’ values are still *false*. In this case, the
combination’s value is *false* and no match is raised.

**Using Logical Combinations**

In logical combination syntax, an expression is written as infix notation, it
consists of operands, operators and parentheses. The operands are expression
IDs, and operators are `!` (NOT), `&` (AND) or `|` (OR). For example, the
combination described in the previous section would be written as:

((301 | 302) & 303) & (304 | !305)

In a logical combination expression:

> - The priority of operators are `!` > `&` > `|`. For example:
>   
>   - `A&B|C` is treated as `(A&B)|C`,
>   
>   - `A|B&C` is treated as `A|(B&C)`,
>   
>   - `A&!B` is treated as `A&(!B)`.
> 
> - Extra parentheses are allowed. For example:
>   
>   - `(A)&!(B)` is the same as `A&!B`,
>   
>   - `(A&B)|C` is the same as `A&B|C`.
> 
> - Whitespace is ignored.

To use a logical combination expression, it must be passed to one of the
Hyperscan compile functions ([`hs_compile_multi()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile_multi "hs_compile_multi"), [`hs_compile_ext_multi()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_compile_ext_multi "hs_compile_ext_multi")) along with the [`HS_FLAG_COMBINATION`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_COMBINATION "HS_FLAG_COMBINATION") flag,
which identifies the pattern as a logical combination expression. The patterns
referred to in the logical combination expression must be compiled together in
the same pattern set as the combination expression.

When an expression has the [`HS_FLAG_COMBINATION`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_COMBINATION "HS_FLAG_COMBINATION") flag set, it ignores
all other flags except the [`HS_FLAG_SINGLEMATCH`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_SINGLEMATCH "HS_FLAG_SINGLEMATCH") flag and the [`HS_FLAG_QUIET`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_QUIET "HS_FLAG_QUIET") flag.

Hyperscan will accept logical combination expressions at compile time that
evaluate to *true* when no patterns have matched, and report the match for
combination at end of data if no patterns have matched; for example:

!101
!101|102
!101&!102
!(101&102)

Patterns that are referred to as operands within a logical combination (for
example, 301 through 305 in the examples above) may also use the [`HS_FLAG_QUIET`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_QUIET "HS_FLAG_QUIET") flag to silence the reporting of individual matches
for those patterns. In the absence of this flag, all matches (for
both individual patterns and their logical combinations) will be reported.

When an expression has both the [`HS_FLAG_COMBINATION`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_COMBINATION "HS_FLAG_COMBINATION") flag and the [`HS_FLAG_QUIET`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_FLAG_QUIET "HS_FLAG_QUIET") flag set, no matches for this logical combination
will be reported.
