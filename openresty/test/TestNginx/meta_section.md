# Meta sections

## `--- ONLY`

只运行指定的测试case。 

It is also very intuitive for the developer's workflow. One does not have to specify a (unique) test name on the command-line; just find the block in the editor, insert a `--- ONLY` line right away, and run the current test file immediately (for Vim users, the final step is as simple as entering `:!prove %` where `:!` is the Vim way of running an external shell command and `%` would get substituted with the current file being edited in Vim's buffer).

This is definitely one of the most useful and frequently used features.

## `--- SKIP`

无条件的跳过指定测试case。

You can use `--- skip_nginx` and `--- skip_nginx2` providied by this module (see their documentation below) to conditionally skip tests according to the current NGINX server versions. You can also use `--- skip_openssl` (see its documentation below) to conditionally skip tests according to the current OpenSSL version.
