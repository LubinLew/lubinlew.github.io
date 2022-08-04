# SSH

SSH(Secure Shell)协议是一种网路加密协议,用于在非安全网络环境中安全的操作网络服务,最常见的应用是远程登录和命令执行。
其目的是替换 telnet 等不安全(明文传输)的协议。


## 历史

SSH 诞生于1955年，发明者是芬兰赫尔辛基科技大学的研究员`塔图·伊洛宁`。
他发现学校内部网络中可能发生密码嗅探攻击, 
原因是`Telnet`/`rsh`(Berkeley Remote Shell)/`rlogin & rexec` 等协议都是明文传输身份验证等信息。
于是他设计了最初版本的SSH协议(被称为SSH-1)用于替换这些不安全的协议。在当年7月就发布了开源实现并迅速走红，
并在当年12月创建了SSH 通信安全组织，负载开发和销售SSH工具，之后发布的版本脱离开源转向商业化。

1999年，社区希望能获取免费版本的SSH软件，于是重启了SSH开源软件的开发。
OpenBSD 的开发者们基于之前的开源程序创建了 [OpenSSH](https://github.com/openssh/openssh-portable) 项目。
到了2005年，OpenSSH 已经成为最受欢迎的SSH实现。

2006年，IETF 组织发布了 SSH-2 标准，这个标准与 SSH-1 不兼容。
由于 SSH-2 更加安全并具备众多新特性，很快得到实现和推广。
例如 [libssh](https://gitlab.com/libssh/libssh-mirror/), [Lsh](https://git.lysator.liu.se/lsh/lsh) 和 [Dropbear](https://github.com/mkj/dropbear) 只支持 SSH-2，OpenSSH 也在7.6版本删除了SSH-1的实现。

## 身份验证：OpenSSH 密钥管理

在类 Unix 系统上，授权公钥列表通常存储在允许远程登录的用户主(home)目录下的文件 `~/.ssh/authorized_keys`.
仅当除所有者和root以外的任何人都不能写入此文件时，SSH 才会尊重该文件。
当远程端存在公钥并且本地端存在匹配的私钥时，不再需要输入密码。
但是，为了提高安全性，可以使用密码锁定私钥本身。

也可以在标准位置查找私钥，并且可以将其完整路径指定为命令行设置（例如 ssh -i /path/to/priavekey user@host）。 
ssh-keygen 实用程序总是成对生成公钥和私钥。

SSH 还支持由自动生成的密钥加密的基于密码的身份验证。
在这种情况下，攻击者可以模仿合法的服务器端，询问密码并获取密码（中间人攻击）。
但是，这只有在双方以前从未进行过身份验证的情况下才有可能，因为 SSH 会记住服务器端之前使用的密钥(`~/.ssh/known_hosts`)。 
SSH 客户端在接受以前未知的新服务器的密钥之前会发出警告。可以从服务器端禁用密码验证。

-----

## 使用

SSH 通常用于登录远程机器并执行命令，但它也支持隧道、转发 TCP 端口和 X11 连接；
它可以使用相关的 SSH 文件传输 (SFTP) 或安全复制 (SCP) 协议来传输文件。

SSH 应用基于CS(服务端/服务端)架构。
SSH 客户端程序通常用于建立与接受远程连接的 SSH 守护程序的连接。
两者通常存在于大多数现代操作系统上，包括 macOS、大多数 Linux 发行版、OpenBSD、FreeBSD、NetBSD、Solaris 和 OpenVMS。
值得注意的是，Windows 10 从 1709  版本之前支持 SSH。
类 UNIX 系统（例如 Konqueror）的文件管理器可以使用 FISH 协议提供具有拖放功能的拆分窗格 GUI。
开源 Windows 程序 WinSCP 使用 PuTTY 作为后端提供了类似的文件管理（同步、复制、远程删除）能力。 

SSH 在云计算中解决连接问题很重要，避免了将基于云的虚拟机直接暴露在 Internet 上的安全问题。 
SSH 隧道可以在 Internet 上提供安全路径，通过防火墙到达虚拟机。

IANA 已为此协议分配了 TCP 端口 22、UDP 端口 22 和 SCTP 端口 22。
IANA 早在 2001 年就将 SSH 服务器的标准 TCP 端口 22 列为众所周知的端口之一。
SSH 也可以使用 SCTP 而不是 TCP 作为面向连接的传输层协议来运行。

----

## 文件传输协议

SSH协议用于多种文件传输机制。

- SCP(Secure copy), which evolved from RCP protocol over SSH
- rsync, intended to be more efficient than SCP. Generally runs over an SSH connection.
- SFTP(SSH File Transfer Protocol), a secure alternative to FTP (not to be confused with FTP over SSH or FTPS)
- FISH(Files transferred over shell protocol), released in 1998, which evolved from Unix shell commands over SSH
- FASP/Aspera(Fast and Secure Protocol), uses SSH for control and UDP ports for data transfer.

---- 

## 架构

SSH 作为一个分层协议套件运行，包括三个主要的分层组件：

- 传输层(transport layer)提供服务器身份验证、机密性和完整性 
- 用户认证协议(user authentication protocol)验证访问服务器的用户
- 连接协议(connection protocol)将加密隧道复用为多个逻辑通信通道

SSH 使用公钥加密来验证远程计算机并允许它在必要时验证用户。

SSH 可用于多种方法。以最简单的方式，通信通道的两端使用自动生成的公私密钥对来加密网络连接，然后使用密码对用户进行身份验证。

当公私密钥对由用户手动生成时，本质上是在创建密钥对时进行认证，然后可以自动打开会话而无需密码提示。在这种情况下，公钥放置在必须允许访问匹配私钥所有者的所有计算机上，所有者将其保密。虽然身份验证是基于私钥的，但在身份验证期间从不通过网络传输密钥。 SSH 仅验证提供公钥的同一个人是否也拥有匹配的私钥。

在所有版本的 SSH 中，验证未知公钥很重要，即将公钥与身份相关联，然后再接受它们为有效。未经验证接受攻击者的公钥将授权未经授权的攻击者作为有效用户。


SSH 协议具有分层架构，包含三个独立的组件：

传输层 (RFC 4253) 通常使用 TCP/IP 的传输控制协议 (TCP)，保留端口号 22 作为服务器侦听端口。
该层处理初始密钥交换以及服务器身份验证，并设置加密、压缩和完整性验证。 
它向上层公开了一个用于发送和接收明文数据包的接口，每个数据包的大小最多为 32,768 字节，但每个实现可以允许更多。
传输层还安排重新交换密钥，通常在传输 1 GB 数据后或经过一小时后，以先发生者为准。

用户身份验证层 (RFC 4252) 处理客户端身份验证，并提供一套身份验证算法。
身份验证是客户端驱动的：当提示输入密码时，可能是 SSH 客户端提示，而不是服务器。服务器仅响应客户端的身份验证请求。
广泛使用的用户身份验证方法包括：

- 密码：一种简单的密码验证方法，包括允许更改密码的工具。并非所有程序都实现此方法。
- 公钥：一种基于公钥的身份验证方法，通常至少支持 DSA、ECDSA 或 RSA 密钥对，其他实现也支持 X.509 证书。
- 键盘交互(RFC 4256)：一种通用方法，服务器发送一个或多个提示以输入信息，客户端显示它们并返回由用户键入的响应。用于提供一次性密码验证，例如 S/Key 或 SecurID。当 PAM 是底层主机身份验证提供程序时，某些 OpenSSH 配置使用它来有效地提供密码身份验证，有时会导致无法使用仅支持纯密码身份验证方法的客户端登录。
- GSSAPI: 该方法提供了一种可扩展的方案，以使用 Kerberos 5 或 NTLM 等外部机制执行 SSH 身份验证，从而为 SSH 会话提供单点登录功能。这些方法通常由用于组织的商业 SSH 实现来实现，尽管 OpenSSH 确实有一个有效的 GSSAPI 实现。

连接层 (RFC 4254) 定义了通道、通道请求和全局请求的概念，它们定义了所提供的 SSH 服务。
单个 SSH 连接可以同时多路复用到多个逻辑通道，每个通道都双向传输数据。 
通道请求用于中继带外通道特定的数据，例如终端窗口大小的变化，或服务器端进程的退出代码。 
此外，每个通道使用接收窗口大小执行自己的流量控制。 SSH 客户端使用全局请求请求要转发的服务器端端口。 
标准频道类型包括：

- 用于终端 shell、SFTP 和 exec 请求的 shell（包括 SCP 传输）
- direct-tcpip 用于客户端到服务器的转发连接
- forwarded-tcpip 用于服务器到客户端的转发连接

SSHFP DNS 记录 (RFC 4255) 提供公共主机密钥指纹，以帮助验证主机的真实性。

这种开放式架构提供了相当大的灵活性，允许将 SSH 用于安全外壳之外的各种目的。
仅传输层的功能就可以与传输层安全性 (TLS) 相媲美； 
用户身份验证层可通过自定义身份验证方法进行高度扩展；
并且连接层提供了将许多辅助会话多路复用到单个 SSH 连接的能力，该功能可与 BEEP 相媲美，但在 TLS 中不可用。

----

## 算法

- EdDSA, ECDSA, RSA and DSA for public-key cryptography
- ECDH and Diffie–Hellman for key exchange
- HMAC, AEAD and UMAC for MAC
- AES (and deprecated RC4, 3DES, DES) for symmetric encryption
- AES-GCM and ChaCha20-Poly1305 for AEAD encryption
- SHA (and deprecated MD5) for key fingerprint

----

## 标准文档

The following RFC publications by the IETF "secsh" working group document SSH-2 as a proposed Internet standard.

- RFC 4250 – The Secure Shell (SSH) Protocol Assigned Numbers
- RFC 4251 – The Secure Shell (SSH) Protocol Architecture
- RFC 4252 – The Secure Shell (SSH) Authentication Protocol
- RFC 4253 – The Secure Shell (SSH) Transport Layer Protocol
- RFC 4254 – The Secure Shell (SSH) Connection Protocol
- RFC 4255 – Using DNS to Securely Publish Secure Shell (SSH) Key Fingerprints
- RFC 4256 – Generic Message Exchange Authentication for the Secure Shell Protocol (SSH)
- RFC 4335 – The Secure Shell (SSH) Session Channel Break Extension
- RFC 4344 – The Secure Shell (SSH) Transport Layer Encryption Modes
- RFC 4345 – Improved Arcfour Modes for the Secure Shell (SSH) Transport Layer Protocol

The protocol specifications were later updated by the following publications:

- RFC 4419 – Diffie-Hellman Group Exchange for the Secure Shell (SSH) Transport Layer Protocol (March 2006)
- RFC 4432 – RSA Key Exchange for the Secure Shell (SSH) Transport Layer Protocol (March 2006)
- RFC 4462 – Generic Security Service Application Program Interface (GSS-API) Authentication and Key Exchange for the Secure Shell (SSH) Protocol (May 2006)
- RFC 4716 – The Secure Shell (SSH) Public Key File Format (November 2006)
- RFC 4819 – Secure Shell Public Key Subsystem (March 2007)
- RFC 5647 – AES Galois Counter Mode for the Secure Shell Transport Layer Protocol (August 2009)
- RFC 5656 – Elliptic Curve Algorithm Integration in the Secure Shell Transport Layer (December 2009)
- RFC 6187 – X.509v3 Certificates for Secure Shell Authentication (March 2011)
- RFC 6239 – Suite B Cryptographic Suites for Secure Shell (SSH) (May 2011)
- RFC 6594 – Use of the SHA-256 Algorithm with RSA, Digital Signature Algorithm (DSA), and Elliptic Curve DSA (ECDSA) in SSHFP Resource Records (April 2012)
- RFC 6668 – SHA-2 Data Integrity Verification for the Secure Shell (SSH) Transport Layer Protocol (July 2012)
- RFC 7479 – Ed25519 SSHFP Resource Records (March 2015)
- RFC 5592 – Secure Shell Transport Model for the Simple Network Management Protocol (SNMP) (June 2009)
- RFC 6242 – Using the NETCONF Protocol over Secure Shell (SSH) (June 2011)
- RFC 8332 – Use of RSA Keys with SHA-256 and SHA-512 in the Secure Shell (SSH) Protocol (March 2018)
- draft-gerhards-syslog-transport-ssh-00 – SSH transport mapping for SYSLOG (July 2006)
- draft-ietf-secsh-filexfer-13 – SSH File Transfer Protocol (July 2006)

In addition, the OpenSSH project includes several vendor protocol specifications/extensions:

- OpenSSH PROTOCOL overview
- OpenSSH certificate/key overview
- draft-miller-ssh-agent-04 - SSH Agent Protocol (December 2019)

---- 

**开源桌面终端工具**

[PuTTY](https://git.tartarus.org/?p=simon/putty.git;a=summary) - PuTTY is an SSH and telnet client

[FluentTerminal](https://github.com/felixse/FluentTerminal) - A Terminal Emulator based on UWP(C#) and web technologies

[hyper](https://github.com/vercel/hyper) - A terminal built on web technologies(electron)

[Alacritty](https://github.com/alacritty/alacritty) - A fast, cross-platform, OpenGL terminal emulator

**库**

[libssh2](https://github.com/libssh2/libssh2)