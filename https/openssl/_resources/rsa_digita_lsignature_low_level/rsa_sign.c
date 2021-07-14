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

