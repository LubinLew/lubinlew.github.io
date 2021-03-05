# 前言

Hyperscan是一个旨在提供高性能，多模匹配(同时匹配多个正则表达式)能力 和 扫描操作 的正则表达式引擎。由于其出色的性能和较低的系统占用率, 现在广泛用于DPI/IPS/IDS/FW等安全产品中。

## 名词

| 英文                                        | 中文           | 备注           |
| ----------------------------------------- | ------------ | ------------ |
| RE / Regular Expression                   | 正则表达式        |              |
| PCRE(Perl Compatible Regular Expressions) | Perl兼容的正则表达式 |              |
| meta character                            | 元字符          | 指正则表达式中的特殊字符 |
| `pattern`                                 | 模式           |              |

## 内容

本文档提供了有关 Hyperscan API 的详细信息，旨在促进Hyperscan库与应用程序的代码级集成。
