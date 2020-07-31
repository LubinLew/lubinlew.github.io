# 前言

开启 Hyperscan 学习之旅, 由于英文不好, 翻译不好的地方请指正



## 名词

| 英文                      | 中文    | 备注  |
| ----------------------- | ----- | --- |
| RE / Regular Expression | 正则表达式 |     |
| PCRE                    |       |     |
| pattern                 | 模式    |     |



## 概述

Hyperscan 是一个高性能的正则表达式匹配引擎 , the
ability to match multiple expressions simultaneously and flexibility in
scanning operation.

Patterns are provided to a compilation interface which generates an immutable
pattern database. The scan interface then can be used to scan a target data
buffer for the given patterns, returning any matching results from that data
buffer. Hyperscan also provides a streaming mode, in which matches that span
several blocks in a stream are detected.

This document is designed to facilitate code-level integration of the Hyperscan
library with existing or new applications.

[Introduction](http://intel.github.io/hyperscan/dev-reference/intro.html#intro) is a short overview of the Hyperscan library, with more detail on
the Hyperscan API provided in the subsequent sections: [Compiling Patterns](http://intel.github.io/hyperscan/dev-reference/compilation.html#compilation) and [Scanning for Patterns](http://intel.github.io/hyperscan/dev-reference/runtime.html#runtime).

[Performance Considerations](http://intel.github.io/hyperscan/dev-reference/performance.html#perf) provides details on various factors which may impact the
performance of a Hyperscan integration.

[API Reference: Constants](http://intel.github.io/hyperscan/dev-reference/api_constants.html#api-constants) and [API Reference: Files](http://intel.github.io/hyperscan/dev-reference/api_files.html#api-files) provides a detailed summary of the
Hyperscan Application Programming Interface (API).
