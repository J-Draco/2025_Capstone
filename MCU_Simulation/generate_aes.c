#include <stdlib.h>
void print_state(unsigned char *state);
static const unsigned char sbox[16][16] = {
        {0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76},
        {0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0},
        {0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15},
        {0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75},
        {0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84},
        {0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF},
        {0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8},
        {0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2},
        {0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73},
        {0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB},
        {0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79},
        {0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08},
        {0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A},
        {0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E},
        {0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF},
        {0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16}
};
const unsigned char mix_matrix[4][4] = {
        {0x02, 0x03, 0x01, 0x01},
        {0x01, 0x02, 0x03, 0x01},
        {0x01, 0x01, 0x02, 0x03},
        {0x03, 0x01, 0x01, 0x02}
};

unsigned int ROTW(unsigned int Word){
    return (Word<<8)|(Word>>24);
}
unsigned SubWord(unsigned int Word){
    int row = 0;
    int col = 0;
    unsigned char b[4] ;
    for (int i=0;i<4;i++){
        b[i] = (Word >> (24 - 8 * i)) & 0xFF;
        b[i]=sbox[b[i]>>4][b[i]&0x0F];
        }
    return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
    //24-0
    //24-8
    //24-16
    //24-24
}
unsigned int RCON(int i){
        unsigned char RC[15]={0x00,0x01, 0x02, 0x04, 0x08,
                              0x10, 0x20, 0x40, 0x80,
                              0x1B, 0x36, 0x6C, 0xD8,0xAB};
        if(i<1||i>14){
            return 0;
        }
        return (RC[i]<<24);
}
unsigned g(unsigned int Word,int i){
    return SubWord(ROTW(Word)) ^ (RCON(i));
}
unsigned int* key_schedule(unsigned int input[8]){
    unsigned int *Word =malloc(sizeof (unsigned int)*60);
    for(int i=0;i<8;i++){
        Word[i]=input[i];
    }
    //printf("WORD : ");
    //for(int i=0;i<60;i++){
     //   printf("%02x",Word[i]);
    //}printf("\n");
    for(int i=8;i<60;i++){
        if(i%8==0){
            Word[i] = Word[i - 8] ^ g(Word[i - 1], i / 8);
            //Word[i]=Word[i-8] ^ g(Word[i-1],i);
        }else if(i%8==4){
            Word[i]=Word[i-8] ^ SubWord(Word[i-1]);
        }else{
            Word[i]=Word[i-8] ^ Word[i-1];
        }
        //printf("%d Key schedule Word : ",i);
        //for(int i =0;i<60;i++){
         //   printf("%02x",Word[i]);
        //}printf("\n");
    }

    return Word;
}
void sub_bytes(unsigned char* state,unsigned char* out_state){
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            unsigned char byte = *(state + i * 4 + j);
            unsigned char sub = sbox[byte >> 4][byte & 0x0F];
            *(out_state + i * 4 + j) = sub;

        }
    }
}
void shift_rows(unsigned char* state,unsigned char* out_state){
    unsigned char temp[4];
    for(int row=0;row<4;row++){
        for(int col=0;col<4;col++){
            temp[col] = *(state + row * 4 + ((col + row) % 4));// 왼쪽으로 row만큼 shift,임시에저장
            //state[2]=a,b,c,d
            //state[2][(0+2)%4]->state[2][2] ->temp[0]=state[2]:c
            //state[2][(1+2)%4]->state[2][3] ->temp[1]=state[2]:d
            //state[2][(2+2)%4]->state[2][0] ->temp[2]=state[2]:a
            //state[2][(3+2)%4]->state[2][1] ->temp[3]=state[2]:b
            //temp={c,d,a,b]
        }
        for(int col=0;col<4;col++){
            *(out_state + row * 4 + col) = temp[col];  // 출력에 저장
        }
    }
}
unsigned char gf_mul(unsigned char x,unsigned char y){
    unsigned char result=0;
    for(int i=0;i<8;i++){
        if(y&1) {
            result ^= x;//현재자리비트가 1이면 a를 더함 xor
        }
        int limit = x&0x80;//최상위비트확인 8비트 넘어가는지 확인
        x<<=1;//다음비트자리
        if(limit){
            x^=0x1B; //불가항다항식 적용
        }y>>=1;
    }return result;
}

void mix_columns(const unsigned char *state, unsigned char *out_state) {
    for (int i = 0; i < 4; i++) { // 열 기준
        for (int j = 0; j < 4; j++) {
            out_state[j * 4 + i] = 0;
            for (int k = 0; k < 4; k++) {
                out_state[j * 4 + i] ^= gf_mul(mix_matrix[j][k], state[k * 4 + i]);
            }
        }
    }
}
void add_roundkey(const unsigned char *state,unsigned char *round_key, unsigned char* out_state) {
    for (int i = 0; i < 16; i++) {
        out_state[i] = state[i] ^ round_key[i];
    }}
void aes_encrypt(unsigned char *input,unsigned int *round_key,unsigned char *output){
    unsigned char state[16];
    unsigned char temp[16];
    for (int i =0;i<16;i++){
        state[i]=input[i];
    }
    add_roundkey(state, (unsigned char *)(round_key), state);
    for(int round =1 ;round<=13;round++){
        sub_bytes(state, temp);
        shift_rows(temp, state);
        mix_columns(state, temp);
        add_roundkey(temp, (unsigned char *)(round_key + round * 4), state);
    }
    sub_bytes(state, temp);
    shift_rows(temp, state);
    add_roundkey(state, (unsigned char *)(round_key + 14 * 4), output);
}


void inv_sub_bytes(unsigned char* state, unsigned char* out_state) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            unsigned char byte = *(state + i * 4 + j);
            for (int row = 0; row < 16; row++) {
                for (int col = 0; col < 16; col++) {
                    if (sbox[row][col] == byte) {
                        *(out_state + i * 4 + j) = (row << 4) | col;
                        goto found;
                    }
                }
            }
            found:;
        }
    }
}

void inv_shift_rows(unsigned char* state, unsigned char* out_state) {
    unsigned char temp[4];
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            temp[col] = *(state + row * 4 + ((col - row + 4) % 4));
        }
        for (int col = 0; col < 4; col++) {
            *(out_state + row * 4 + col) = temp[col];
        }
    }
}

unsigned char inv_gf_mul(unsigned char a, unsigned char b);

void inv_mix_columns(const unsigned char *state, unsigned char *out_state) {
    const unsigned char inv_mix[4][4] = {
            {0x0e, 0x0b, 0x0d, 0x09},
            {0x09, 0x0e, 0x0b, 0x0d},
            {0x0d, 0x09, 0x0e, 0x0b},
            {0x0b, 0x0d, 0x09, 0x0e}
    };
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            out_state[j * 4 + i] = 0;
            for (int k = 0; k < 4; k++) {
                out_state[j * 4 + i] ^= gf_mul(inv_mix[j][k], state[k * 4 + i]);
            }
        }
    }
}

void aes_decrypt(unsigned char *input, unsigned int *round_key, unsigned char *output) {
    unsigned char state[16];
    unsigned char temp[16];
    for (int i = 0; i < 16; i++) state[i] = input[i];

    add_roundkey(state, (unsigned char *)(round_key + 14 * 4), state);
    for (int round = 13; round >= 1; round--) {
        inv_shift_rows(state, temp);
        inv_sub_bytes(temp, state);
        add_roundkey(state, (unsigned char *)(round_key + round * 4), state);
        inv_mix_columns(state, temp);
        for (int i = 0; i < 16; i++) state[i] = temp[i];
    }
    inv_shift_rows(state, temp);
    inv_sub_bytes(temp, state);
    add_roundkey(state, (unsigned char *)round_key, state);

    // 전체 16바이트 복호화 결과 저장
    for (int i = 0; i < 16; i++) output[i] = state[i];
}


void print_AES(char *name,void *input,int length){
    char * format;
    unsigned char *data = (unsigned char *)input;
    printf("%s : ", name ? name : "Round Key");

    if (name == NULL) {
        name ="Round Key : ";
        format = "%08X";
        unsigned int *data = (unsigned int *)input;
        for (int i = 0; i < length; i++) {
            printf(format, data[i]);
        }
    } else {
        format = "%02X";
        unsigned char *data = (unsigned char *) input;
        for (int i = 0; i < length; i++) {
            printf(format, data[i]);
        }
    }

    printf("\n");
}