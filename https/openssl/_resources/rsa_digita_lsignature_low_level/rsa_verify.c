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

