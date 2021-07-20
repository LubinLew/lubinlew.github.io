# RSA 数字签名加解密

> https://www.openssl.org/docs/man1.0.2/man3/RSA_private_encrypt.html

```c
#include <openssl/rsa.h>

int RSA_private_encrypt(int flen, const unsigned char *from,
    unsigned char *to, RSA *rsa, int padding);

int RSA_public_decrypt(int flen, const unsigned char *from,
    unsigned char *to, RSA *rsa, int padding);
```

### 例子

> [lzhaoruoxu/lochsemusamples/MyCrypto/MyCrypto/rsatest.cpp · GitHub](https://github.com/zhaoruoxu/lochsemusamples/blob/master/MyCrypto/MyCrypto/rsatest.cpp)

先生成测试用的 RSA 密钥对

```bash
# 生成 RSA 私钥（2048位）
openssl genrsa -out privatekey.pem 2048
# 生成 RSA 公钥
openssl rsa -in privatekey.pem -pubout -out publickey.pem
```

#### 私钥加密

```c
/* for Linux */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <openssl/pem.h>
#include <openssl/err.h>


#define  RSA_PRIVATE_KEY_FILE "privatekey.pem"
#define  RSA_ENCRYPT_OUT_FILE "rsa_encrypt.dat"
#define  RSA_TEXT "hello world"

int main(int argc, const char *argv[])
{
    RSA  *key = NULL;
    FILE *fp = NULL;

    int   rsa_len = 0;
    unsigned char *outbuf = NULL;


    /* OpenSSL 初始化 */
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    printf("encrypt string: %s\n", RSA_TEXT);

    /* 加载私钥文件 */
    fp = fopen(RSA_PRIVATE_KEY_FILE, "r");   assert(fp != NULL);
    key = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
    if (NULL == key) {
        printf("PEM_read_RSAPrivateKey failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    /* 申请加密后的存储空间 */
    rsa_len = RSA_size(key);
    printf("=== RSA_size() return %d\n", rsa_len);
    outbuf = calloc(1, rsa_len);  assert(outbuf != NULL);

    /* 使用 privatekey.pem 私钥加密   RSA_TEXT */
    if (RSA_private_encrypt(strlen(RSA_TEXT), (const unsigned char *)RSA_TEXT, outbuf, key, RSA_PKCS1_PADDING) == -1) {
        printf("RSA_private_encrypt failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
        exit(EXIT_FAILURE);
    }

    /* 将结果写入文件 */
    fp = fopen(RSA_ENCRYPT_OUT_FILE, "wb");  assert(fp != NULL);
    fwrite(outbuf, 1, rsa_len, fp);
    fclose(fp);
    printf("=== result write in file %s\n", RSA_ENCRYPT_OUT_FILE);

    /* cleanup */
    free(outbuf);
    RSA_free(key);
    printf("=== SUCCESS !!!\n");
    exit(EXIT_SUCCESS);
}
```

#### 公钥解密

```c
/* for Linux */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <openssl/pem.h>
#include <openssl/err.h>


#define  RSA_PUBLIC_KEY_FILE  "publickey.pem"
#define  RSA_ENCRYPT_OUT_FILE "rsa_encrypt.dat"

int main(int argc, const char *argv[])
{
    RSA  *key = NULL;
    FILE *fp = NULL;

    int   rsa_len = 0;
    unsigned char *rsabuf = NULL;
    unsigned char *outbuf = NULL;


    /* OpenSSL 初始化 */
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    /* 加载公钥文件 */
    fp = fopen(RSA_PUBLIC_KEY_FILE, "r");   assert(fp != NULL);
    key = PEM_read_RSA_PUBKEY(fp, NULL, NULL, NULL);
    if (NULL == key) {
        printf("PEM_read_RSAPublicKey failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    /* 申请解密后的存储空间 */
    rsa_len = RSA_size(key);
    printf("=== RSA_size() return %d\n", rsa_len);
    outbuf = calloc(1, rsa_len);             assert(outbuf != NULL);

    /* 读取加密后的文件(RSA_ENCRYPT_OUT_FILE)内容 */
    fp = fopen(RSA_ENCRYPT_OUT_FILE, "rb");  assert(fp != NULL);
    rsabuf = calloc(1, rsa_len);             assert(outbuf != NULL);
    fread(rsabuf, 1, rsa_len, fp);
    fclose(fp);

    /* 使用公钥解密    RSA_ENCRYPT_OUT_FILE */
    if (RSA_public_decrypt(rsa_len, rsabuf, outbuf, key, RSA_PKCS1_PADDING) == -1) {
        printf("RSA_public_decrypt failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
        exit(EXIT_FAILURE);
    }
    printf("=== outbuf: %s\n", outbuf);

    /* cleanup */
    free(rsabuf);
    free(outbuf);
    RSA_free(key);
    printf("=== SUCCESS !!!\n");
    exit(EXIT_SUCCESS);

}
```