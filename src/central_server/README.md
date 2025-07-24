# 중앙 서버 (Central Server)

이 디렉토리는 Lockify의 중앙 서버 구성 요소를 포함하고 있습니다.

## 주요 파일

- `api_server.py`: Flask 기반 웹 서버
- `setup_db.py`: 데이터베이스 초기 설정 스크립트

## 보안 파일

다음 파일들은 `secure/` 디렉토리에 위치해야 합니다:

- `database.db`: SQLite 데이터베이스 파일
- `server_private.pem`: RSA 개인 키 파일
- `server_public.pem`: RSA 공개 키 파일

## 기능

- 사용자 인증 및 권한 관리
- RSA 암호화/복호화
- AES 암호화/복호화
- 도메인별 비밀번호 생성 로직

## 설정

서버를 실행하기 전에 다음 단계를 수행하세요:

1. 데이터베이스 초기화:

   ```bash
   python setup_db.py
   ```

2. RSA 키 생성 (필요한 경우):

   ```bash
   cd secure
   openssl genrsa -out server_private.pem 2048
   openssl rsa -in server_private.pem -pubout -out server_public.pem
   cp server_public.pem ../../client_app/secure/
   ```

3. `server_public.pem` 파일을 client_app/secure 디렉토리에 복사하세요.

## 실행 방법

```bash
python api_server.py
```

서버는 기본적으로 포트 10001에서 실행됩니다.
