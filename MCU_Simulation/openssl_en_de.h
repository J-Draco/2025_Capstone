// openssl_en_de.h
#ifndef OPENSSL_EN_DE_H
#define OPENSSL_EN_DE_H

void aes_encrypt_openssl(unsigned char*, unsigned char*, unsigned char*, int);
void aes_decrypt_openssl(unsigned char*, unsigned char*, unsigned char*, int);
void print_aes_result(const char*, const unsigned char*, int);

#endif
