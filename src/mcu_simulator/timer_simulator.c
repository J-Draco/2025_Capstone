#define MCU_CLOCK_HZ 1000
//mcu클럭 최대 속도 반영 pc환경에선 2.08ns로 타이머를 증가시키는건
//불가능하므로 더낮은 클럭을 기반으로 tick설정
#define tick (1000000000/MCU_CLOCK_HZ)
//나노초->초
#define DELAY 100

#define MCU_UID 0xA12345 //MCU_UID
#define MAX_TICK 4294967296UL  // 2^32 32비트타이머
unsigned int random_value=MCU_UID;
unsigned int C=0;
unsigned int previous_tick=0;
unsigned int simulate_delay();
volatile unsigned int timer = 0;

char uid_bytes[64] = {
        (MCU_UID >> 24) & 0xFF,
        (MCU_UID >> 16) & 0xFF,
        (MCU_UID >> 8)  & 0xFF,
        (MCU_UID >> 0)  & 0xFF,
        // 나머지는 0으로 자동 초기화됨
};

void timer_simulate(){
    while(1){
        for(volatile unsigned int i=0;i<tick/DELAY;i++);
        timer++;
    }
}/*
void event_simulator(){
    printf("엔터 키를 누를 때마다 난수출력 (종료: Ctrl+C)\n");//엔터를 누르는것으로 이벤트 지연을 구현
    while(1){
        getchar();
        generate_random();

        struct timespec start, end;
         clock_gettime(CLOCK_MONOTONIC, &start);

         generate_random();
         clock_gettime(CLOCK_MONOTONIC, &end);
         long elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 +
                           (end.tv_nsec - start.tv_nsec) / 1000000;
         long remaining_ms = 3000 - elapsed_ms;
         if (remaining_ms > 0) {
             usleep(remaining_ms * 1000);  // 남은 시간 채우기
         } else {
             printf("경고: 루프가 %ld ms 초과됨, 다음 루프 즉시 실행\n", -remaining_ms);
         }

        //sleep(3);
    }
}
*/
void generate_random(unsigned int *random){
    unsigned int current_tick = timer;
    unsigned int delta_tick=current_tick-previous_tick;
    C=(C+MCU_CLOCK_HZ*delta_tick)%MAX_TICK;// Ct+1 = (Ct + f * Δt) mod M타이머 누적과정
    random_value=random_value^C;//Rn=Rn-1 xor Ct
    //printf("\tCounter Random = %x\n",random_value);
    unsigned int hash_delay[8];
    unsigned int final_random[8];
    unsigned int random_delay=simulate_delay();
    //printf("\t통신 지연 = %u ticks\n", random_delay);
    previous_tick=current_tick;
    //printf("\tdelta tick=%x\n",delta_tick);

    char stream[64]={0};
    dt_to_bytes(delta_tick,stream);
    sha256(hash_delay,stream,64);
    /*printf("Hashed delay = ");
    for (int i = 0; i < 8; i++) {
        printf("%08x", hash_delay[i]);
    }printf("\n");*/
    for (int i = 0; i < 8; i++) {
        random_value ^= hash_delay[i];  // 누적 XOR로 전체 해시 반영
    }
    unsigned int hash_temp[8] = {0};
    for(int i=0;i<8;i++) {
        dt_to_bytes(hash_delay[i], stream);
        sha256(hash_temp,stream,64);
        for (int j = 0; j < 8; j++) {
            random[j] ^= hash_temp[j];  // 누적 XOR
        }
    }

}
unsigned int simulate_delay(){
    unsigned int req_time=timer;
    for (volatile int i = 0; i < 100000; i++);  // 지연 가정
    unsigned int ack_time = timer;
    return ack_time - req_time;  // delta t

}