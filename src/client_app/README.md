# 클라이언트 앱 (Client App)

이 디렉토리는 Lockify의 사용자 인터페이스 및 주요 로직을 포함하고 있습니다.

## 주요 파일

- `main.py`: 애플리케이션의 진입점
- `ui_controller.py`: PyQt5 기반 사용자 인터페이스
- `mcu_communication.py`: MCU 시뮬레이터와의 통신 로직
- `browser_communication.py`: 크롬 확장 프로그램과의 통신 로직
- `auth_validator.py`: 사용자 인증 확인
- `crypto_utils.py`: RSA 암호화 기능
- `global_variables.py`: 전역 변수 관리

## 보안 파일

다음 파일들은 `secure/` 디렉토리에 위치해야 합니다:

- `auth.txt`: 사용자 인증 파일
- `server_public.pem`: 서버 공개키
- `encrypted.bin`: 암호화된 데이터

## 설정

1. `secure/auth.txt` 파일이 이 디렉토리에 위치해야 합니다.
2. MCU 시뮬레이터가 실행 중이어야 합니다.
3. `secure/server_public.pem` 파일이 필요합니다.

## 실행 방법

```bash
python main.py
```
