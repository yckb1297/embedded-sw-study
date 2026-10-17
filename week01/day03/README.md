# Day 03 - File Offset and lseek()

## Overview
POSIX File I/O를 이용해 파일 오프셋을 조회하고 이동하는 코드를 작성.
명령행 인자로 받은 문자열을 파일의 지정 위치에 덮어쓰는 프로그램을 구현하고, `cat`과 `xxd`로 변경된 데이터를 확인.

## Environment
- OS: Ubuntu 26.04 LTS on WSL2
- Architecture: x86_64
- GCC: 15.2.0
- GNU Make: 4.4.1
- GDB: 17.1

## Implementation

### File Offset Test
- `open()`으로 파일을 읽기 전용으로 열고 `read()`로 데이터를 읽는 코드 작성
- `lseek()`의 기준 위치에 따른 오프셋 계산 방식 확인
- 현재 위치 조회와 파일 시작·현재 위치·파일 끝 기준의 위치 이동 구분

### File Overwrite
- 명령행 인자로 대상 파일 경로와 덮어쓸 문자열 전달
- `O_RDWR`로 기존 파일을 읽기·쓰기 모드로 열기
- `lseek(fd, 4, SEEK_SET)`으로 파일 시작에서 4바이트 위치로 이동
- `write()`로 전달받은 문자열 기록
- 시스템 호출 실패 시 오류를 출력하고 파일 디스크립터 정리

처리 흐름:

`인자 검사 → 파일 열기 → 오프셋 이동 → 문자열 쓰기 → 파일 닫기`

## API Used

### lseek()
```c
off_t lseek(int fd, off_t offset, int whence);
```
- `fd`의 파일 오프셋을 `whence` 기준으로 `offset`만큼 설정
- 성공 시 변경된 오프셋, 실패 시 `(off_t)-1` 반환

| whence | 기준 | 변경 후 오프셋 |
|---|---|---|
| SEEK_SET | 파일 시작 | offset |
| SEEK_CUR | 현재 오프셋 | 현재 오프셋 + offset |
| SEEK_END | 파일 크기 | 파일 크기 + offset |

## Build / Run

### Offset Test
```bash
gcc -Wall -Wextra -g -o offset_test offset_test.c
printf "ABCDEFGHIJ" > data.txt
./offset_test data.txt
```

### Overwrite Test
```bash
make
printf "0123456789" > data.txt
./overwrite data.txt ABC
cat data.txt
xxd data.txt
```

확인한 출력:

```text
byte_writen: 3
```

변경된 파일 내용:

```text
0123ABC789
```

`xxd` 확인 결과:

```text
00000000: 3031 3233 4142 4337 3839                 0123ABC789
```

- `printf`: 지정한 문자열 출력
- `>`: 출력 내용을 대상 파일에 저장하며 기존 내용 대체
- `cat`: 파일 내용 출력
- `xxd`: 파일 내용을 16진수와 문자로 표시
- `xxd -g 1 data.txt`: 1바이트 단위로 구분해서 표시

## Technical Notes
- 파일 오프셋은 다음 읽기 또는 쓰기가 시작되는 위치다.
- `lseek(fd, 0, SEEK_CUR)`는 위치를 변경하지 않고 현재 오프셋을 반환한다.
- `SEEK_CUR`의 양수 이동량은 파일 끝 방향, 음수 이동량은 파일 시작 방향을 의미한다.
- `lseek()` 자체는 파일 데이터를 읽거나 쓰지 않는다.
- `read()`와 `write()`는 실제 처리한 바이트 수만큼 오프셋을 증가시킨다.
- `write()`는 기존 데이터를 덮어쓴다. 이번 테스트에서는 `456`이 `ABC`로 바뀌었고 파일 크기는 10바이트로 유지됐다.
- `argv[0]`을 사용하면 실행 파일 이름을 하드코딩하지 않고 사용법을 출력할 수 있다.

## Troubleshooting

### 현재 위치 조회에 SEEK_SET 사용
- 문제: 현재 오프셋을 확인하려고 `lseek(fd, 0, SEEK_SET)` 호출
- 원인: `SEEK_SET`은 파일 시작을 기준으로 위치를 설정하므로 오프셋이 0으로 변경됨
- 해결 방법: 현재 위치를 유지하며 조회하는 `lseek(fd, 0, SEEK_CUR)` 사용

### 덮어쓴 데이터가 비정상적인 문자로 출력
- 문제: `"ABC"`를 전달했지만 파일에 예상과 다른 바이트가 기록됨
- 원인: `char *buf[10]`으로 포인터 배열을 선언하고, 배열 주소를 `write()`에 전달해 포인터 값의 일부가 기록됨
- 해결: `char *buf`로 변경하고 `buf = argv[2]`로 문자열을 가리키도록 수정
- 검증: `cat`과 `xxd`로 오프셋 4~6에 `ABC`에 해당하는 `41 42 43`이 기록된 것을 확인

### 문자열 버퍼 재사용 시 이전 데이터가 남을 가능성
- 문제: 3바이트를 읽은 버퍼에 2바이트만 다시 읽으면 이전 세 번째 문자가 남을 수 있음
- 원인: `read()`는 문자열 종료 문자를 자동으로 추가하지 않음
- 해결 방법: 읽기 성공 후 반환값을 기준으로 `buf[ret] = '\0'` 설정. 단, 종료 문자를 위한 여유 공간 확보 필요

## Takeaways
- 파일의 특정 구간을 수정할 때는 기준 위치와 이동량을 명확히 지정해야 한다.
- 데이터 주소와 포인터를 저장한 공간의 주소를 구분해야 올바른 바이트를 전달할 수 있다.
- 문자 출력과 16진수 출력을 함께 확인하면 파일 변경 결과를 바이트 단위로 검증할 수 있다.