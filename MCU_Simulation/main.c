#include<stdio.h>
#include "generate_aes.c"
#include "sha256.h"
#include "simulate.c"
#include <pthread.h>
#include "communicate.c"
#include "global_data.h"


#include <openssl/evp.h>
#include <openssl/aes.h>
#include <string.h>
#include "openssl_en_de.h"


unsigned int Hashed_UID[8] = {0};
unsigned char encrypted_auth[48] = {0};
unsigned char key_bytes[32] = {0};
int main(void){
    /*unsigned char input[16] = {0};
    input[0] = (MCU_UID >> 24) & 0xFF; // 상위 바이트 (0일 가능성 높음)
    input[1] = (MCU_UID >> 16) & 0xFF; // AB
    input[2] = (MCU_UID >> 8)  & 0xFF; // CD
    input[3] = MCU_UID & 0xFF;*/
    pthread_t t1,t2,t3;
    pthread_create(&t1,NULL,(void*)timer_simulate,NULL);//타이머를 계속 실행해야하지만 무한루프이기떄문에 쓰레드를이용해 분리한다.
    pthread_create(&t2, NULL, receive_req_signal, NULL);
    pthread_create(&t3, NULL, fin_out, NULL);

    printf("MCU_UID : 0X%08X\n",MCU_UID);


    unsigned int random[8] = {0};
    unsigned int *AES_KEY;

    unsigned char decrypted_uid[48];
    unsigned char input[80]={0};
    sha256(Hashed_UID,uid_bytes,4);
    print_hash("Hashed_UID",Hashed_UID);
    int verification_result =0;


    while(!verification_result) {
        receive_pw();
        verification_result = verify_pw(Hashed_UID);
        send_verification(verification_result);
        for (int i = 0; i < 8; i++) {
            input[i * 4 + 0] = (hashed_auth[i] >> 24) & 0xFF;
            input[i * 4 + 1] = (hashed_auth[i] >> 16) & 0xFF;
            input[i * 4 + 2] = (hashed_auth[i] >> 8) & 0xFF;
            input[i * 4 + 3] = (hashed_auth[i] >> 0) & 0xFF;
        }


        if (verification_result== 1) {
            printf("Verification complete\n");

            while (1) {
                if (trigger==1) {
                    input[32] = (timestamp >> 24) & 0xFF;
                    input[33] = (timestamp >> 16) & 0xFF;
                    input[34] = (timestamp >> 8) & 0xFF;
                    input[35] = (timestamp >> 0) & 0xFF;
                    printf("TIMESTAMP 입력 바이트: %02X %02X %02X %02X\n",
                           input[32], input[33], input[34], input[35]);
                    generate_random(random);
                    print_hash("Random", random);

                    for (int i = 0; i < 8; i++) {
                        key_bytes[i * 4 + 0] = (random[i] >> 24) & 0xFF;
                        key_bytes[i * 4 + 1] = (random[i] >> 16) & 0xFF;
                        key_bytes[i * 4 + 2] = (random[i] >> 8) & 0xFF;
                        key_bytes[i * 4 + 3] = (random[i]) & 0xFF;
                    }
                    unsigned char message[80];
                    // FILE *key_fp = fopen("aes_key.bin", "wb");
                    //fwrite(key_bytes, 1, 32, key_fp);
                    //fclose(key_fp);

                    ////AES_KEY = key_schedule(random);
                    //print_AES(NULL,AES_KEY,60);

                    ////aes_encrypt(input, AES_KEY, encrypted_auth);
                    ////aes_encrypt(input + 16, AES_KEY, encrypted_auth + 16);
                    ////aes_encrypt(input + 32, AES_KEY, encrypted_auth + 32);
                    ////print_AES("Encrypted Auth", encrypted_auth, 48);
                    aes_encrypt_openssl(input,key_bytes,encrypted_auth,16);
                    aes_encrypt_openssl(input+16,key_bytes,encrypted_auth+16,16);
                    aes_encrypt_openssl(input+32,key_bytes,encrypted_auth+32,16);
                    print_aes_result("Encrypted Auth",encrypted_auth,48);


                    ////aes_decrypt(encrypted_auth, AES_KEY, decrypted_uid);
                    ////aes_decrypt(encrypted_auth + 16, AES_KEY, decrypted_uid + 16);
                    ////aes_decrypt(encrypted_auth + 32, AES_KEY, decrypted_uid + 32);
                    ////print_AES("Decrypted Auth", decrypted_uid, 48);
                    aes_decrypt_openssl(encrypted_auth,key_bytes,decrypted_uid,16);
                    aes_decrypt_openssl(encrypted_auth+16,key_bytes,decrypted_uid+16,16);
                    aes_decrypt_openssl(encrypted_auth+32,key_bytes,decrypted_uid+32,16);
                    print_aes_result("Decrypted Auth",decrypted_uid,48);


                    combine(message,  random, encrypted_auth, 8);
                    printf("message size: %lu\n", sizeof(message));  // 확인
                    printf("[MCU] 데이터 전송 준비 완료\n");

                    send_(message, 80);
                    trigger=0;
                }
            }
        }
    }
    return 0;
}
