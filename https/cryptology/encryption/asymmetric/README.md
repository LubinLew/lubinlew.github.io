# 非对称加密

公钥密码体制根据其所依据的难题一般分为三类：

- 大素数分解问题类、
- 离散对数问题类
- 椭圆曲线类。

有时也把椭圆曲线类归为离散对数类。

### PKCS

PKCS 标准一览

| Standard | Version | Name                                                                                                                               | Comments                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
| -------- | ------- | ---------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| PKCS #1  | 2.2     | RSA Cryptography Standard                                                                                                          | See [RFC 8017](https://datatracker.ietf.org/doc/html/rfc8017). Defines the mathematical properties and format of RSA public and private keys ([ASN.1](https://en.wikipedia.org/wiki/ASN.1 "ASN.1")-encoded in clear-text), and the basic algorithms and encoding/padding schemes for performing RSA encryption, decryption, and producing and verifying signatures.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        |
| PKCS #2  | -       | Withdrawn                                                                                                                          | No longer active as of 2010. Covered RSA encryption of message digests; subsequently merged into PKCS #1.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |
| PKCS #3  | 1.4     | [Diffie–Hellman Key Agreement](https://en.wikipedia.org/wiki/Diffie%E2%80%93Hellman_key_exchange) Standard                         | A cryptographic protocol that allows two parties that have no prior knowledge of each other to jointly establish a shared secret key over an insecure communications channel.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
| PKCS #4  | -       | *Withdrawn*                                                                                                                        | No longer active as of 2010. Covered RSA key syntax; subsequently merged into PKCS #1.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |
| PKCS #5  | 2.1     | Password-based Encryption Standard                                                                                                 | See RFC 8018 and [PBKDF2](https://en.wikipedia.org/wiki/PBKDF2 "PBKDF2").                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |
| PKCS #6  | 1.5     | Extended-Certificate Syntax Standard                                                                                               | Defines extensions to the old v1 [X.509](https://en.wikipedia.org/wiki/X.509 "X.509") certificate specification. Obsoleted by v3 of the same.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
| PKCS #7  | 1.5     | [Cryptographic Message Syntax](https://en.wikipedia.org/wiki/Cryptographic_Message_Syntax "Cryptographic Message Syntax") Standard | See RFC 2315. Used to sign and/or encrypt messages under a [PKI](https://en.wikipedia.org/wiki/Public_key_infrastructure "Public key infrastructure"). Used also for certificate dissemination (for instance as a response to a PKCS #10 message). Formed the basis for [S/MIME](https://en.wikipedia.org/wiki/S/MIME "S/MIME"), which is as of 2010 based on RFC 5652, an updated [Cryptographic Message Syntax Standard](https://en.wikipedia.org/wiki/Cryptographic_Message_Syntax) (CMS). Often used for [single sign-on](https://en.wikipedia.org/wiki/Single_sign-on "Single sign-on").                                                                                                                                                                                                                                                                                              |
| PKCS #8  | 1.2     | Private-Key Information Syntax Standard                                                                                            | See RFC 5958. Used to carry private certificate keypairs (encrypted or unencrypted).                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |
| PKCS #9  | 2.0     | Selected Attribute Types                                                                                                           | See RFC 2985. Defines selected attribute types for use in PKCS #6 extended certificates, PKCS #7 digitally signed messages, PKCS #8 private-key information, and PKCS #10 certificate-signing requests.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |
| PKCS #10 | 1.7     | Certification Request Standard                                                                                                     | See RFC 2986. Format of messages sent to a [certification authority](https://en.wikipedia.org/wiki/Certification_authority "Diffie–Hellman key exchange") to request certification of a public key. See [certificate signing request](https://en.wikipedia.org/wiki/Certificate_signing_request "Certificate signing request").                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
| PKCS #11 | 3.0     | Cryptographic Token Interface                                                                                                      | Also known as "Cryptoki". An [API](https://en.wikipedia.org/wiki/API "API") defining a generic interface to [cryptographic tokens](https://en.wikipedia.org/wiki/Cryptographic_token "Elliptic curve cryptography") (see also [hardware security module](https://en.wikipedia.org/wiki/Hardware_security_module "Hardware security module")). Often used in [single sign-on](https://en.wikipedia.org/wiki/Single_sign-on "Standards organization"), [public-key cryptography](https://en.wikipedia.org/wiki/Public-key_cryptography "Public-key cryptography") and [disk encryption](https://en.wikipedia.org/wiki/Disk_encryption "Disk encryption")[[10]](https://en.wikipedia.org/wiki/PKCS#cite_note-10) systems. RSA Security has turned over further development of the PKCS #11 standard to the [OASIS PKCS 11 Technical Committee](http://www.oasis-open.org/committees/pkcs11/). |
| PKCS #12 | 1.1     | Personal Information Exchange Syntax Standard                                                                                      | See RFC 7292. Defines a file format commonly used to store [private keys](https://en.wikipedia.org/wiki/Private_key) with accompanying [public key certificates](https://en.wikipedia.org/wiki/Public_key_certificate "Public key certificate"), protected with a password-based [symmetric key](https://en.wikipedia.org/wiki/Symmetric_key "Symmetric key"). PFX is a predecessor to PKCS #12.This container format can contain multiple embedded objects, such as multiple certificates. Usually protected/encrypted with a password. Usable as a format for the [Java key store](https://en.wikipedia.org/wiki/Java_key_store "Java key store") and to establish client authentication certificates in Mozilla Firefox. Usable by [Apache Tomcat](https://en.wikipedia.org/wiki/Apache_Tomcat "Apache Tomcat").                                                                        |
| PKCS #13 | –       | [Elliptic Curve Cryptography](https://en.wikipedia.org/wiki/Elliptic_curve_cryptography) Standard                                  | *(Apparently abandoned, only reference is a proposal from 1998.)*[[12]](https://en.wikipedia.org/wiki/PKCS#cite_note-12)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
| PKCS #14 | –       | [Pseudo-random Number Generation](https://en.wikipedia.org/wiki/Pseudorandom_number_generator "Cryptographic token")               | *(Apparently abandoned, no documents exist.)*                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
| PKCS #15 | 1.1     | Cryptographic Token Information Format Standard                                                                                    | Defines a standard allowing users of [cryptographic tokens](https://en.wikipedia.org/wiki/Cryptographic_token "Cryptographic token") to identify themselves to applications, independent of the application's Cryptoki implementation (PKCS #11) or other [API](https://en.wikipedia.org/wiki/API "API"). RSA has relinquished IC-card-related parts of this standard to ISO/IEC 7816-15.[[14]](https://en.wikipedia.org/wiki/PKCS#cite_note-14)                                                                                                                                                                                                                                                                                                                                                                                                                                           |

# 