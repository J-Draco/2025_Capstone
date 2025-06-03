//
// Created by Moon Seoung Min on 4/17/25.
//

#ifndef MCU_SIMULATION_SHA256_H
#define MCU_SIMULATION_SHA256_H
#include <stdlib.h>
#include <stdio.h>
#define H0 1779033703
#define H1 3144134277
#define H2 1013904242
#define H3 2773480762
#define H4 1359893119
#define H5 2600822924
#define H6 528734635
#define H7 1541459225


unsigned int ROTR(unsigned int x, int n);
unsigned int Ch(unsigned int x, unsigned int y, unsigned int z);
unsigned int Maj(unsigned int x, unsigned int y, unsigned int z);
unsigned int SIGMA0(unsigned int x);
unsigned int SIGMA1(unsigned int x);
unsigned int sigma0(unsigned int x);
unsigned int sigma1(unsigned int x);
// SHA 관련 함수
void message_schedule(char *stream, unsigned int *W);
void compression(unsigned int *W, unsigned int *H);
void process_blocks(char *input, int length, unsigned int *H);
void sha256(unsigned int *hash_out, char *input, int length);
// 입력 숫자를 SHA 입력으로 포맷
void dt_to_bytes(unsigned int dt, char *stream);
void print_hash(char *name,unsigned int *input);
void print_hex(const char* label, unsigned char* data, int len);
#endif //MCU_SIMULATION_SHA256_H

