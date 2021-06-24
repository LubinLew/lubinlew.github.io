# POSIX Shell 标准

> [Shell Command Language](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html)

`shell` 是命令语言解释器。本文描述的是 [*sh*](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/sh.html) 工具和 [*system*()](https://pubs.opengroup.org/onlinepubs/9699919799/functions/system.html)/[*popen*()](https://pubs.opengroup.org/onlinepubs/9699919799/functions/popen.html) 函数中使用命令语言语法。

`shell` 操作大体分为下面7种:

1. The shell reads its input from a file (see [*sh*](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/sh.html)), from the **-c** option or from the [*system*()](https://pubs.opengroup.org/onlinepubs/9699919799/functions/system.html) and [*popen*()](https://pubs.opengroup.org/onlinepubs/9699919799/functions/popen.html) functions defined in the System Interfaces volume of POSIX.1-2017. If the first line of a file of shell commands starts with the characters "#!", the results are unspecified.

2. `shell` 程序将输入内容分割成`tokens`: `word` 和 `operator`; 

3. The shell parses the input into simple commands (see [Simple Commands](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_09_01)) and compound commands (see [Compound Commands](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_09_04)).

4. The shell performs various expansions (separately) on different parts of each command, resulting in a list of pathnames and fields to be treated as a command and arguments; see [*wordexp*](https://pubs.opengroup.org/onlinepubs/9699919799/functions/wordexp.html#).

5. The shell performs redirection (see [Redirection](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_07)) and removes redirection operators and their operands from the parameter list.

6. The shell executes a function (see [Function Definition Command](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_09_05)), built-in (see [Special Built-In Utilities](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_14)), executable file, or script, giving the names of the arguments as positional parameters numbered 1 to *n*, and the name of the command (or in the case of a function within a script, the name of the script) as the positional parameter numbered 0 (see [Command Search and Execution](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_09_01_01)).

7. The shell optionally waits for the command to complete and collects the exit status (see [Exit Status for Commands](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_08_02)).
