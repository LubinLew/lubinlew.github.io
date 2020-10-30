# Environment variables

All environment variables starting with `TEST_NGINX_` are expanded in the sections used to build the configuration of the server that tests automatically starts. The following environment variables are supported by this module:

## [TEST_NGINX_REUSE_PORT](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_REUSE_PORT)

When this environment is set to a true value, the test scaffold would add the "resuseport" parameter to the "listen" directive automatically generated in *nginx.conf*.

## [TEST_NGINX_USE_HTTP2](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_USE_HTTP2)

Enables the "http2" test mode by enforcing using the (plain text) HTTP/2 protocol to send the test request.

Under the hood, the test scaffold uses the `curl` command-line utility to do the wire communication with the NGINX server. The `curl` utility must be recent enough to support both the `--http2` and `--http2-prior-knowledge` command-line options.

**WARNING:** not all the sections and features are supported in the "http2" test mode. For example, the [pipelined_requests](https://metacpan.org/pod/pipelined_requests) and [raw_request](https://metacpan.org/pod/raw_request) will still use the HTTP/1 protocols even in the "http2" test mode. Similarly, test blocks explicitly require the HTTP 1.0 protocol will still use HTTP 1.0.

One can enable HTTP/2 mode for an individual test block by specifying the [http2](https://metacpan.org/pod/http2) section, as in

`--- http2`

## [TEST_NGINX_VERBOSE](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_VERBOSE)

Controls whether to output verbose debugging messages in Test::Nginx. Default to empty.

## [TEST_NGINX_BENCHMARK](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_BENCHMARK)

When set to an non-empty and non-zero value, then the test scaffold enters the benchmarking testing mode by invoking `weighttp` (for HTTP 1.1 requests) and `ab` (for HTTP 1.0 requests) to run each test case with the test request repeatedly.

When specifying a positive number as the value, then this number is used for the total number of repeated requests. For example,

`export TEST_NGINX_BENCHMARK=1000`

will result in 1000 repeated requests for each test block. Default to `100000`.

When a second number is specified (separated from the first number by spaces), then this second number is used for the concurrency level for the benchmark. For example,

`export TEST_NGINX_BENCHMARK=``'1000 10'`

will result in 1000 repeated requests over 10 concurrent connections for each test block. The default concurrency level is 2 (or 1 if the number of requests is 1).

The "benchmark" testing mode will also output to stderr the actual "ab" or "weighttp" command line used by the test scaffold. For example,

`weighttp -c2 -k -n2000 -H` `'Host: foo.com'` `http://127.0.0.1:1984/t`

See also the `TEST_NGINX_BENCHMARK_WARMUP` environment.

This testing mode requires the `unbuffer` command-line utility from the `expect` package.

## [TEST_NGINX_BENCHMARK_WARMUP](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_BENCHMARK_WARMUP)

Specify the number of "warm-up" requests performed before the actual benchmark requests for each test block.

The latencies of the warm-up requests never get included in the final benchmark results.

Only meaningful in the "benchmark" testing mode.

See also the `TEST_NGINX_BENCHMARK` environment.

## [TEST_NGINX_CHECK_LEAK](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_CHECK_LEAK)

When set to 1, the test scaffold performs the most general memory leak test by means of calling `weighttpd`/`ab` and `ps`.

Specifically, it starts `weighttp` (for HTTP 1.1 `GET` requests) or `ab` (for HTTP 1.0 requests) to repeatedly hitting Nginx for seconds in a sub-process, and then after about 1 second, it will start sampling the RSS value of the Nginx process by calling the `ps` utility every 20 ms. Finally, it will output all the sample point data and the line slope of the linear regression result on the 100 sample points.

One typical output for non-leaking test cases:

`t/075-logby.t .. 3/17 TEST 2: log_by_lua_file`

`LeakTest: [2176 2176 2176 2176 2176 2176 2176`

 `2176 2176 2176 2176 2176 2176 2176 2176 2176`

 `2176 2176 2176 2176 2176 2176 2176 2176 2176`

 `2176 2176 2176 2176 2176 2176 2176 2176 2176`

 `2176 2176 2176 2176 2176 2176 2176 2176 2176`

 `2176 2176 2176 2176 2176 2176 2176 2176 2176`

 `2176 2176 2176 2176 2176 2176 2176 2176 2176`

 `2176 2176 2176 2176 2176 2176 2176 2176 2176`

 `2176 2176 2176 2176 2176 2176 2176 2176 2176`

 `2176 2176 2176 2176 2176 2176 2176 2176 2176`

 `2176 2176 2176 2176 2176 2176 2176 2176 2176`

 `2176 2176 2176]`

`LeakTest: k=0.0`

and here is an example of leaking:

`TEST 5: ngx.ctx available in log_by_lua (not` `defined` `yet)`

`LeakTest: [4396 4440 4476 4564 4620 4708 4752`

 `4788 4884 4944 4996 5032 5080 5132 5188 5236`

 `5348 5404 5464 5524 5596 5652 5700 5776 5828`

 `5912 5964 6040 6108 6108 6316 6316 6584 6672`

 `6672 6752 6820 6912 6912 6980 7064 7152 7152`

 `7240 7340 7340 7432 7508 7508 7600 7700 7700`

 `7792 7896 7896 7992 7992 8100 8100 8204 8296`

 `8296 8416 8416 8512 8512 8624 8624 8744 8744`

 `8848 8848 8968 8968 9084 9084 9204 9204 9324`

 `9324 9444 9444 9584 9584 9704 9704 9832 9832`

 `9864 9964 9964 10096 10096 10488 10488 10488`

 `10488 10488 11052 11052]`

`LeakTest: k=64.1`

Even very small leaks can be amplified and caught easily by this testing mode because their slopes will usually be far above `1.0`.

One can configure the number of sample points via the [TEST_NGINX_CHECK_LEAK_COUNT](https://metacpan.org/pod/TEST_NGINX_CHECK_LEAK_COUNT) system environment, for example, to sample 1000 data points, we can set the following environment *before* running the test:

`export TEST_NGINX_CHECK_LEAK_COUNT=1000`

For now, only `GET`, `POST`, `PUT`, and `HEAD` requests are supported (due to the limited HTTP support in both `ab` and `weighttp`). Other methods specified in the test cases will turn to `GET` with force.

The tests in this mode will always succeed because this mode also enforces the "dry-run" mode.

Test blocks carrying the "--- no_check_leak" directive will be skipped in this testing mode.

## [TEST_NGINX_CHECK_LEAK_COUNT](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_CHECK_LEAK_COUNT)

Takes a number value which controls how many data points to be sampled in the "check leak" test mode. See [TEST_NGINX_CHECK_LEAK](https://metacpan.org/pod/TEST_NGINX_CHECK_LEAK) for more details.

Defaults to 100.

## [TEST_NGINX_USE_HUP](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_USE_HUP)

When set to 1, the test scaffold will try to send `HUP` signal to the Nginx master process to reload the config file between successive test blocks (but not successive `repeast_each` sub-tests within the same test block). When this environment is set to 1, it will also enforce the "master_process on" config line in the *nginx.conf* file, because Nginx is buggy in processing HUP signal when the master process is off.

## [TEST_NGINX_LOAD_MODULES](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_LOAD_MODULES)

This environment takes a list of dynamic NGINX module files' paths. The test scaffold generates a series of `load_module` directives in the top-level scope of *nginx.conf*.

For example, when this environment takes the value `../../work/nginx/modules/ngx_http_headers_more_filter_module.so ../../work/nginx/modules/ngx_http_lua_module.so` , then the following snippet will be generated in *nginx.conf*:

`load_module ../../work/nginx/modules/ngx_http_headers_more_filter_module.so;`

`load_module ../../work/nginx/modules/ngx_http_lua_module.so;`

This requires at least NGINX 1.9.11 to work.

## [TEST_NGINX_POSTPONE_OUTPUT](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_POSTPONE_OUTPUT)

Defaults to empty. This environment takes positive integer numbers as its value and it will cause the auto-generated nginx.conf file to have a "postpone_output" setting in the http {} block.

For example, setting TEST_NGINX_POSTPONE_OUTPUT to 1 will have the following line in nginx.conf's http {} block:

`postpone_output 1;`

and it will effectively disable the write buffering in nginx's ngx_http_write_module.

## [TEST_NGINX_NO_CLEAN](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_NO_CLEAN)

When this environment is set to 1, it will prevent the test scaffold from quitting the Nginx server at the end of the run. This is very useful when you want to use other tools like gdb or curl inspect the Nginx server manually afterwards.

## [TEST_NGINX_NO_NGINX_MANAGER](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_NO_NGINX_MANAGER)

Defaults to 0. If set to 1, Test::Nginx module will not manage (configure/start/stop) the `nginx` process. Can be useful to run tests against an already configured (and running) nginx server.

## [TEST_NGINX_NO_SHUFFLE](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_NO_SHUFFLE)

Defaults to 0. If set to 1, will make sure the tests are run in the order they appear in the test file (and not in random order).

## [TEST_NGINX_USE_VALGRIND](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_USE_VALGRIND)

If set, Test::Nginx will start nginx with valgrind with the the value of this environment as the options.

Nginx is actually started with `valgrind -q $TEST_NGINX_USE_VALGRIND --gen-suppressions=all --suppressions=valgrind.suppress`, the suppressions option being used only if there is actually a valgrind.suppress file.

If this environment is set to the number `1` or any other non-zero numbers, then it is equivalent to taking the value `--tool=memcheck --leak-check=full`.

## [TEST_NGINX_USE_RR](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_USE_RR)

Uses Mozilla rr to record the execution of the nginx server run by the test scaffold.

This feature is experimental.

## [TEST_NGINX_USE_STAP](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_USE_STAP)

When set to true values (like 1), the test scaffold will use systemtap to instrument the nginx process.

You can specify the stap script in the `stap` section.

Note that you need to use the `stap-nginx` script from the `nginx-dtrace` project.

## [TEST_NGINX_STAP_OUT](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_STAP_OUT)

You can specify the output file for the systemtap tool. By default, a random file name under the system temporary directory is generated.

It's common to specify `TEST_NGINX_STAP_OUT=/dev/stderr` when debugging.

## [TEST_NGINX_BINARY](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_BINARY)

The command to start nginx. Defaults to `nginx`. Can be used as an alternative to setting `PATH` to run a specific nginx instance.

## [TEST_NGINX_LOG_LEVEL](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_LOG_LEVEL)

Value of the last argument of the `error_log` configuration directive. Defaults to `debug`.

## [TEST_NGINX_MASTER_PROCESS](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_MASTER_PROCESS)

Value of the `master_process` configuration directive. Defaults to `off`.

## [TEST_NGINX_SERVER_PORT](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_SERVER_PORT)

Value of the port the server started by Test::Nginx will listen to. If not set, `TEST_NGINX_PORT` is used. If `TEST_NGINX_PORT` is not set, then `1984` is used. See below for typical use.

## [TEST_NGINX_CLIENT_PORT](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_CLIENT_PORT)

Value of the port Test::Nginx will direct requests to. If not set, `TEST_NGINX_PORT` is used. If `TEST_NGINX_PORT` is not set, then `1984` is used. A typical use of this feature is to test extreme network conditions by adding a "proxy" between Test::Nginx and nginx itself. This is described in the `etcproxy integration` section of this module README.

## [TEST_NGINX_PORT](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_PORT)

A shortcut for setting both `TEST_NGINX_CLIENT_PORT` and `TEST_NGINX_SERVER_PORT`.

## [TEST_NGINX_SLEEP](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_SLEEP)

How much time (in seconds) should Test::Nginx sleep between two calls to `syswrite` when sending request data. Defaults to 0.015 (seconds).

## [TEST_NGINX_FORCE_RESTART_ON_TEST](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_FORCE_RESTART_ON_TEST)

Defaults to 1. If set to 0, Test::Nginx will not restart the nginx server when the config does not change between two tests.

## [TEST_NGINX_SERVROOT](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_SERVROOT)

The root of the nginx "hierarchy" (where you find the conf, *_tmp and logs directories). This value will be used with the `-p` option of `nginx`. Defaults to appending `t/servroot` to the current directory.

## [TEST_NGINX_IGNORE_MISSING_DIRECTIVES](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_IGNORE_MISSING_DIRECTIVES)

If set to 1 will SKIP all tests which `config` sections resulted in a `unknown directive` when trying to start `nginx`. Useful when you want to run tests on a build of nginx that does not include all modules it should. By default, these tests will FAIL.

## [TEST_NGINX_EVENT_TYPE](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_EVENT_TYPE)

This environment can be used to specify a event API type to be used by Nginx. Possible values are `epoll`, `kqueue`, `select`, `rtsig`, `poll`, and others.

For example,

`$ TEST_NGINX_EVENT_TYPE=``select` `prove -r t`

## [TEST_NGINX_ERROR_LOG](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_ERROR_LOG)

Error log files from all tests will be appended to the file specified with this variable. There is no default value which disables the feature. This is very useful when debugging. By default, each test triggers a start/stop cycle for `nginx`. All logs are removed before each restart, so you can only see the logs for the last test run (which you usually do not control except if you set `TEST_NGINX_NO_SHUFFLE=1`). With this, you accumulate all logs into a single file that is never cleaned up by Test::Nginx.

## [TEST_NGINX_RANDOMIZE](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_RANDOMIZE)

When set, the test scaffold forces the use of random server listening port numbers as well as random `t/servroot_XXXX/` directories. This can help test suite run in multiple parallel jobs via `prove -jN` where `N` is an integer bigger than 1. For instance, `prove -j8 -r t` runs the test suite under *t/* in 8 parallel jobs, utilizing up to 8 (logical) CPU cores in the same machine.

Note that only test suite *without* external shared and writable service dependencies (like Memcached, Redis or MySQL) can run in parallel in this way, obviously.

## [TEST_NGINX_WORKER_USER](https://metacpan.org/pod/Test::Nginx::Socket#TEST_NGINX_WORKER_USER)

Sets the user account used to run the nginx worker processes when the master process is enabled. This requires root access to run the nginx master process. For instance,

`export TEST_NGINX_WORKER_USER=``'agentzh'`

Defaults to the `root` when the master is run by `root` also. Otherwise defaults to the current user.

One can also add an optional user group separated by spaces, as in

`export TEST_NGINX_WORKER_USER=``'agentzh wheel'`

## [Valgrind Integration](https://metacpan.org/pod/Test::Nginx::Socket#Valgrind-Integration)

Test::Nginx has integrated support for valgrind ([http://valgrind.org](http://valgrind.org/)) even though by default it does not bother running it with the tests because valgrind will significantly slow down the test suite.

First ensure that your valgrind executable visible in your PATH env. And then run your test suite with the `TEST_NGINX_USE_VALGRIND` env set to true:

`TEST_NGINX_USE_VALGRIND=1 prove -r t`

If you see false alarms, you do have a chance to skip them by defining a ./valgrind.suppress file at the root of your module source tree, as in

[drizzle-nginx-module/valgrind.suppress at master · openresty/drizzle-nginx-module · GitHub](https://github.com/chaoslawful/drizzle-nginx-module/blob/master/valgrind.suppress)

This is the suppression file for ngx_drizzle. Test::Nginx will automatically use it to start nginx with valgrind memcheck if this file does exist at the expected location.

If you do see a lot of "Connection refused" errors while running the tests this way, then you probably have a slow machine (or a very busy one) that the default waiting time is not sufficient for valgrind to start. You can define the sleep time to a larger value by setting the `TEST_NGINX_SLEEP` env:

`TEST_NGINX_SLEEP=1 prove -r t`

The time unit used here is "second". The default sleep setting just fits my ThinkPad (`Core2Duo T9600`).

Applying the no-pool patch to your nginx core is recommended while running nginx with valgrind:

[GitHub - openresty/no-pool-nginx: replace nginx&#39;s pool mechanism with plain malloc &amp; free to help tools like valgrind](https://github.com/shrimp/no-pool-nginx)

The nginx memory pool can prevent valgrind from spotting lots of invalid memory reads/writes as well as certain double-free errors. We did find a lot more memory issues in many of our modules when we first introduced the no-pool patch in practice ;)

There's also more advanced features in Test::Nginx that have never documented. I'd like to write more about them in the near future ;)

## [Etcproxy Integration](https://metacpan.org/pod/Test::Nginx::Socket#Etcproxy-Integration)

**WARNING:** use etcproxy is no longer recommended because the mockeagain is way more effective and efficient:

[GitHub - openresty/mockeagain: Mocking ideally slow network that only allows reading and/or writing one byte at a time](https://github.com/openresty/mockeagain)

The default settings in etcproxy (https://github.com/chaoslawful/etcproxy) makes this small TCP proxy split the TCP packets into bytes and introduce 1 ms latency among them.

There's usually various TCP chains that we can put etcproxy into, for example

### [est::Nginx <=> nginx](https://metacpan.org/pod/Test::Nginx::Socket#Test::Nginx-%3C=%3E-nginx)T

`$ ./etcproxy 1234 1984`

Here we tell etcproxy to listen on port 1234 and to delegate all the TCP traffic to the port 1984, the default port that Test::Nginx makes nginx listen to.

And then we tell Test::Nginx to test against the port 1234, where etcproxy listens on, rather than the port 1984 that nginx directly listens on:

`$ TEST_NGINX_CLIENT_PORT=1234 prove -r t/`

Then the TCP chain now looks like this:

`Test::Nginx <=> etcproxy (1234) <=> nginx (1984)`

So etcproxy can effectively emulate extreme network conditions and exercise "unusual" code paths in your nginx server by your tests.

In practice, *tons* of weird bugs can be captured by this setting. Even ourselves didn't expect that this simple approach is so effective.

### [nginx <=> memcached](https://metacpan.org/pod/Test::Nginx::Socket#nginx-%3C=%3E-memcached)

We first start the memcached server daemon on port 11211:

`memcached -p 11211 -vv`

and then we another etcproxy instance to listen on port 11984 like this

`$ ./etcproxy 11984 11211`

Then we tell our t/foo.t test script to connect to 11984 rather than 11211:

`# foo.t`

`use` `[Test::Nginx::Socket](https://metacpan.org/pod/Test::Nginx::Socket);`

`repeat_each(1);`

`plan` `tests` `=> 2 * repeat_each() * blocks();`

`$ENV``{TEST_NGINX_MEMCACHED_PORT} ||= 11211;` `# make this env take a default value`

`run_tests();`

`__DATA__`

`=== TEST 1: sanity`

`--- config`

`location /foo {`

 `set $memc_cmd set;`

 `set $memc_key foo;`

 `set $memc_value bar;`

 `memc_pass 127.0.0.1:$TEST_NGINX_MEMCACHED_PORT;`

`}`

`--- request`

 `GET /foo`

`--- response_body_like: STORED`

`--- error_code: 201`

The Test::Nginx library will automatically expand the special macro `$TEST_NGINX_MEMCACHED_PORT` to the environment with the same name. You can define your own `$TEST_NGINX_BLAH_BLAH_PORT` macros as long as its prefix is `TEST_NGINX_` and all in upper case letters.

And now we can run your test script against the etcproxy port 11984:

`TEST_NGINX_MEMCACHED_PORT=11984 prove t/foo.t`

Then the TCP chains look like this:

`Test::Nginx <=> nginx (1984) <=> etcproxy (11984) <=> memcached (11211)`

If `TEST_NGINX_MEMCACHED_PORT` is not set, then it will take the default value 11211, which is what we want when there's no etcproxy configured:

`Test::Nginx <=> nginx (1984) <=> memcached (11211)`

This approach also works for proxied mysql and postgres traffic. Please see the live test suite of ngx_drizzle and ngx_postgres for more details.

Usually we set both `TEST_NGINX_CLIENT_PORT` and `TEST_NGINX_MEMCACHED_PORT` (and etc) at the same time, effectively yielding the following chain:

`Test::Nginx <=> etcproxy (1234) <=> nginx (1984) <=> etcproxy (11984) <=> memcached (11211)`

as long as you run two separate etcproxy instances in two separate terminals.

It's easy to verify if the traffic actually goes through your etcproxy server. Just check if the terminal running etcproxy emits outputs. By default, etcproxy always dump out the incoming and outgoing data to stdout/stderr.

# 
