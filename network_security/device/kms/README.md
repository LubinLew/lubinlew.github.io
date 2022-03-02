# 密钥管理服务(KMS)

密钥管理服务（Key Management Service，KMS）是一款安全管理类服务，使用经过第三方认证的硬件安全模块 HSM（Hardware Security Module）来生成和保护密钥。帮助用户轻松创建和管理密钥，满足用户多应用多业务的密钥管理需求，符合监管和合规要求。



## HSM

硬件安全模块（**HSM**,hardware security module）是一种用于保障和管理强认证系统所使用的数字密钥，并同时提供相关密码学操作的计算机硬件设备。硬件安全模块一般通过扩展卡或外部设备的形式直接连接到电脑或网络服务器。

HSM 在国内需要国家密码局认证, 国外需要 [FIPS-140-2](https://csrc.nist.gov/publications/detail/fips/140/2/final)(Security Requirements for Cryptographic Modules)认证。



## CMK

用户主密钥（Customer Master Key，CMK）是由腾讯云为您保管的主密钥，主密钥受到经过第三方认证硬件安全模块（HSM）的保护，通过它来加解密业务使用到的密码、证书、数据密钥等敏感数据，可以通过控制台和 API 来创建和管理主密钥。  
用户主密钥包括用户密钥和云产品密钥。

用户主密钥（CMK）是KMS的基本资源，由密钥ID、基本元数据（如密钥状态等）以及用于加密、解密数据的密钥材料组成。默认情况下，当您创建CMK时，会由KMS生成密钥材料。您也可以选择创建密钥材料来源为外部的密钥，此时，KMS将不会为您创建的CMK生成密钥材料，您可以将自己的密钥材料导入到CMK中。

## BYOK

实施 BYOK（Bring Your Own Key）方案，可通过 KMS 服务生成一个密钥材料为空的 CMK，并将自己的密钥材料导入到该用户主密钥中，形成一个外部密钥（EXTERNAL CMK），再由 KMS 服务进行该外部密钥的分发管理。



## 数据加密密钥

数据加密密钥 （Data Encryption Key，DEK）用于加密业务数据的密钥，受主密钥保护，可以自定义，也可以通过腾讯云 [密钥管理系统](https://cloud.tencent.com/document/product/573)（KMS）的 API 来创建新的数据密钥。

## 信封加密

信封加密（Envelope Encryption）是一种应对海量数据的高性能加解密方案，加解密业务数据时使用数据密钥，并采取性能较高的对称加密方法，再通过密钥管理服务来保证数据密钥的使用安全，特点是在保证数据安全的同时保持较高的数据读写性能。



[亚马逊 KMS](https://aws.amazon.com/cn/kms/)

[腾讯云 KMS](https://intl.cloud.tencent.com/zh/products/kms)

[密钥管理系统 - 文档中心 - 腾讯云](https://cloud.tencent.com/document/product/573)


