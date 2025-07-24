# Lockify - 하드웨어 기반 비밀번호 보안 솔루션

## 프로젝트 소개

Lockify는 하드웨어 보안 요소를 활용한 고급 비밀번호 관리 시스템입니다. 웹사이트마다 서로 다른 비밀번호를 사용해야 하는 현대 인터넷 환경의 문제를 해결하기 위해 설계되었습니다. 사용자는 하나의 마스터 비밀번호만 기억하면 되고, 시스템이 각 웹사이트에 대해 고유하고 안전한 비밀번호를 자동으로 생성하여 보안성을 크게 향상시킵니다.

[프로젝트 발표 자료 보기](https://github.com/J-Draco/2025_Capstone/releases/download/v1.0.0/2025_CapStone.pptx)

## 문제 해결 방안

Lockify는 다음과 같은 문제를 해결합니다:

1. **비밀번호 관리의 어려움**: 수많은 웹사이트에 서로 다른 비밀번호를 사용해야 하는 부담
2. **보안 취약점**: 동일한 비밀번호 재사용으로 인한 보안 위험
3. **복잡한 비밀번호 생성**: 안전한 비밀번호 생성의 어려움

## 주요 기능

### 하드웨어 보안 요소

- **MCU UID 활용**: MCU의 고유 ID를 활용한 인증 시스템
- **타이머 기반 난수 생성**: 하드웨어 타이머를 이용한 예측 불가능한 키 생성
- **하드웨어 기반 암호화**: 소프트웨어 공격에 대한 저항성 향상

### 다중 계층 인증

- **마스터 비밀번호**: 사용자가 기억하는 단일 비밀번호
- **도메인 정보 결합**: 웹사이트 도메인에 따라 고유한 비밀번호 생성
- **SHA-256 해싱**: 안전한 해시 알고리즘 적용

### 암호화 보안

- **AES 암호화**: 대칭키 암호화를 통한 데이터 보호
- **RSA 암호화**: 공개키/개인키 기반 비대칭 암호화 적용
- **다중 계층 암호화**: 여러 단계의 암호화 과정으로 보안성 강화

### 브라우저 통합

- **크롬 확장 프로그램**: 웹사이트 비밀번호 필드 자동 감지
- **자동 비밀번호 입력**: 생성된 비밀번호 자동 입력 기능
- **사용자 친화적 인터페이스**: 간편한 사용자 경험 제공

### 세션 관리

- **만료 시간 설정**: 사용자 정의 보안 세션 관리
- **자동 세션 갱신**: 필요에 따른 세션 연장 기능
- **안전한 세션 종료**: 사용 완료 후 안전한 세션 종료

## 작동 원리

1. **사용자 인증**: 마스터 비밀번호와 MCU UID를 통한 초기 인증
2. **도메인 기반 해시 생성**: 웹사이트 도메인 정보와 결합하여 고유 해시 생성
3. **암호화 처리**: AES와 RSA를 통한 다중 계층 암호화
4. **자동 입력**: 생성된 비밀번호를 웹사이트에 자동 입력

## 시스템 구성

프로젝트는 다음과 같은 주요 구성 요소로 이루어져 있습니다:

1. **클라이언트 앱 (src/client_app)**

   - PyQt5 기반 GUI 애플리케이션
   - 사용자 인증 및 MCU와의 통신 관리
   - 브라우저 확장 프로그램과의 연동

2. **MCU 시뮬레이터 (src/mcu_simulator)**

   - 실제 MCU 하드웨어를 시뮬레이션하는 C 프로그램
   - 암호화 및 인증 로직 처리
   - 타이머 기반 난수 생성 구현

3. **중앙 서버 (src/central_server)**

   - Flask 기반 웹 서버
   - 사용자 인증 및 데이터베이스 관리
   - RSA/AES 암호화 처리

4. **브라우저 확장 프로그램 (src/browser_extension)**

   - 웹사이트의 비밀번호 입력 필드 감지
   - 클라이언트 앱과의 통신
   - 생성된 비밀번호 자동 입력

5. **인증 생성기 (src/auth_generator)**

   - 사용자 인증 파일 생성을 위한 웹 페이지
   - 마스터 비밀번호 설정 기능

6. **유틸리티 (src/utils)**
   - SHA-256 해시 알고리즘 구현
   - 기타 유틸리티 및 공통 코드

## 기술 스택

### 백엔드

- **Python**: 클라이언트 앱 및 서버 로직 구현
- **Flask**: 웹 서버 프레임워크
- **C/C++**: MCU 시뮬레이션 및 암호화 알고리즘 구현
- **SQLite**: 데이터베이스 관리

### 프론트엔드

- **PyQt5**: 클라이언트 GUI 구현
- **HTML/CSS/JavaScript**: 브라우저 확장 프로그램 개발
- **Chrome Extension API**: 브라우저 통합 기능

### 보안 기술

- **SHA-256**: 해시 알고리즘
- **AES**: 대칭키 암호화
- **RSA**: 비대칭 암호화
- **타이머 기반 난수 생성**: 하드웨어 기반 난수 생성

## 프로젝트 구조

```
lockify/
├── src/
│   ├── client_app/          # 클라이언트 애플리케이션
│   │   ├── main.py          # 애플리케이션 진입점
│   │   ├── ui_controller.py # GUI 컨트롤러
│   │   ├── mcu_communication.py # MCU 통신 모듈
│   │   ├── browser_communication.py # 브라우저 통신 모듈
│   │   ├── crypto_utils.py  # 암호화 유틸리티
│   │   ├── global_variables.py # 전역 변수 관리
│   │   ├── auth_validator.py # 인증 검증 모듈
│   │   └── secure/          # 보안 파일 저장 디렉토리 (git에서 무시됨)
│   ├── mcu_simulator/       # MCU 시뮬레이터
│   │   ├── main.c           # 시뮬레이터 진입점
│   │   ├── network_communication.c # 네트워크 통신 모듈
│   │   ├── timer_simulator.c # 타이머 시뮬레이션
│   │   ├── aes_generator.c  # AES 키 생성 모듈
│   │   ├── sha256.c/h       # SHA-256 구현
│   │   └── openssl_en_de.c/h # OpenSSL 암호화/복호화
│   ├── central_server/      # 중앙 서버
│   │   ├── api_server.py    # API 서버 구현
│   │   ├── setup_db.py      # 데이터베이스 설정
│   │   └── secure/          # 보안 파일 저장 디렉토리 (git에서 무시됨)
│   ├── browser_extension/   # 크롬 확장 프로그램
│   │   ├── manifest.json    # 확장 프로그램 설정
│   │   ├── content_script.js # 웹페이지 삽입 스크립트
│   │   ├── background_service.js # 백그라운드 서비스
│   │   └── popup.html       # 팝업 UI
│   ├── auth_generator/      # 인증 파일 생성기
│   │   └── index.html       # 인증 파일 생성 웹페이지
│   └── utils/               # 유틸리티 및 공통 코드
│       └── main.cpp         # SHA-256 테스트 프로그램
└── requirements.txt         # Python 패키지 의존성
```

## 설치 및 실행 방법

### 필수 요구사항

- Python 3.7 이상
- PyQt5
- Flask
- cryptography
- OpenSSL
- Chrome 브라우저 (확장 프로그램용)
- C/C++ 컴파일러 (MCU 시뮬레이션용)
- CMake (빌드 시스템)

### 설치 단계

1. 저장소 복제

   ```bash
   git clone https://github.com/yourusername/lockify.git
   cd lockify
   ```

2. Python 패키지 설치

   ```bash
   pip install -r requirements.txt
   ```

3. MCU 시뮬레이터 빌드

   ```bash
   cd src/mcu_simulator
   cmake .
   make
   ```

4. 중앙 서버 설정

   ```bash
   cd src/central_server
   python setup_db.py
   ```

5. RSA 키 생성 (필요한 경우)

   ```bash
   cd src/central_server
   mkdir -p secure
   openssl genrsa -out secure/server_private.pem 2048
   openssl rsa -in secure/server_private.pem -pubout -out secure/server_public.pem
   mkdir -p ../client_app/secure
   cp secure/server_public.pem ../client_app/secure/
   ```

6. 중앙 서버 실행

   ```bash
   cd src/central_server
   python api_server.py
   ```

7. 클라이언트 앱 실행

   ```bash
   cd src/client_app
   python main.py
   ```

8. 크롬 확장 프로그램 설치
   - Chrome 브라우저에서 `chrome://extensions/` 접속
   - '개발자 모드' 활성화
   - '압축해제된 확장 프로그램 로드' 클릭
   - `src/browser_extension` 폴더 선택

### 초기 설정

1. 인증 생성기를 사용하여 인증 파일 생성

   - `src/auth_generator/index.html` 파일을 브라우저에서 열기
   - 마스터 비밀번호 설정 및 auth.txt 파일 다운로드
   - auth.txt 파일을 src/client_app/secure 폴더에 저장

2. 클라이언트 앱 실행 및 로그인
   - 마스터 비밀번호로 로그인
   - MCU 시뮬레이션과의 연결 확인

## 보안 고려사항

- 모든 보안 관련 파일(키, 인증 파일 등)은 `secure/` 디렉토리에 저장되며 git에서 무시됩니다.
- 실제 구현 시 암호화 키와 인증 파일은 안전하게 보관해야 합니다.
- 프로덕션 환경에서는 추가적인 보안 조치가 필요합니다.
- 이 프로젝트는 교육 및 시연 목적으로 개발되었습니다.

## 기대 효과

- 사용자는 하나의 마스터 비밀번호만 기억하면 됨
- 각 웹사이트마다 고유하고 강력한 비밀번호 사용
- 하드웨어 기반 보안으로 소프트웨어 공격에 대한 저항성 향상
- 비밀번호 관리의 편의성과 보안성 동시 확보
