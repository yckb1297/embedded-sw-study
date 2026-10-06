# DAY01 - Linux Development Environment

## Goal
- Linux C 개발환경 확인 및 구성
- GCC 직접 빌드
- Makefile 작성

## Environment
- OS: Ubuntu 26.04 LTS on WSL2
- Architecture: x86_64
- GCC: 15.2.0
- GNU Make: 4.4.1
- GDB: 17.1

## Implementation
- GCC를 이용한 C 프로그램 빌드
- Makefile 기반 빌드 자동화
- `make`, `make clean` target 구성
- GDB breakpoint 및 실행 흐름 확인

## Build / Run
```bash
make
make run
make clean
```

## Technical Notes
- Compile vs Link
    - source -> object
    - object + library -> executable
- GCC Build Stages
    - preprocessing: `-E`
    - compilation: `-S`
    - assembly: `-c`
    - linking: executable 생성ㅇ
- Object File
    - 링크 전 단계의 architecture-dependent object code
- Makefile Dependency
    - target/prerequisite 및 timestamp를 기반으로 필요한 부분만 다시 빌드
- Native vs Cross Compiler
    - `gcc`: x86_64 native compiler
    -`aarch64-linux-gnu-gcc`: ARM64 cross compiler

## Troubleshooting
    - `gcc -E main.c` 실행 시 -o를 지정하지 않아 전처리 결과가 stdout으로 출력됨
        - `gcc -E main.c -o main.i` 형태로 출력 파일을 지정하여 해결

## Takeaways
    - GCC의 전체 빌드 단계를 직접 확인
    - Makefile이 단순 명령 모음이 아니라 dependency 기반 빌드 시스템이라는 점 확인
    - Natice compiler와 Cross compiler의 차이 이해

