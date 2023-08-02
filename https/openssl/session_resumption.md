# OpenSSL 会话复用

## 1. Session ID

```c
#include <openssl/ssl.h>

/* d2i_SSL_SESSION() transforms the external ASN1 representation of an SSL/TLS session, 
 * stored as binary data at location pp with length length, into an SSL_SESSION object.
 */
SSL_SESSION *d2i_SSL_SESSION(SSL_SESSION **a, const unsigned char **pp, long length);

/* i2d_SSL_SESSION() transforms the SSL_SESSION object in into the ASN1 representation and stores it into the memory location pointed to by pp. 
 * The length of the resulting ASN1 representation is returned. 
 * If pp is the NULL pointer, only the length is calculated and returned.
 */
int i2d_SSL_SESSION(SSL_SESSION *in, unsigned char **pp);
```


## 2. Session Ticket

```c
 #include <openssl/tls1.h>

 long SSL_CTX_set_tlsext_ticket_key_cb(SSL_CTX sslctx,
        int (*cb)(SSL *s, unsigned char key_name[16],
                  unsigned char iv[EVP_MAX_IV_LENGTH],
                  EVP_CIPHER_CTX *ctx, HMAC_CTX *hctx, int enc));

```


## 3. PSK(pre-shared key)

TLSv1.3 新增的