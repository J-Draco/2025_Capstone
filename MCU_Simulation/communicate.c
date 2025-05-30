#include <sys/socket.h>   // socket, connect, send
#include <arpa/inet.h>    // sockaddr_in, inet_pton
#include <unistd.h>       // close
#include <string.h>
#define MAX_PW_LEN 64
char received_buffer[96] = {0};
unsigned int received_auth[8] = {0};
char received_pw[MAX_PW_LEN + 1] = {0};
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
    server_addr.sin_port = htons(8888);  // 포트는 필요에 따라 조절
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

    if (len <= 32) {
        printf("수신 데이터 길이가 너무 짧습니다: %d\n", len);
        return;
    }

    int pw_len = len - 32;
    memcpy(received_pw, buffer, pw_len);
    received_pw[pw_len] = '\0';

    //printf("[MCU] 수신된 데이터: %s\n", buffer);
    for (int i = 0; i < 8; i++) {
        received_auth[i] = ((unsigned char)buffer[pw_len + i * 4 + 0] << 24) |
                           ((unsigned char)buffer[pw_len + i * 4 + 1] << 16) |
                           ((unsigned char)buffer[pw_len + i * 4 + 2] << 8)  |
                           ((unsigned char)buffer[pw_len + i * 4 + 3]);
    }
    printf("[MCU] 수신된 비밀번호: %s\n", received_pw);
    print_hash("[MCU] 수신된 AUTH",received_auth);

    close(client_sock);
    close(server_sock);
}

int verify_pw(unsigned int *Hashed_UID) {
    char concat[128] = {0};
    unsigned int hashed_pw[8] = {0};

    int length=0;

    sha256(hashed_pw, received_pw, strlen(received_pw));
    print_hash("Hashed_PW:",hashed_pw);
    strcpy(concat,received_pw);
    length=strlen(received_pw);

    for (int i = 0; i < 8; i++) {
        concat[i * 4 + 0] = (hashed_pw[i] >> 24) & 0xFF;
        concat[i * 4 + 1] = (hashed_pw[i] >> 16) & 0xFF;
        concat[i * 4 + 2] = (hashed_pw[i] >> 8)  & 0xFF;
        concat[i * 4 + 3] = (hashed_pw[i] >> 0)  & 0xFF;
    }
    for (int i = 0; i < 8; i++) {
        concat[32 + i * 4 + 0] = (Hashed_UID[i] >> 24) & 0xFF;
        concat[32 + i * 4 + 1] = (Hashed_UID[i] >> 16) & 0xFF;
        concat[32 + i * 4 + 2] = (Hashed_UID[i] >> 8)  & 0xFF;
        concat[32 + i * 4 + 3] = (Hashed_UID[i] >> 0)  & 0xFF;
    }
    // 2. SHA-256 해시
    sha256(hashed_auth, concat, 64);
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

