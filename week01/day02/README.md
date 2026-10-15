# Week 01 · Day 02 - Linux File I/O
## Overview
- POSIX 저수준 File I/O API인 open(), read(), write(), close()를 이용해 파일 복사 프로그램을 구현.
- 고정 크기 버퍼를 사용해 파일을 반복적으로 읽고 쓰는 구조를 구성, EOF와 system call error를 구분해 처리함.
- write()가 요청한 크기보다 적은 바이트만 기록할 수 있는 상황을 고려해 partial write 처리 로직을 구현하고 동작을 확인

## Environment
- OS: Ubuntu 26.04 LTS on WSL2
- Architecture: x86_64
- GCC: 15.2.0
- GNU Make: 4.4.1
- GDB: 17.1

## Implementation
### File copy flow
source file
    ↓
open(O_RDONLY)
    ↓
read()
    ↓
buffer
    ↓
write()
    ↓
destination file
    ↓
close()

- 실행 인자로 source file과 destination file을 전달받도록 구현.
- source file은 O_RDONLY로 열고, destination file은 O_WRONLY | O_CREAT | O_TRUNC 옵션으로 열음.
- 1024-byte buffer를 사용해 source file을 반복적으로 읽도록 구성.
- read()가 0을 반환하면 EOF로 판단해 복사를 종료.
- read() 또는 write()가 -1을 반환하면 오류 메시지를 출력하고 열린 file descriptor를 정리하도록 처리.

### Partial Write Handling
- 한 번의 read()로 읽은 데이터가 write() 한 번에 모두 기록되지 않을 수 있는 상황을 고려
- BLOCK_SIZE를 300 bytes로 제한해 partial write 상황을 의도적으로 재현했고, 하나의 read buffer가 여러 번의 write를 통해 완전히 기록되는 구조를 확인함.
- `remaining = byte_read - total_written;`: total_written 변수를 사용해 현재 buffer에서 이미 기록한 바이트 수를 추적
- 다음 기록 위치는`buf + total_written`으로 계산함.
- 외부 loop는 새로운 chunk를 `read()`하고, 내부 loop는 현재 chunk가 모두 기록될 때까지 `write()`를 반복하도록 구성함.

```
./copy input.bin output.bin
read: 1024 bytes
write: 300 bytes
write: 300 bytes
write: 300 bytes
write: 124 bytes
total written: 1024
read: 976 bytes
write: 300 bytes
write: 300 bytes
write: 300 bytes
write: 76 bytes
total written: 976
```

## API Used
- open()
```
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```
```
pathname: 열 파일의 경로
flags: 파일 접근 방식 및 동작 지정
mode: O_CREAT 사용 시 생성 파일의 권한 지정
성공 시 file descriptor, 실패 시 -1 반환

주요 옵션:
- O_RDONLY: 읽기 전용
- O_WRONLY: 쓰기 전용
- O_CREAT: 파일이 없으면 생성
- O_TRUNC: 기존 파일이 있으면 내용을 0 byte로 초기화
```
- close()
```c
int close(int fd);
```
```
- 지정한 file descriptor를 닫는다.
- 성공 시 0, 실패 시 -1 반환
```
- read()
```c
ssize_t read(int fd, void *buf, size_t count);
```
```
- fd: 읽을 file descriptor
- buf: 읽은 데이터를 저장할 buffer
- count: 최대 읽기 크기
- 리턴
    - 양수: 실제 읽은 byte 수
    - 0: EOF
    - -1: error
```
- write()
```c
ssize_t write(int fd, const void *buf, size_t count);
```
```
- fd: 기록할 file descriptor
- buf: 기록할 데이터의 시작 주소
- count: 기록을 요청할 byte 수
- 성공 시 실제 기록한 byte 수, 실패 시 -1 반환
```
## Build / Run
- 복사할 input.bin 파일 생성
```bash
dd if=/dev/urandom of=input.bin bs=1000 count=2
```
- build
```bash
make
```
- run
``` bash
./copy input.bin output.bin
```
- 복사한 파일 내용 검증
```bash
cmp input.bin output.bin
echo $?
```
## Technical Notes
- `read() == 0`은 EOF를 의미하며, 파일 복사 종료 조건으로 사용.
- `read()`와 `write()`의 반환값은 실제 처리된 byte 수이므로 요청 크기와 같다고 가정하지 않고 반환값을 기준으로 후속 처리를 수행해야 함.

## Troubleshooting
### 1. 첫 번째 write 이후 복사 종료
#### Problem
`write()`가 성공하면 break하도록 작성해 1024-byte보다 큰 파일의 첫 buffer만 복사됨.
#### Cause
`write()` 성공과 파일 복사 완료를 동일하게 처리함.
#### Solution
종료 조건을 `read() == 0`인 EOF로 변경, write 성공 후에는 다음 read를 수행하도록 수정.

### 2. Partial write 처리 중 read/write loop 구조 혼합
#### Problem
 현재 buffer를 모두 쓰기 전에 다음 `read()`를 수행해 데이터 흐름이 꼬임
#### Cause
 파일 전체 처리와 현재 buffer 처리의 반복 범위를 분리하지 않음
#### Solution
바깥 loop는 `read()`, 안쪽 loop는 현재 buffer가 모두 기록될 때까지 `write()`를 수행하도록 구조 분리

### 3. Makefile object file 치환 실패
#### Problem
 `main.o`가 생성되지 않고 `gcc main.c -o copy`로 바로 빌드됨
#### Cause
 `$(SRCS:.c =.o)`에 포함된 공백으로 suffix substitution 실패
#### Solution
 `$(SRCS:.c=.o)`로 수정해 object 생성 후 link하도록 변경

## Takeaways
- Linux의 File I/O는 file descriptor를 기준으로 open → read/write → close 흐름으로 동작.
- 파일 전체를 한 번에 처리하기보다 고정 크기 buffer를 사용해 chunk 단위로 반복 처리하는 구조가 일반적.
- system call의 반환값은 단순 성공 여부가 아니라 실제 처리 결과를 의미하므로 반드시 확인할 것.
- read()와 write()의 반환값을 기반으로 EOF, error, partial processing을 구분하는 것이 안정적인 I/O 코드의 핵심.