# Day 04 - Process Creation with fork()

## Overview
`fork()`를 사용해 부모·자식 프로세스를 생성하고, 반환값에 따라 실행 경로를 분리했다.
각 프로세스의 PID·PPID를 출력하고, 전역변수 변경 실험으로 부모와 자식의 메모리가 독립적으로 관리되는 것을 확인했다.

## Environment
- OS: Ubuntu 26.04 LTS on WSL2
- Architecture: x86_64
- GCC: 15.2.0
- GNU Make: 4.4.1
- GDB: 17.1

## Implementation
### 1. Parent / Child Process Creation (`fork_basic.c`)
- `fork()` 반환값으로 생성 실패, 자식, 부모를 구분
- 생성 실패 시 오류 메시지를 출력하고 `EXIT_FAILURE` 반환
- 부모와 자식에서 각각 PID, PPID, `fork()` 반환값 출력
- 분기 이후 공통 코드에 `After fork` 출력을 배치해 두 프로세스의 실행 흐름 확인

### 2. Process Memory Independence (`fork_memory.c`)
- 지역변수 `value`를 `100`으로 초기화한 후 `fork()` 호출
- 전역변수 `value`를 `100`으로 초기화한 후 `fork()` 호출
- 부모는 `20`, 자식은 `10`을 각각 증가
- 실행 결과 부모는 `120`, 자식은 `110`을 출력
- 전역변수와 지역변수의 변경 내용이 프로세스 간 자동으로 공유되지 않음을 확인

## API Used
```c
#include <unistd.h>

pid_t fork(void);
pid_t getpid(void);
pid_t getppid(void);
```
| API | 인자 | 반환값 및 사용 목적 |
|---|---|---|
| `fork()` | 없음 | 자식 프로세스 생성. 부모에서는 자식 PID, 자식에서는 `0`, 실패 시 `-1` 반환 |
| `getpid()` | 없음 | 호출한 프로세스 자신의 PID 반환 |
| `getppid()` | 없음 | 호출한 프로세스의 현재 부모 PID 반환 |

## Build / Run

```bash
gcc -Wall -Wextra -g fork_basic.c -o app
./app

gcc -Wall -Wextra -g fork_memory.c -o app
./app
```

### fork_basic.c Result
```text
Before fork
[PARENT]
PID: 4263
PPID: 1901
Fork return = 4264
After fork
[Child]
PID: 4264
PPID: 488
Fork return = 0
After fork
```

### fork_memory.c Result

```text
[parent] pid: 2546 value: 120
[child] pid: 2547 value: 110
```

PID와 출력 순서는 실행마다 달라질 수 있다.

## Technical Notes
- 자식 프로세스는 `main()`의 처음부터 실행하지 않고, `fork()` 반환 이후부터 실행을 이어 간다.
- 부모가 받은 `fork()` 반환값은 자신의 PID가 아니라 생성된 자식의 PID다.
- 부모와 자식은 독립적인 주소 공간을 가지므로, 전역변수도 변경 내용이 자동으로 공유되지 않는다.
- 부모와 자식의 실행 순서는 별도의 동기화 없이 보장되지 않는다.

## Troubleshooting
- None

## Takeaways
- 한 번의 `fork()` 호출로 생성된 두 프로세스의 실행 경로를 반환값으로 구분할 수 있다.
- 전역변수의 범위와 프로세스 간 메모리 공유는 서로 다른 개념이다.
- 프로세스 간 데이터 전달과 실행 순서 제어에는 별도의 통신·동기화 수단이 필요하다.