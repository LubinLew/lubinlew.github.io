# OpenResty 自动化测试工具 Test::Nginx

> [Automated Testing &#xB7; Programming OpenResty](https://openresty.gitbooks.io/programming-openresty/content/testing/)

自动化测试在软件开发和维护中扮演了重要的角色. OpenResty 提供了一个数据驱动的测试框架，它可以用来为Nginx C模块、Lua库 甚至OpenResty应用编写声明式的测试case。 测试case有规范的格式，容易阅读和编写，机器也容易处理。 The data-driven approach 
makes it easy to run the same tests in wildly different ways that can 
help expose issues in different scenarios or with different kinds of 
external tools.

This chapter introduces the `Test::Nginx` test scaffold that has been widely used to organize test suites for almost all the OpenResty components, including the `ngx_http_lua` module, most of the `lua-resty-*` Lua libraries, as well as full-blown business applications like CloudFlare’s Lua CDN and Lua SSL.

## Introduction

OpenResty itself has been relying on automated testing to remain high quality
over the years. As OpenResty core developers, we embrace the test driven
development (TDD) process all the time. An excellent result of our TDD
practices over the years is a huge set of test suites for all the OpenResty
components. These test suites are so large as a whole, so it is impractical
to run all the tests thoroughly on a single machine. A relatively large
test cluster is often run on Amazon EC2 to run all these tests in all existing
test modes. Lying at the heart of these test suites is usually the `Test::Nginx` test scaffold module developed by the OpenResty team.

The `Test::Nginx` scaffold provides a generic simple specification language
for expressing and organizing test cases in an intuitive way. It also provides
various powerful testing modes or "engines" to run the tests in various
different ways in the hope of exposing bugs in different settings. It is
also supported to extend the test specification language to add custom
abstractions for advanced testing needs, usually found in application-level
regression testing.

## Test::Nginx

[Test::Nginx](https://metacpan.org/pod/Test::Nginx) 是一个Nginx的测试框架，使用Perl语言编写，它不需要使用者熟悉Perl语言，因为`Test::Nginx`提供了一个简单的规则来编写测试case。它基于[Test::Base]([Test::Base - A Data Driven Testing Framework - metacpan.org](https://metacpan.org/pod/distribution/Test-Base/lib/Test/Base.pod) 模块，是其子集，所以`Test::Base`提供的特性， `Test::Nginx`都支持。

`Test::Nginx` 通过 [CPAN](http://www.cpan.org/)（the Comprehensive Perl Archive Network）发布, 使用下面的命令安装:

```bash
yum install -y cpanminus
cpanm Test::Nginx
```

For the first time that the `cpan` utility is run, you may be prompted
to configure the `cpan` utility to fit your requirements. If you are unsure
about those options, just choose the automatic configuration option (if
available) or just accept all the default settings.

`Test::Nginx` provides several different testing classes for different
user requirements. The most frequently used one is `Test::Nginx::Socket`.
The rest of this chapter will focus on this testing class and its subclasses.
We will use the names `Test::Nginx` and `Test::Nginx::Socket` interchangeably
from now on to mean the `Test::Nginx::Socket` test module and its subclasses,
unless otherwise specified.

| Note | There is actually another different testing scaffold called `Test::Nginx`,<br>created by Maxim Dounin and maintained by the official NGINX team. That<br>testing module is shipped with the [official<br>NGINX test suite](http://hg.nginx.org/nginx-tests/file/tip) and has no<br>relationship with our `Test::Nginx` except that both of these are meant<br>to test NGINX related code. The NGINX team’s `Test::Nginx` requires the<br>user to directly code in Perl to convey all the test cases, which means<br>that tests written for their `Test::Nginx` are not data driven and requires<br>decent knowledge about Perl programming. |
| ---- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |

## Test Suite Layout

Projects using `Test::Nginx` to drive their test suites usually have a
common directory layout and common test file name patterns to organize
their tests. This makes it easy for the user to reason about the
location of the test suite in a project source tree and the usage of
the tests. It is not really required, however, to use this common
convention; it is just highly recommended.

By convention, such projects have a `t/` directory at the root of their
source tree where test files reside in. Each test file contains test cases
that are closely related in some way and has the file extension `.t` to
easily identify themselves as "test files". Below is the directory tree
structure of a real-world test suite inside the [headers-more-nginx-module](https://github.com/openresty/headers-more-nginx-module) project:

└── t
    ├── bug.t
    ├── builtin.t
    ├── eval.t
    ├── input-conn.t
    ├── input-cookie.t
    ├── input-ua.t
    ├── input.t
    ├── phase.t
    ├── sanity.t
    ├── subrequest.t
    ├── unused.t
    └── vars.t

When you have many test files, you can also group them further with sub-directories
under `t/`. For example, in the [lua-nginx-module](https://github.com/openresty/lua-nginx-module) project, we have sub-directores like `023-rewrite/` and `024-access/` under
its `t/` directory.

In essence, each `.t` file is a Perl script file runnable by either `perl` or Perl’s universal test harness tool named [prove](http://perldoc.perl.org/prove.html).
We usually use the `prove` command-line utility to run such `.t` files to obtain test results.
Although `.t` files are Perl scripts per se, they usually do not have much
Perl code at all. Instead, all of the test cases are declared as cleanly
formatted "data" in these `.t` files.

| Note | The test suite layout convention we use here has also been used by the<br>Perl community for many years. Because `Test::Nginx` is written in Perl<br>and reuses Perl’s testing toolchain, it makes sense for us to simply follow<br>that convention in the NGINX and OpenResty world as well. |
| ---- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |

## Test File Layout

Test files usually have a common file extension, `.t`, to distinguish themselves
from other types of files in the source tree. Each test file is a Perl
script per se. `Test::Nginx` follows a special design that decomposes each
test file into two main parts: the first part is a very short prologue
that consists of a few lines of Perl code while the second part is a listing
of the test cases in a special data format. These two parts are separated
by the following special line

__DATA__

The `perl` interpreter or the `prove` utility stops interpreting the file
content as Perl source code once they see this special line. Everything
after this line is treated as **data** in plain text that is reachable
by the Perl code above this line. The most interesting part of each `.t` test file is the stuff after this line, i.e., the data part.

| Note | The special `__DATA__` notation is a powerful feature<br>of the Perl programming language that allows embedding arbitrary free-text<br>data in any Perl script files that can be manipulated by the containing<br>Perl scripts themselves. `Test::Nginx` takes advantage of this feature<br>to allow data-driven test case specifications in a simple format or language<br>that is easily understandable by everyone, even those without any prior<br>experiences in Perl programming. |
| ---- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |

### The Prologue Part

The first part, i.e., the "prologue" above the `__DATA__` line
is usually just a few lines of Perl code. You do not have to know Perl
programming to write them down because they are so simple and seldom or
never change. The simplest Perl code prologue is as follows:

```perl
use Test::Nginx::Socket 'no_plan';
run_tests();
```

The first line is just loading the Perl module (or class), `Test::Nginx::Socket` and passing the option `'no_plan'` to it to disable test plans (we will
talk more about test plans in later chapters and we do not bother worrying
about it here). `Test::Nginx::Socket` is one of the most popular classes
in the `Test::Nginx` test framework. The second line just calls the `run_tests` Perl function imported automatically from the `Test::Nginx::Socket` module
to run all the test cases defined in the data part of the test file (i.e.,
the things coming after the `__DATA__` line).

There are, however, more complicated prologue parts in many real-world
test suites. Such prologues usually define some special environment variables
or Perl variables that can be shared and referenced in the test cases defined
in the "data part", or just call some other Perl functions imported by
the `Test::Nginx::Socket` module to customize the testing configurations
and behaviors for the current test file. We will return to such fancier
prologues in later sections. They can be very helpful in some cases.

| Note | Perl allows function calls to omit the parentheses if the context<br>is unambiguous. So we may see Perl function calls without parentheses in<br>real-world test files' prologue part, like `run_tests;`. We may use such<br>forms in examples presented in later sections because they are more compact. |
| ---- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |

### The Data Part

The data part is the most important part of any test files powered by `Test::Nginx`.
This is where test cases reside. It uses a simple specification format
to express test cases so that the user does not use Perl or any other general-purpose
languages to present the tests themselves. This special specification format
is an instance of Domain-Specific Languages (DSL) where the "domain" is
defined as testing code running upon or inside NGINX. Use of a DSL to present
test cases opens the door for presenting the test cases as *data* instead
of code. This is also why `Test::Nginx` is a data-driven testing framework.

The test case specification in the data part is composed by a series of *test blocks*. Each test block usually corresponds to a single test case,
which has a *title*, an optional *description*, and a series of *data sections*.
The structure of a test block is described by the following template.

```test-base
=== title
optional description
goes here...
--- section1
value1 goes
here
--- section2
value2 is
here
--- section3
value3
```

#### Block Titles

As we can see, each test block starts with a title line prefixed by three
equal signs (`===`). It is important to *avoid* any leading spaces at the
beginning of the line. The title is mandatory and is important to describe
the intention of the current test case in the most concise form, and also
to identify the test block in the test report when test failures happen.
By convention we put a `TEST N:` prefix in this title, for instance, `TEST 3: test the simplest form`. Don’t worry about maintaining the test ordinal
numbers in these titles yourself, we will introduce a command-line utility
called [reindex](https://raw.githubusercontent.com/openresty/openresty-devel-utils/master/reindex) in a later section that can automatically update the ordinal numbers in
the block titles for you.

#### Block Descriptions

Each test block can carry an optional description right after the block
title line. This description can span multiple lines if needed. It is a
more detailed description of the intention of the test block than the block
title and may also give some background information about the current test.
Many test cases just omit this part for convenience.

#### Data Sections

Every test block carries one or more *data sections* right after the block
description (if any). Data sections always have a name and a value, which
specify any input data fields and the *expected* output data fields.

The name of a data section is the word after the line prefix `---`. Spaces
are allowed though not syntactically required after `---`. We usually use
a single space between the prefix and the section name for aesthetic considerations
and we hope that you follow this convention as well. The section names
usually contain just alphanumeric letters and underscore characters.

Section values are specified in two forms. One is all the lines after the
section name line, before the next section or the next block. The other
form is more concise and specifies the value directly on the same line
as the section name, but right after the first colon character (`:`). The
latter form requires that the value contains no line-breaks. Any spaces
around the colon are always discarded and never count as a part of the
section value; furthermore, the trailing line-break character in the one-line
form does not count either.

If no visible values come after the section name in either form, then the
section takes an empty string value, which is still a *defined* value,
however. On the other hand, omitting the section name (and value) altogether
makes that section *undefined*.

`Test::Nginx` offers various pre-defined data section names that can be
used in the test blocks for different purposes. Some data sections are
for specifying input data, some are for expected output, and some for controlling
whether the current test block should be run at all.

It is best to explain data sections in a concrete test block example.

```test-base
=== TEST 1: hello, world
This is just a simple demonstration of the
echo directive provided by ngx_http_echo_module.
--- config
location = /t {
    echo "hello, world!";
}
--- request
GET /t
--- response_body
hello, world!
--- error_code: 200
```

Here we have two input data sections, `config` and `request`, for specifying
a custom NGINX configuration snippet in the default `server {}` and the
HTTP request sent by the test scaffold to the test NGINX server, respectively.
In addition, we have one output data section, `response_body`, for specifying
the expected response body output by the test NGINX server. If the actual
response body data is different from what we specify under the `response_body` section, this test case fails. We have another output data section, `error_code`,
which specifies its value on the same line of the section name. We see
that a colon character is used to separate the section name and values.
Obviously, the `error_code` section specifies the expected HTTP response
status code, which is 200.

Empty lines around data sections are always discarded by `Test::Nginx::Socket`.
Thus the test block above can be rewritten as below without changing its
meaning.

```test-base
=== TEST 1: hello, world
This is just a simple demonstration of the
echo directive provided by ngx_http_echo_module.

--- config
location = /t {
    echo "hello, world!";
}

--- request
GET /t

--- response_body
hello, world!

--- error_code: 200
```

Some users prefer this style for aesthetic reasons. We are free to choose
whatever form you like.

There are also some special data sections that specify neither input nor
output. They are just used to *control* how test blocks are run. For example,
the `ONLY` section makes *only* the current test block in the current test
file run and all the other test blocks are skipped. This is extremely useful
for running an individual test block in any given file, which is a common
requirement while debugging a particular test failure. Also, the special `SKIP` section can skip running the containing test block unconditionally,
handy for preparing test cases for future features without introducing
any expected test failures. We will visit more such "control sections"
in later sections.

We shall see, in a later section, that the user can define their own data
sections or extend existing ones by writing a little bit of custom Perl
code to satisfy more complicated testing requirements.

#### Section Filters

Data sections can take one or more *filters*. Filters are handy when you
want to adjust or convert the section values in certain ways.

Syntactically, filters are specified right after the section name with
at least one space character as the separator. Multiple filters are also
separated by spaces and are applied in the order they are written.

`Test::Nginx::Socket` provides many filters for your convenience. Consider
the following data section from the aforementioned test block.

```test-base
--- error_code: 200
```

If we want to place the section value, 200, in a separate line, like below,

```test-base
--- error_code
200
```

then the section value would contain a trailing new line, which leads to
a test failure. This is because the one-line form always excludes the trailing
new-line character while the multi-line form always includes one. To explicitly
exclude the trailing new-line in the multi-line form, we can employ the `chomp` filter, as in

```test-base
--- error_code chomp
200
```

Now it has exactly the same semantics as the previous one-line form.

Some filters have a more dramatic effect on the section values. For instance,
the `eval` filter evaluates the section value as arbitrary Perl code, and
the Perl value resulting from the execution will be used as the final section
value. The following section demonstrates using the `eval` filter to produce
4096 a’s:

```test-base
--- response_body eval
"a" x 4096
```

The original value of the `response_body` section above is a Perl expression
where the `x` symbol is a Perl operator is used to construct a string that
repeats the string specified as the left-hand-side N times where N is specified
by the right-hand-side. The resulting 4096-byte Perl string after evaluating
this expression dictated by the `eval` filter will be used as the final
section value for comparison with the actual response body data. It is
obvious that use of the `eval` filter and a Perl expression here is much
more readable and manageable than directly pasting that 4096-byte string
in the test block.

As with data sections, the user can also define their own filters, as we
shall see in a later section.

### A Complete Example

We can conclude this section by a complete test file example given below,
with both the prologue part and the data part.

```test-base
use Test::Nginx::Socket 'no_plan';

run_tests();

__DATA__

=== TEST 1: hello, world
This is just a simple demonstration of the
echo directive provided by ngx_http_echo_module.
--- config
location = /t {
    echo "hello, world!";
}
--- request
GET /t
--- response_body
hello, world!
--- error_code: 200
```

We will see how to actually run such test files in the next section.

| Note | The test file layout described in this section is exactly the same<br>as the test files based on other test frameworks derived from `Test::Base`,<br>the superclass of `Test::Nginx::Socket`, except those specialized test<br>sections and specialized Perl functions defined only in `Test::Nginx::Socket`.<br>All the `Test::Base` derivatives share the same basic layout and syntax.<br>They proudly inherit the same veins of blood |
| ---- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |

## Running Tests

Like most Perl-based testing frameworks, `Test:Nginx` relies on Perl’s `prove` command-line utility to run the test files. The `prove` utility is usually
shipped with the standard perl distribution so we should already have it
when we have `perl` installed.

`Test::Nginx` always invokes a real NGINX server and a real socket client
to run the tests. It automatically uses the `nginx` program found in the
system environment `PATH`. It is your responsibility to specify the right `nginx` in your `PATH` environment for the test suite. Usually we just
specify the path of the `nginx` program inside the `OpenResty` installation
tree. For example,

```bash
export PATH=/usr/local/openresty/nginx/sbin:$PATH
```

Here we assume that OpenResty is installed to the default prefix, i.e., `/usr/local/openresty/`.

You can always use the `which` command to verify if the `PATH` environment
is indeed set properly:

```console
$ which nginx
/usr/local/openresty/nginx/sbin/nginx
```

For convenience, we usually wrap such environment settings in a custom
shell script so that we do not risk polluting the system-wide or account-wide
environment settings nor take on the burden of manually setting the environments
manually for every shell session. For example, I usually have a local bash
script named `go` in each project I work on. A typical `go` script might
look like below

```bash
#!/usr/bin/env bash

export PATH=/usr/local/openresty/nginx/sbin:$PATH

exec prove "$@"
```

Then we can use this `./go` script to substitute the `prove` utility in
any of the subsequent commands involving `prove`.

Because `Test::Nginx` makes heavy use of environment variables for the
callers to fine tune the testing behaviors (as we shall see in later sections),
such shell wrapper scripts also make it easy to manage all these environment
variable settings and hard to get things wrong.

| Note | Please do not confuse the name of this bash script with Google’s<br>Go programming language. It has nothing to do with the Go language in any<br>way. |
| ---- | ----------------------------------------------------------------------------------------------------------------------------------------------------- |

### Running A Single File

If you want to run a single test file, say, `t/foo.t`, then all you need
to do is just to type the following command in your terminal.

```bash
prove t/foo.t
```

Here inside `t/foo.t` we employ the simple test file example presented
in the previous section. We repeat the content below for the reader’s
convenience.

t/foo.t

```test-base
use Test::Nginx::Socket 'no_plan';

run_tests();

__DATA__

=== TEST 1: hello, world
This is just a simple demonstration of the
echo directive provided by ngx_http_echo_module.
--- config
location = /t {
    echo "hello, world!";
}
--- request
GET /t
--- response_body
hello, world!
--- error_code: 200
```

It is worth mentioning that we could run the following command instead
if we have a custom wrapper script called `./go` for `prove` (as mentioned
earlier in this section):

```bash
./go foo.t
```

When everything goes well, it generates an output like this:

t/foo.t .. ok
All tests successful.
Files=1, Tests=2, 0 wallclock secs (0.02 usr 0.01 sys + 0.08 cusr 0.03 csys = 0.14 CPU)
Result: PASS

This is a very concise summary. The first line tells you all tests were
passed while the second line gives you a summary of the number of test
files (1 in this case), the number of tests (2 in this case), and the wallclock
and CPU times used to run all the tests.

It is interesting to see that we have only one test block in the sample
test file but in the test summary output by `prove` we see that the number
of tests are 2. Why the difference? We can easily find it out by asking `prove` to generate a detailed test report for all the individual tests.
This is achieved by passing the `-v` option (meaning "verbose") to the `prove` command we used earlier:

```bash
prove -v t/foo.t
```

Now the output shows all the individual tests performed in that test file:

t/foo.t ..
ok 1 - TEST 1: hello, world - status code ok
ok 2 - TEST 1: hello, world - response_body - response is expected (req 0)
1..2
ok
All tests successful.
Files=1, Tests=2, 0 wallclock secs (0.01 usr 0.01 sys + 0.07 cusr 0.03 csys = 0.12 CPU)
Result: PASS

Obviously, the first test is doing the status code check, which is dictated
by the `error_code` data section in the test block, and the second test
is doing the response body check, required by the `response_body` section.
Now the mystery is solved.

It is worth mentioning that the `--- error_code: 200` section is automatically
assumed when no `error_code` section is explicitly provided in the test
block. So our test block above can be simplified by removing the `--- error_code: 200` line without affecting the number of tests. This is because that checking
200 response status code is so common that `Test::Nginx` makes it the default.
If you expect a different status code, like 500, then just add an explicit `error_code` section.

From this example, we can see that one test block can contain multiple
tests and the number of tests for any given test block can be determined
or predicted by looking at the data sections performing output checks.
This is important when we provide a "test plan" ourselves to the test file
where a "test plan" is the exact number of tests we *expect* the current
test file to run. If a different number of tests than the plan were actually
run, then the test result would be considered malicious even when all the
tests are passed successfully. Thus, a test plan adds a strong constraint
on the total number of tests expected to be run. For our `t/foo.t` file
here, however, we intentionally avoid providing any test plans by passing
the `'no_plan'` argument to the `use` statement that loads the `Test::Nginx::Socket` module. We will revisit the "test plan" feature and explain how to provide
one in a later section.

### Running Multiple Files

Running multiple test files is straightforward; just specify the file
names on the `prove` command line, as in

```bash
prove -v t/foo.t t/bar.t t/baz.t
```

If you want to run all the test files directly under the `t/` directory,
then using a shell wildcard can be handy:

```bash
prove -v t/*.t
```

In the case that you have sub-directories under `t/`, you can specify
the `-r` option to ask `prove` to recursively traverse the while directory
tree rooted at `t/` to find test files:

```bash
prove -r t/
```

This command is also the standard way to run the whole test suite of a
project.

### Running Individual Test Blocks

`Test::Nginx` makes it easy to run an individual test block in a given
file. Just add the special data section `ONLY` to that test block you want
to run individually and `prove` will skip all the other test blocks while
running that test file. For example,

```test-base
=== TEST 1: hello, world
This is just a simple demonstration of the
echo directive provided by ngx_http_echo_module.
--- config
location = /t {
    echo "hello, world!";
}
--- request
GET /t
--- response_body
hello, world!
--- ONLY
```

Now `prove` won’t run any other test blocks (if any) in the same test file.

This is very handy while debugging a particular test block. You can focus
on one test case at a time without worrying about other unrelated test
cases stepping in your way.

When using the [Vim](http://www.vim.org/) editor, we can quickly insert
a `--- ONLY` line to the test block we are viewing in the vim file buffer,
and then type `:!prove %` in the command mode of vim without leaving the
editor window. This works because vim automatically expands the special `%` placeholder with the path of the current active file being edited.
This workflow is great since you never leave your editor window and you
never have to type the title (or other IDs) of your test block nor the
path of the containing test file. You can quickly jump between test blocks
even across different files. Test-driven development usually demands very
frequent interactions and iterations, and `Test::Nginx` is particularly
optimized to speed up this process.

Sometimes you may forget to remove the `--- ONLY` line from some test files
even after debugging, this will incorrectly skip all the other tests in
those files. To catch such mistakes, `Test::Nginx` always reports a warning
for files using the `ONLY` special section, as in

```console
$ prove t/foo.t
t/foo.t .. # I found ONLY: maybe you're debugging?
t/foo.t .. ok
All tests successful.
Files=1, Tests=2, 0 wallclock secs (0.01 usr 0.00 sys + 0.09 cusr 0.03 csys = 0.13 CPU)
Result: PASS
```

This way it is much easier to identify any leftover `--- ONLY` lines.

Similar to `ONLY`, `Test::Nginx` also provides the `LAST` data section
to make the containing test block become the last test block being run
in that test file.

| Note | The special data sections `ONLY` and `LAST` are actually features<br>inherited from the `Test::Base` module. |
| ---- | ------------------------------------------------------------------------------------------------------------ |

### Skipping Tests

We can specify the special `SKIP` data section to skip running the containing
test block unconditionally. This is handy when we write a test case that
is for a future feature or a test case for a known bug that we haven’t
had the time to fix right now. For example,

```test-base
=== TEST 1: test for the future
--- config
    location /t {
      some_fancy_directive;
    }
--- request
    GET /t
--- response_body
blah blah blah
--- SKIP
```

It is also possible to skip a whole test file in the prologue part. Just
replace the `use` statement with the following form.

```Perl
use Test::Nginx::Socket skip_all => "some reasons";
```

Then running the test file gives something like follows.

t/foo.t .. skipped: some reasons

| Note | It is also possible to conditionally skip a whole test file but it<br>requires a little bit of Perl programming. Interested readers can try using<br>a `BEGIN {}` before the `use` statement to calculate the value of<br>the `skip_all` option on the fly. |
| ---- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |

### Test Running Order

#### Test File Running Order

Test files are usually run by the alphabetical order of their file names.
Some people prefer explicitly controlling the running order of their test
files by prefixing the test file names with number sequences like `001-`, `002-`, and etc.

The test suite of the [ngx_http_lua](https://github.com/openresty/lua-nginx-module#readme) module follows this practice, for example, which has test file names like
below

t/000-sanity.t
t/001-set.t
t/002-content.t
t/003-errors.t
...
t/139-ssl-cert-by.t

Although the `prove` utility supports running test files in multiple parallel
jobs via the `-jN` option, `Test::Nginx` does not really support this mode
since all the test cases share exactly the same test server directory, `t/servroot/`, and the same listening ports, as we have already seen, while
parallel running requires strictly isolated running environments for each
individual thread of execution. One can still manually split the test files
into different groups and run each group on a different (virtual) machine
or an isolated environment like a Linux container.

#### Test Block Running Order

By default, the `Test::Nginx` scaffold *shuffles* the test blocks in each
file and run them in a *random* order. This behavior encourages writing
self-contained and independent test cases and also increases the chance
of hitting a bug by actively mutating the relative running order of the
test cases. This may, indeed, confuse new comers, coming from a more traditional
testing platform.

We can always disable this test block shuffling behavior by calling the
Perl function, `no_shuffle()`, imported by the `Test::Nginx::Socket` module,
before the `run_tests()` call in the test file prologue. For example,

```Perl
use Test::Nginx::Socket 'no_plan';

no_shuffle();
run_tests();

__DATA__
...
```

With the `no_shuffle()` call in place, the test blocks are run in the exact
same order as their appearance in the test file.

## Preparing Tests

As we have seen in the previous sections, `Test::Nginx` provides a simple
declarative format to express test cases. Each test case is represented
by a test block. A test block consists of a title, an optional description,
and several data sections for specifying inputs and expected outputs. In
this section, we will have a close look at how to prepare such test cases
for different test requirements.

Designing test cases is an art, in many ways. It may, sometimes, take even
more time and effort than implementing the feature to be tested, according
to our own experience. `Test::Nginx` tries hard to make writing tests as
simple as possible but it still cannot automate the whole test case design
process. Only you know exactly what to test and how it can be tested anyway.
This section will focus on the basic primitives provided by `Test::Nginx` that you can take advantage of to devise clever and effective test cases.

### Preparing NGINX Configuration

In a test block, we can use different data sections to specify our custom
snippets in different positions of the final `nginx.conf` configuration
file generated by `Test::Nginx`.

The most common one is the `config` section which is used to insert custom
snippets inside the `server {}` configuration block for the default test
server. We can also use the `http_config` section to insert our custom
content into the `http {}` configuration block of `nginx.conf`. The `main_config` section can be used to insert content into the top-level scope of the NGINX
configuration. Let’s consider the following example.

```test-base
=== TEST 1:
--- main_config
env MY_ENVIRONMENT;

--- http_config
    init_worker_by_lua_block {
        print("init")
    }

--- config
    location = /t {
        echo ok;
    }

--- request
GET /t
--- response_body
ok
```

This test block will generate an `nginx.conf` file with the following basic
structure:

```nginx
...
env MY_ENVIRONMENT;

http {
    ...

    init_worker_by_lua_block {
        print("init")
    }

    server {
        ...

        location = /t {
            echo ok;
        }
    }
}
```

Please pay attention to how the `main_config`, `http_config`, and `config` data sections' values are mapped into different locations in the NGINX
configuration file.

When in doubt, we can always check out the actual `nginx.conf` file generated
by the test scaffold at the location `t/servroot/conf/nginx.conf` in the
current working directory (usually just being the root directory of the
current project).

`Test::Nginx` generates a new `nginx.conf` file for each test block, which
makes it possible for each test block to become self-contained. By default,
the test scaffold automatically starts a new NGINX server before running
each test block and shuts down the server immediately after running the
block. Fortunately, NGINX is a lightweight server and it is usually very
fast to start and stop. Thus, the test blocks are not that slow to run
as it might look.

### Preparing Requests

The simplest way to prepare a request is to use the `request` data section,
as in

```test-base
--- request
GET /t?a=1&b=2
```

The HTTP/1.1 protocol is used by default. You can explicitly make it use
the HTTP/1.0 protocol if desired:

```test-base
--- request
GET /t?a=1&b=2 HTTP/1.0
```

Leading spaces or empty lines in the value of the `request` section are
automatically discarded. You can even add comments by leading them with
a `#` character, as in

```test-base
--- request

    # this is a simple test:
    GET /t
```

You can add some additional request headers at the same time through the `more_headers` section as below.

```test-base
--- request
GET /t
--- more_headers
Foo: bar
Bar: baz
```

#### Pipelined Requests

Preparing pipelined HTTP requests are also possible. But you need to use
the `pipelined_requests` section instead of `request`. For instance,

```test-base
=== TEST 1: pipelined requests
--- config
    location = /t {
        echo ok;
    }

--- pipelined_requests eval
["GET /t", "GET /t"]

--- response_body eval
["ok\n", "ok\n"]
```

It is worth noting that we use the `eval` filter with the `pipelined_requests` section to treat the literal value of that section as Perl code. This way
we can construct a Perl array of the request strings, which is the expected
data format for the `pipelined_requests` section. Similarly we need a similar
trick for the `response_body` section when checking outputs. With an array
of expected response body data, we can expect and check different values
for different individual request in the pipeline. Note, however, not every
data section supports the same array-typed value semantics as `response_body`.

### Checking Responses

We have already visited the `response_body` and `error_code` data sections
for checking the response body data and response status code, respectively.

The `response_body` data section always performs an exact whole-string
comparison between the section value and the actual response body. It tries
to be clever when long string value comparison fails. Consider the following
sample output from `prove`.

t/foo.t .. 1/?

# Failed test 'TEST 1: long string test - response_body - response is expected (req 0)'

# at .../test-nginx/lib/Test/Nginx/Socket.pm line 1282.

# got: ..."IT 2.x is enabled.\x{0a}\x{0a}"...

# length: 409

# expected: ..."IT 2.x is not enabled.\x{0a}"...

# length: 412

# strings begin to differ at char 400 (line 1 column 400)

# Looks like you failed 1 test of 2.

/tmp/foo.t .. Dubious, test returned 1 (wstat 256, 0x100)
Failed 1/2 subtests
Test Summary Report

-------------------

/tmp/foo.t (Wstat: 256 Tests: 2 Failed: 1)
  Failed test:  2
  Non-zero exit status: 1
Files=1, Tests=2,  0 wallclock secs (0.01 usr 0.00 sys + 0.09 cusr 0.03 csys = 0.13 CPU)
Result: FAIL

From this test report, we can clearly see that

1. it is the test block with the title `TEST 1: long string test` that is
   failing,

2. it is the `response_body` data section check that fails,

3. the actual response body data is 409 bytes long while the expected value
   is 412 bytes, and

4. the expected value has an additional `not` word in the string fragment `IT 2.x is enabled` and the difference starts at the offset 400 in the
   long string.

Behind the scene, `Test::Nginx` uses the Perl module [Test::LongString](https://metacpan.org/pod/Test::LongString) to do the long string comparisons. It is also particularly useful while
checking response body data in binary formats.

If your response body data is in a multi-line textual format, then you
may also want to use a `diff`-style output when the data does not match.
To achieve this, we can call the `no_long_string()` Perl function before
the `run_tests()` function call in the prologue part of the test file.
Below is such an example.

```test-base
use Test::Nginx::Socket 'no_plan';

no_long_string();

run_tests();

__DATA__

=== TEST 1:
--- config
    location = /t {
        echo "Life is short.";
        echo "Moon is bright.";
        echo "Sun is shining.";
    }
--- request
GET /t
--- response_body
Life is short.
Moon is deem.
Sun is shining.
```

Note the `no_long_string()` call in the prologue part. It is important
to place it before the `run_tests()` call otherwise it would be too late
for it to take effect, obviously.

Invoking the `prove` utility (or any shell wrappers for it) to run this
test file gives the following details about the test failure:

# Failed test 'TEST 1: - response_body - response is expected (req 0)'

# at .../test-nginx/lib/Test/Nginx/Socket.pm line 1277.

# @@ -1,3 +1,3 @@

# Life is short.

# -Moon is deem.

# +Moon is bright.

# Sun is shining.

# Looks like you failed 1 test of 2.

It is obvious that the second line of the response body output is different.

You can even further disable the `diff`-style comparison mode by adding
a `no_diff()` Perl function call in the prologue part. Then the failure
report will look like this:

# Failed test 'TEST 1: - response_body - response is expected (req 0)'

# at .../test-nginx/lib/Test/Nginx/Socket.pm line 1277.

# got: 'Life is short.

# Moon is bright.

# Sun is shining.

# '

# expected: 'Life is short.

# Moon is deem.

# Sun is shining.

# '

# Looks like you failed 1 test of 2.

That is, `Test::Nginx` just gives full listing of the actual response body
data and the expected one without any abbreviations or hand-holding.

#### Pattern Matching on Response Bodies

When the request body may change in some ways or you just care about certain
key words in a long data string, you can specify a Perl regular expression
to do a pattern match against the actual request body data. This is achieved
by the `response_body_like` data section. For example,

```test-base
--- response_body_like: age: \d+
```

Be careful when you are using the multi-line data section value form. A
trailing newline character appended to your section value may make your
pattern never match. In this case the `chomp` filter we introduced in an
early section can be very helpful here. For example,

```test-base
--- response_body_like chomp
age: \d+
```

You can also use the `eval` filter to construct a Perl regular expression
object with a Perl expression, as in

```test-base
--- response_body_like eval
qr/age: \d+/
```

This is the most flexible form to specify a pattern.

| Note | Perl uses the `qr` quoting structure to explicitly construct regular<br>expression objects. You can use various different quoting forms like `qr/…​/`, `qr!…​!`, `qr#…​#`, and `qr{…​}`. |
| ---- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |

#### Checking Response Headers

The `response_headers` data section can be used to validate response header
entries. For example,

```test-base
--- response_headers
Foo: bar
Bar: baz
!Blah
```

This section dictates 3 tests actually:

1. The response header `Foo` must appear and must take the value `bar`;

2. The response header `Bar` must appear and must take the value `baz`;
   and

3. The response header `Blah` must not appear or take an empty value.

### Checking NGINX Error Logs

In addition to responses, the NGINX error log file is also an important
output channel for an NGINX server setup.

#### True-False Tests

One immediate testing requirement is to check whether or not a piece of
text appears in any error log messages. Such checks can be done via the
data sections `error_log` and `no_error_log`, respectively. The former
ensures that some lines in the error log file contain the string specified
as the section value while the latter tests the opposite: ensuring that
no line contains the pattern.

For example,

```test-base
--- error_log
Hello world from my server
```

Then the string `Hello world from my server` (without the trailing new-line)
must appear in at least one line of the NGINX error log. You can specify
multiple strings in separate lines of the section value to perform different
checks, for instance,

```test-base
--- error_log
This is a dog!
Is it a cat?
```

Then it performs two error log checks, one is to ensure that the string `This is a dog!` appears in some error log lines. The order of these two
string patterns do not matter at all.

If one of the string pattern failed to match any lines in the error log
file, then we would get a test failure report from `prove` like below.

# Failed test 'TEST 1: simple test - pattern "This is a dog!" matches a line in error.log (req 0)'

If you want to specify a Perl regular expression (regex) as one of the
patterns, then you should use the `eval` section filter to construct a
Perl-array as the section value, as in

```test-base
--- error_log eval
[
  "This is a dog!",
  qr/\w+ is a cat\?/,
]
```

As we have seen earlier, Perl regexes can be constructed via the `qr/…​/` quoting syntax. Perl string patterns in the Perl array specified by double
quotes or single quotes are still treated as plain string patterns, as
usual. If the array contains only one regex pattern, then you can omit
the array itself, as in

```test-base
--- error_log eval
qr/\w+ is a cat\?/
```

`Test::Nginx` puts the error log file of the test NGINX server in the file
path `t/servroot/logs/error.log`. As a test writer, we frequently check
out this file directly when things go wrong. For example, it is common
to make mistakes or typos in the patterns we specify for the `error_log` section. Also, scanning the raw log file can give us insight about the
details of the NGINX internal working when the NGINX debugging logs are
enabled in the NGINX build.

The `no_error_log` section is very similar to `error_log` but it checks
the nonexistence of the string patterns in the NGINX error log file. One
of the most frequent uses of the `no_error_log` section is to ensure that
there is *no* error level messages in the log file.

```test-base
--- no_error_log
[error]
```

If, however, there is a line in the nginx error log file that contains
the string `[error]`, then the test fails. Below is such an example.

# Failed test 'TEST 1: simple test - pattern "[error]" should not match any line in error.log but matches line "2016/02/01 11:59:50 [error] 1788\#0: *1 lua entry thread aborted: runtime error: content_by_lua(nginx.conf:42):2: bad"'

This is a great way to find the details of the error quickly by just looking
at the test report.

Like `error_log`, this section also supports Perl array values and Perl
regex values through the `eval` filter.

#### Grep Tests

The `error_log` and `no_error_log` sections are very handy in quickly checking
the appearance of contain patterns in the NGINX error log file. But they
have serious limitations in that it is impossible to impose stronger constraints
on the relative order of the messages containing the patterns nor on the
number of their occurrences.

To address such limitations, `Test::Nginx::Socket` provides an alternative
way to check NGINX error logs in a way similar to the famous UNIX tool, `grep`. The sections `grep_error_log` and `grep_error_log_out` are used
for this purpose. The test writer uses the `grep_error_log` section to
specify a pattern, with which the test framework scans through the NGINX
error log file and collect all the matched parts of the log file lines
along the way, forming a final result. This aggregated log data result
is then matched against the expected value specified as the value of the `grep_error_log_out` section, in a similar way as with the `response_body` section discussed above.

It is easiest to explain with a simple example.

```test-base
=== TEST 1: simple grep test for error logs
--- config
    location = /t {
        content_by_lua_block {
            print("it is matched!")
            print("it is matched!")
            print("it is matched!")
        }
    }
--- request
GET /t
--- grep_error_log: it is matched!
--- grep_error_log_out
it is matched!
it is matched!
it is matched!
```

Here we use the Lua function `print()` provided by the [ngx_http_lua](https://github.com/openresty/lua-nginx-module#readme) module to generate NGINX error log messages at the `notice` level. This
test case tests the number of the log messages containing the string `it is matched!`. It is important to note that only the *matched* part of the
log file lines are collected in the final result instead of the whole log
lines. This simplifies the comparison a lot since NGINX error log messages
can contain varying details like timestamps and connection numbers.

A more useful form of this test is to specify a Perl regex pattern in the `grep_error_log` section. Consider the following example.

```test-base
=== TEST 1: simple grep test for error logs
--- config
    location = /t {
        content_by_lua_block {
            print("test: before sleeping...")
            ngx.sleep(0.001)  -- sleeping for 1ms
            print("test: after sleeping...")
        }
    }
--- request
GET /t
--- grep_error_log eval: qr/test: .*?\.\.\./
--- grep_error_log_out
test: before sleeping...
test: after sleeping...
```

We specify a Perl regex pattern, `test: .*?\.\.\.`, here to filter out
all the error log messages starting with `test:` and ending with `…​`.
And naturally in this test we also require the relative order of these
two messages, that is, `before sleeping` must appear *before* `after sleeping`.
Otherwise, we shall see failure reports like below:

# Failed test 'TEST 1: simple grep test for error logs - grep_error_log_out (req 0)'

# at ..../lib/Test/Nginx/Socket.pm line 1048.

# got: "test: after sleeping...\x{0a}test: before sleeping...\x{0a}"

# length: 49

# expected: "test: before sleeping...\x{0a}test: after sleeping...\x{0a}"

# length: 49

# strings begin to differ at char 7 (line 1 column 7)

As with the `response_body` section, we can also call the `no_long_string()` Perl function before `run_tests()` in the test file prologue, so as to
disable the long string output mode and enable the `diff` mode. Then the
test failure would look like this:

# Failed test 'TEST 1: simple grep test for error logs - grep_error_log_out (req 0)'

# at .../lib/Test/Nginx/Socket.pm line 1044.

# @@ -1,2 +1,2 @@

# -test: before sleeping...

# test: after sleeping...

# +test: before sleeping...

Obviously, for this test case, the `diff` format looks better.

#### Extra Delay Before Log Checks

By default, `Test::Nginx::Socket` performs the NGINX error log checks not
long after it receives the complete HTTP response for the test request.
Sometimes, when the log messages are generated by the server after sending
out the response, the error log checks may be carried out too early
that the messages are not yet written into the log file. In this case,
we can specify an extra delay via the `wait` data section for the test
scaffold to wait for the error log messages. Here is an example:

```test-base
=== TEST 1: wait for the timer
--- config
    location = /t {
        content_by_lua_block {
            local function f(premature)
                print("HERE!")
            end
            assert(ngx.timer.at(0.1, f))
        }
    }
--- request
GET /t
--- error_log
HERE!
--- no_error_log
[error]
--- wait: 0.12
```

Here we create a timer via the `ngx.timer.at` Lua function, which expires
after 0.1 seconds. Due to the asynchronous nature of timers, the request
handler does not wait for the timer to expire and immediately finishes
processing the current request and sends out a response with an empty body.
To check for the log message `HERE!` generated by the timer handler `f`,
we have to specify an extra delay for the test scaffold to wait. The 0.12
seconds time is specified in this example but any values larger than 0.1
would suffice. Without the `wait` section, this test case would fail with
the following output:

# Failed test 'TEST 1: wait for the timer - pattern "HERE!" matches a line in error.log (req 0)'

Obviously the test scaffold checks the error log too soon, even before
the timer handler runs.

### Section Review

`Test::Nginx::Socket` offers a rich set of data sections for specifying
various different input data and expected output data, ranging from NGINX
configuration file snippets, test requests, to expected responses and error
log messages. We have already demonstrated the power of data driven testing
and declarative test case crafting. We want to achieve multiple goals at
the same time, that is, not only to make the tests self-contained and highly
readable, but also to make the test report easy to interpret and analyze
when some of the tests fail. Raw files automatically generated by the test
scaffold, like `t/servroot/conf/nginx.conf` and `t/servroot/logs/error.log`,
should be checked frequently when manually debugging the test cases. The
next section extends the discussion of this section with a focus on testing
erroneous cases.
