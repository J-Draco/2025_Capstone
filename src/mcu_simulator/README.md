# MCU 시뮬레이터 (MCU Simulator)

이 디렉토리는 하드웨어 MCU의 동작을 시뮬레이션하는 C 프로그램을 포함하고 있습니다.

## 주요 파일

- `main.c`: 프로그램의 진입점
- `network_communication.c`: 사용자 프로그램과의 통신 로직
- `sha256.c/h`: SHA-256 해시 함수 구현
- `openssl_en_de.c/h`: OpenSSL 기반 암호화/복호화 기능
- `timer_simulator.c`: MCU 하드웨어 타이머 시뮬레이션
- `aes_generator.c`: AES 키 생성 및 암호화 로직
- `global_data.h`: 전역 변수 정의

## 빌드 방법

```bash
cmake .
make
```

## 실행 방법

```bash
./MCU_Simulation
```

## 주요 기능

- 타이머 기반 난수 생성
- AES 암호화/복호화
- 사용자 인증 및 비밀번호 검증
- 소켓 통신을 통한 데이터 교환
