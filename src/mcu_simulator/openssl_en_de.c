#include <openssl/evp.h>
#include <openssl/aes.h>
#include <string.h>

void aes_encrypt_openssl(unsigned char *input,unsigned char *key,unsigned char *output,int len){
    EVP_CIPHER_CTX *ctx;
    int outlen, tmplen;

    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_ecb(), NULL, key, NULL);

    EVP_CIPHER_CTX_set_padding(ctx, 0);  // 패딩 비활성화 (입력은 16바이트 블록 정렬 필요)

    EVP_EncryptUpdate(ctx, output, &outlen, input, len);
    EVP_EncryptFinal_ex(ctx, output + outlen, &tmplen);

    EVP_CIPHER_CTX_free(ctx);
}
void aes_decrypt_openssl(unsigned char *input,unsigned char *key,unsigned char *output,int len ){
    EVP_CIPHER_CTX *ctx;
    int outlen, tmplen;

    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_ecb(), NULL, key, NULL);

    EVP_CIPHER_CTX_set_padding(ctx, 0);

    EVP_DecryptUpdate(ctx, output, &outlen, input, len);
    EVP_DecryptFinal_ex(ctx, output + outlen, &tmplen);

    EVP_CIPHER_CTX_free(ctx);
}
void print_aes_result(const char *label, const unsigned char *data, int len) {
    printf("%s (%d bytes): ", label, len);
    for (int i = 0; i < len; i++) {
        printf("%02X", data[i]);
    }
    printf("\n");
}
