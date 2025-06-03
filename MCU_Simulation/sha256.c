#include "sha256.h"
const unsigned int K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};
unsigned int ROTR(unsigned int x, int n) {
    return (x >> n) | (x << (32 - n));
}
unsigned int Ch(unsigned int x, unsigned int y, unsigned int z) {
    return (x & y) ^ (~x & z);
}
unsigned int Maj(unsigned int x, unsigned int y, unsigned int z) {
    return (x & y) ^ (x & z) ^ (y & z);
}
unsigned int SIGMA0(unsigned int x) {
    return ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22);
}
unsigned int SIGMA1(unsigned int x) {
    return ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25);
}
unsigned int sigma0(unsigned int x) {
    return ROTR(x, 7) ^ ROTR(x, 18) ^ (x >> 3);
}
unsigned int sigma1(unsigned int x) {
    return ROTR(x, 17) ^ ROTR(x, 19) ^ (x >> 10);
}
void dt_to_bytes(unsigned int dt, char *stream) {
    for (int i = 0; i < 4; i++) {
        stream[i] = (dt >> (24 - i * 8)) & 0xFF;
    }
    for (int i = 4; i < 64; i++) {
        stream[i] = 0;
    }
}
void message_schedule(char *stream, unsigned int *W) {
    for (int i = 0; i < 16; i++) {
        W[i] = ((unsigned char)stream[i * 4]     << 24) |
               ((unsigned char)stream[i * 4 + 1] << 16) |
               ((unsigned char)stream[i * 4 + 2] << 8)  |
               ((unsigned char)stream[i * 4 + 3]);
    }
    for (int i = 16; i < 64; i++) {
        W[i] = sigma1(W[i - 2]) + W[i - 7] + sigma0(W[i - 15]) + W[i - 16];
    }
}

void compression(unsigned int *W, unsigned int *H) {
    unsigned int a = H[0], b = H[1], c = H[2], d = H[3];
    unsigned int e = H[4], f = H[5], g = H[6], h = H[7];

    for (int t = 0; t < 64; t++) {
        unsigned int T1 = h + SIGMA1(e) + Ch(e, f, g) + K[t] + W[t];
        unsigned int T2 = SIGMA0(a) + Maj(a, b, c);
        h = g; g = f; f = e; e = d + T1;
        d = c; c = b; b = a; a = T1 + T2;
    }

    H[0] += a; H[1] += b; H[2] += c; H[3] += d;
    H[4] += e; H[5] += f; H[6] += g; H[7] += h;
}

void process_blocks(char *input, int length, unsigned int *H) {
    int bit_len = length * 8;
    int total_len = length + 1 + 8;
    int padded_len = ((total_len + 63) / 64) * 64;

    char *padded = (char *)calloc(padded_len, 1);
    for (int i = 0; i < length; i++) padded[i] = input[i];
    padded[length] = 0x80;

    unsigned long long bit_length = (unsigned long long)bit_len;
    for (int i = 0; i < 8; i++) {
        padded[padded_len - 1 - i] = (bit_length >> (i * 8)) & 0xFF;
    }

    for (int i = 0; i < padded_len; i += 64) {
        unsigned int W[64];
        message_schedule(&padded[i], W);
        compression(W, H);
    }

    free(padded);
}

void sha256(unsigned int *hash_out, char *input, int length) {
    unsigned int H[8] = { H0, H1, H2, H3, H4, H5, H6, H7 };
    process_blocks(input, length, H);
    for (int i = 0; i < 8; i++) {
        hash_out[i] = H[i]; // 결과를 바깥에서 넘긴 포인터에 저장
    }
}
void print_hash(char *name,unsigned int *input){
    printf("%s : ",name);
    for(int i=0;i<8;i++){
        printf("%08X",input[i]);
    }
    printf("\n");
}
void print_hex(const char* label, unsigned char* data, int len) {
    printf("%s: ", label);
    for (int i = 0; i < len; i++) {
        printf("%02X", data[i]);
    }
    printf("\n");
}

