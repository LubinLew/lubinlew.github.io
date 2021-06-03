# TLS v1.3

## 4.1. Key Exchange Messages

   The key exchange messages are used to determine the security
   capabilities of the client and the server and to establish shared
   secrets, including the traffic keys used to protect the rest of the
   handshake and the data.

### 4.1.1. 秘钥协商(Cryptographic Negotiation)

#### TLSv1.3 之前的秘钥协商方式

这里简单的描述下 TLSv1.3 之前协议的秘钥交换流程，以及其缺点.

##### RSA 秘钥协商流程

- client 发送 `ClientHello` 消息
- server 回复 `Certificate` 消息 (client 获取公钥)
- client 使用证书中的公钥，加密预主秘钥，通过 `ClientKeyExchange` 消息发给 server
- server 提取出预主秘钥，计算出主秘钥，然后发送使用主秘钥加密的 `Finished` 消息
- client 计算主秘钥，验证 `Finished` ，验证成功后，就可以发送 `Application Data` 消息了

![RSA handshake](_sources/tls_rsa_handshake.png)

**缺点**：

RSA秘钥交换不是前向安全算法（证书对应私钥泄漏后，之前抓包的报文都能被解密）。
所以在 TLS 1.3中 RSA 已经废弃了。

##### ECDHE 秘钥协商流程

- client 发送 `ClientHello` 消息，附带客户端支持的椭圆曲线类型。
- server 回复 `ServerHello` 和 `Certificate` 等；
    server 选择的椭圆曲线参数，然后生成私钥（BIGNUM），乘以椭圆曲线的base point得到公钥（POINT），
    顺便签个名表示自己拥有证书，然后将报文发给client，报文就是`ServerKeyExchange`，
    其包含了server选择的椭圆曲线参数、自己根据这个参数计算的公钥、自己用证书的私钥对当前报文的签名。
- client 收到 Server Key Exchange，获得椭圆曲线参数，生成私钥（BIGNUM）后计算公钥（POINT），
    然后把公钥发出去`ClientKeyExchange`。client使用自己的私钥（BIGNUM）和server的公钥（POINT）计算出主秘钥。
- server 收到 client的公钥（POINT），使用自己的私钥（BIGNUM），计算主秘钥。两端主秘钥是一致。

![ECDHE handshake](_sources/tls_ecdh_handshake.png)

**缺点**：

client发送自己支持的椭圆曲线类型，然后等待server选择后，才计算自己的公钥然后发送给server。这个可以优化。

#### TLSv1.3 的PSK秘钥协商方式

- client 发送请求（Client Hello），extension携带支持的椭圆曲线类型。且对每个自己支持的椭圆曲线类型计算公钥（POINT）。公钥放在extension中的keyshare中。
- server 回复 Server Hello和certificate等；server选择的椭圆曲线参数，然后乘以椭圆曲线的base point得到公钥（POINT）。然后提取Client Hello中的key_share拓展中对应的公钥，计算主秘钥。公钥（POINT）不再和之前的以协议一样放在Server Key Exchange中，而是放在Server Hello的key_share拓展中。client收到server的公钥（POINT）后计算主秘钥。

![PSK handshake](_sources/tls_psk_handshake.png)

秘钥协商功能由 `ClientHello` 消息中的 4 组选项提供:

- 客户端支持的 AEAD algorithm/HKDF hash 算法列表

- 列出客户端支持的 (EC)DHE 组的 `supported_groups` (Section 4.2.7) 扩展,
  和包含一些或全部 (EC)DHE shares 的`key_share`(Section 4.2.8) 扩展

- 含有客户端支持的签名算法的扩展 `signature_algorithms` (Section 4.2.3),
  (可选)指定证书签名算法的扩展 `signature_algorithms_cert`" (Section 4.2.3) 

- 包含客户端已知的对称密钥身份(identities)列表 `pre_shared_key` (Section 4.2.11) 扩展,
  和用来指定秘钥交换模式的 `psk_key_exchange_modes` (Section 4.2.9) 扩展.
  “pre_shared_key”扩展包含客户端已知的对称密钥身份列表和“psk_key_exchange_modes”扩展，它指示可以与 PSK 一起使用的密钥交换模式。
  
  如果服务端不选择一个 PSK, 那么前三个选项时完全正交(orthogonal)的:
  服务端独立的选择一个密码套件、一个 (EC)DHE group 、 key share for key establishment,
  和 a signature algorithm/certificate pair to authenticate itself to the client.  
  
  如果客户端发送的 "supported_groups" 与服务端支持的 groups 没有重叠, 
  那么服务端必须终止握手,并发送 `handshake_failure` 或者 `insufficient_security` alert.
  
  If the server selects a PSK, then it MUST also select a key
  establishment mode from the set indicated by the client's
  "psk_key_exchange_modes" extension (at present, PSK alone or with
  (EC)DHE).  Note that if the PSK can be used without (EC)DHE, then
  non-overlap in the "supported_groups" parameters need not be fatal,
  as it is in the non-PSK case discussed in the previous paragraph.
  
  If the server selects an (EC)DHE group and the client did not offer a
  compatible "key_share" extension in the initial ClientHello, the
  server MUST respond with a HelloRetryRequest (Section 4.1.4) message.
  
  If the server successfully selects parameters and does not require a
  HelloRetryRequest, it indicates the selected parameters in the
  ServerHello as follows:

- If PSK is being used, then the server will send a "pre_shared_key"
  extension indicating the selected key.

- When (EC)DHE is in use, the server will also provide a "key_share"
  extension.  If PSK is not being used, then (EC)DHE and
  certificate-based authentication are always used.

- When authenticating via a certificate, the server will send the
  Certificate (Section 4.4.2) and CertificateVerify (Section 4.4.3)
  messages.  In TLS 1.3 as defined by this document, either a PSK or
  a certificate is always used, but not both.  Future documents may
  define how to use them together.
  
  If the server is unable to negotiate a supported set of parameters
  (i.e., there is no overlap between the client and server parameters),
  it MUST abort the handshake with either a "handshake_failure" or
  "insufficient_security" fatal alert (see Section 6).

### 4.2.8.  Key Share

   `key_share` 扩展包含端点(endpoint)的加密参数。

   Clients MAY send an empty client_shares vector in order to request
   group selection from the server, at the cost of an additional round
   trip (see Section 4.1.4).

```c
      struct {
          NamedGroup group;
          opaque key_exchange<1..2^16-1>;
      } KeyShareEntry;
```

   group:  The named group for the key being exchanged.

   key_exchange:  Key exchange information.  The contents of this field
      are determined by the specified group and its corresponding
      definition.  Finite Field Diffie-Hellman [DH76] parameters are
      described in Section 4.2.8.1; Elliptic Curve Diffie-Hellman
      parameters are described in Section 4.2.8.2.

   In the ClientHello message, the "extension_data" field of this
   extension contains a "KeyShareClientHello" value:

      struct {
          KeyShareEntry client_shares<0..2^16-1>;
      } KeyShareClientHello;

   client_shares:  A list of offered KeyShareEntry values in descending
      order of client preference.

   This vector MAY be empty if the client is requesting a
   HelloRetryRequest.  Each KeyShareEntry value MUST correspond to a
   group offered in the "supported_groups" extension and MUST appear in
   the same order.  However, the values MAY be a non-contiguous subset
   of the "supported_groups" extension and MAY omit the most preferred
   groups.  Such a situation could arise if the most preferred groups
   are new and unlikely to be supported in enough places to make
   pregenerating key shares for them efficient.

   Clients can offer as many KeyShareEntry values as the number of
   supported groups it is offering, each representing a single set of
   key exchange parameters.  For instance, a client might offer shares
   for several elliptic curves or multiple FFDHE groups.  The
   key_exchange values for each KeyShareEntry MUST be generated
   independently.  Clients MUST NOT offer multiple KeyShareEntry values
   for the same group.  Clients MUST NOT offer any KeyShareEntry values
   for groups not listed in the client's "supported_groups" extension.
   Servers MAY check for violations of these rules and abort the
   handshake with an "illegal_parameter" alert if one is violated.

   In a HelloRetryRequest message, the "extension_data" field of this
   extension contains a KeyShareHelloRetryRequest value:

      struct {
          NamedGroup selected_group;
      } KeyShareHelloRetryRequest;

   selected_group:  The mutually supported group the server intends to
      negotiate and is requesting a retried ClientHello/KeyShare for.

   Upon receipt of this extension in a HelloRetryRequest, the client
   MUST verify that (1) the selected_group field corresponds to a group
   which was provided in the "supported_groups" extension in the
   original ClientHello and (2) the selected_group field does not
   correspond to a group which was provided in the "key_share" extension
   in the original ClientHello.  If either of these checks fails, then
   the client MUST abort the handshake with an "illegal_parameter"
   alert.  Otherwise, when sending the new ClientHello, the client MUST

   replace the original "key_share" extension with one containing only a
   new KeyShareEntry for the group indicated in the selected_group field
   of the triggering HelloRetryRequest.

   In a ServerHello message, the "extension_data" field of this
   extension contains a KeyShareServerHello value:

      struct {
          KeyShareEntry server_share;
      } KeyShareServerHello;

   server_share:  A single KeyShareEntry value that is in the same group
      as one of the client's shares.

   If using (EC)DHE key establishment, servers offer exactly one
   KeyShareEntry in the ServerHello.  This value MUST be in the same
   group as the KeyShareEntry value offered by the client that the
   server has selected for the negotiated key exchange.  Servers
   MUST NOT send a KeyShareEntry for any group not indicated in the
   client's "supported_groups" extension and MUST NOT send a
   KeyShareEntry when using the "psk_ke" PskKeyExchangeMode.  If using
   (EC)DHE key establishment and a HelloRetryRequest containing a
   "key_share" extension was received by the client, the client MUST
   verify that the selected NamedGroup in the ServerHello is the same as
   that in the HelloRetryRequest.  If this check fails, the client MUST
   abort the handshake with an "illegal_parameter" alert.

### 4.2.9. PSK交换模式(Pre-Shared Key Exchange Modes)

   为了使用 PSKs, 客户端必需发送 `psk_key_exchange_modes` 扩展。
   这个扩展的意义是表明客户端只支持该扩展中列出的PSK模式。
   这会限制 `ClientHello` 和 `NewSessionTicket` 消息中PSK的使用。
   which restricts both the use of PSKs offered in this ClientHello and
   those which the server might supply via NewSessionTicket.

   如果客户端提供了 `pre_shared_key` 扩展, 那么必需提供 `psk_key_exchange_modes` 扩展， 否则服务端必须终止握手。
   服务端不能选择客户端没有列出的交换模式。
   该扩展会影响 PSK恢复(session复用)，服务端不能通过`NewSessionTicket`消息发送模式不兼容的 ticket .
   如果服务端这么做了,那么影响支持客户端尝试PSK恢复失败。
   Servers SHOULD NOT send NewSessionTicket with tickets that are not compatible with the advertised modes; 
   however, if a server does so, the impact will just be that the client's attempts at resumption fail.

   服务端不允许发送 `psk_key_exchange_modes` 扩展.

```c
enum { 
    psk_ke(0), 
    psk_dhe_ke(1), 
    (255)
} PskKeyExchangeMode;

struct {
    PskKeyExchangeMode ke_modes<1..255>;
} PskKeyExchangeModes;
```

   psk_ke:  PSK-only key establishment.  In this mode, the server MUST NOT supply a "key_share" value.

   psk_dhe_ke:  PSK with (EC)DHE key establishment.  In this mode, the client and server MUST supply "key_share" values as described in Section 4.2.8.

   Any future values that are allocated must ensure that the transmitted
   protocol messages unambiguously identify which mode was selected by
   the server; at present, this is indicated by the presence of the
   "key_share" in the `ServerHello`.

---

## TLS v1.3

https://www.rfc-editor.org/rfc/rfc8446.html

https://www.oschina.net/translate/rfc-8446-aka-tls-1-3?lang=chs

https://blog.csdn.net/u010129119/article/details/54090814

https://blog.csdn.net/mrpre/article/details/81532469

https://www.jianshu.com/p/efe44d4a7501

https://zhuanlan.zhihu.com/p/98142192
