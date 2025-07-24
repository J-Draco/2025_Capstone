#include <sys/socket.h>   // socket, connect, send
#include <arpa/inet.h>    // sockaddr_in, inet_pton
#include <unistd.h>       // close
#include <string.h>
#include "openssl_en_de.c"
#include "global_data.h"
#include "sha256.h"
void send_final_hash(unsigned int *final_hash);
char received_buffer[96] = {0};
unsigned int received_auth[8] = {0};
unsigned char received_pw[32] = {0};
int trigger=0;
unsigned int hashed_auth[8] = {0};
unsigned int timestamp=0;
void combine(char* output, unsigned int* a1,unsigned char *a2,int array_size) {
    for (int i = 0; i < array_size; i++) {
        //unsigned int value = hash_array[i];
        output[i * 4 + 0] = (a1[i] >> 24) & 0xFF;
        output[i * 4 + 1] = (a1[i] >> 16) & 0xFF;
        output[i * 4 + 2] = (a1[i] >> 8)  & 0xFF;
        output[i * 4 + 3] = (a1[i] >> 0)  & 0xFF;

    }
    for (int i = 0; i < 48; i++) {
        output[32 + i] = a2[i];
    }
}

void send_(const char* message,int length) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return;  // 소켓 생성 실패
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        printf("no\n");
        return;  // 연결 실패
    }

    send(sock, message, length, 0);
    close(sock);
}
void receive_pw() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[96] = {0};

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("소켓 생성 실패");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("바인딩 실패");
        close(server_sock);
        return;
    }

    listen(server_sock, 1);
    printf("[MCU] 비밀번호 입력 대기 중...\n");

    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
    int len = recv(client_sock, buffer, sizeof(buffer), 0);

    if (client_sock < 0) {
        perror("연결 실패");
        close(server_sock);
        return;
    }

    if (len !=64) {
        printf("수신 데이터 길이가 64바이트가 아닙니다 %d\n", len);
        return;
    }
    memcpy(received_pw, buffer, 32);

    //printf("[MCU] 수신된 데이터: %s\n", buffer);
    for (int i = 0; i < 8; i++) {
        received_auth[i] = ((unsigned char)buffer[32 + i * 4 + 0] << 24) |
                           ((unsigned char)buffer[32 + i * 4 + 1] << 16) |
                           ((unsigned char)buffer[32 + i * 4 + 2] << 8)  |
                           ((unsigned char)buffer[32 + i * 4 + 3]);
    }
    print_hex("[MCU] 수신된 H(비밀번호)", received_pw,32);
    print_hash("[MCU] 수신된 AUTH",received_auth);

    close(client_sock);
    close(server_sock);
}

int verify_pw(unsigned int *Hashed_UID) {
    unsigned char concat[64] = {0};
    memcpy(concat,received_pw,32);

    for (int i = 0; i < 8; i++) {
        concat[32 + i * 4 + 0] = (Hashed_UID[i] >> 24) & 0xFF;
        concat[32 + i * 4 + 1] = (Hashed_UID[i] >> 16) & 0xFF;
        concat[32 + i * 4 + 2] = (Hashed_UID[i] >> 8)  & 0xFF;
        concat[32 + i * 4 + 3] = (Hashed_UID[i] >> 0)  & 0xFF;
    }
    // 2. SHA-256 해시
    sha256(hashed_auth, (char *)concat, 64);
    print_hash("H(H(PW)+H(MUC_UID))",hashed_auth);
    for (int i = 0; i < 8; i++) {
        if (hashed_auth[i] != received_auth[i]) {
            printf("인증 실패\n");
            return 0;

        }
    }

    printf("인증 성공\n");
    return 1;
}
void send_verification(int result){
    // 인증 후

    int result_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in result_addr;
    result_addr.sin_family = AF_INET;
    result_addr.sin_port = htons(7777);  // 결과 전송용 포트
    inet_pton(AF_INET, "127.0.0.1", &result_addr.sin_addr);

    connect(result_sock, (struct sockaddr*)&result_addr, sizeof(result_addr));
    if (result == 1) {
        send(result_sock, "OK", 2, 0);
    } else {
        send(result_sock, "FAIL", 4, 0);
    }
    close(result_sock);

}
void* receive_req_signal(void* arg) {

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[5] = {0};

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) pthread_exit(NULL);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6666);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(server_sock);
        pthread_exit(NULL);
    }

    listen(server_sock, 5);
    printf("[MCU] 요청 대기 중 (6666)...\n");

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
        if (recv(client_sock, buffer, sizeof(buffer), 0) > 0 && buffer[0] == 'R') {
            timestamp =
                    ((unsigned char)buffer[1] << 24) |
                    ((unsigned char)buffer[2] << 16) |
                    ((unsigned char)buffer[3] << 8) |
                    ((unsigned char)buffer[4]);
            printf("[MCU] 요청 수신! Timestamp: %u\n", timestamp);
            trigger = 1;
        }
        close(client_sock);
    }

    close(server_sock);
    pthread_exit(NULL);
}
void* fin_out(void* arg) {

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[96] = {0};//uid32바이트+salt32바이트+H(PW)32바이트

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) pthread_exit(NULL);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4444);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(server_sock);
        pthread_exit(NULL);
    }
    listen(server_sock, 5);

    while (1) {
        printf("SALT 요청대기중\n");
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
        if (client_sock < 0) continue;

        int received = recv(client_sock, buffer, sizeof(buffer), 0);
        if (received != 96) {
            close(client_sock);
            printf("[fin_out] received: %d bytes\n", received);

            continue;
        }
        unsigned char ciphertext[64], pw_hash[32];
        memcpy(ciphertext, buffer, 64);
        memcpy(pw_hash, buffer + 64, 32);
        print_aes_result("Received Cipher",ciphertext,96);
        print_hex("Domain pw",pw_hash,32);
        unsigned char plain[64] = {0};//복호화

        aes_decrypt_openssl(ciphertext,key_bytes,plain,16);
        aes_decrypt_openssl(ciphertext+16,key_bytes,plain+16,16);
        aes_decrypt_openssl(ciphertext+32,key_bytes,plain+32,16);
        aes_decrypt_openssl(ciphertext+48,key_bytes,plain+48,16);

        unsigned char recv_uid_hash[32], salt[32];
        memcpy(recv_uid_hash, plain, 32);
        memcpy(salt, plain + 32, 32);
        // UID 해시 비교
        unsigned char hashed_uid_bytes[32];
        for (int i = 0; i < 8; i++) {
            hashed_uid_bytes[i * 4 + 0] = (Hashed_UID[i] >> 24) & 0xFF;
            hashed_uid_bytes[i * 4 + 1] = (Hashed_UID[i] >> 16) & 0xFF;
            hashed_uid_bytes[i * 4 + 2] = (Hashed_UID[i] >> 8) & 0xFF;
            hashed_uid_bytes[i * 4 + 3] = (Hashed_UID[i] >> 0) & 0xFF;
        }
        if (memcmp(recv_uid_hash, hashed_uid_bytes, 32) != 0) {
            printf("UID 불일치: 인증 실패\n");
            close(client_sock);
            continue;
        }
        unsigned char pw_salt[64];
        memcpy(pw_salt, pw_hash, 32);
        memcpy(pw_salt + 32, salt, 32);

        unsigned int final_hash[8];
        sha256(final_hash, (char*)pw_salt, 64);
        print_hash("FINAL PW",final_hash);
        send_final_hash(final_hash);

    }

    close(server_sock);
    pthread_exit(NULL);
}

void send_final_hash(unsigned int *final_hash) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    unsigned char bytes[32];

    // final_hash (uint[8]) → bytes[32]
    for (int i = 0; i < 8; i++) {
        bytes[i * 4 + 0] = (final_hash[i] >> 24) & 0xFF;
        bytes[i * 4 + 1] = (final_hash[i] >> 16) & 0xFF;
        bytes[i * 4 + 2] = (final_hash[i] >> 8) & 0xFF;
        bytes[i * 4 + 3] = (final_hash[i]) & 0xFF;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5556);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("사용자 프로그램 연결 실패");
        close(sock);
        return;
    }

    send(sock, bytes, 32, 0);
    close(sock);
}
