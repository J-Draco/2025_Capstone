# 유틸리티 (Utilities)

이 디렉토리는 Lockify 시스템에서 사용되는 다양한 유틸리티 코드를 포함하고 있습니다.

## 주요 파일

- `main.cpp`: SHA-256 테스트 프로그램
- `CMakeLists.txt`: CMake 빌드 설정

## 빌드 방법

```bash
cd utils
cmake .
make
```

## 실행 방법

```bash
./SHA256
```

## 참고 사항

이 디렉토리는 주로 테스트 및 교육 목적으로 제공됩니다. 프로젝트의 주요 SHA-256 기능은 mcu_simulator 디렉토리에 있는 구현을 사용합니다.
