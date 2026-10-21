# Day 05 - Process Execution and Exit Status Handling

## Overview

`fork()`로 생성한 자식 프로세스에서 외부 명령을 실행하고, 부모 프로세스가 자식의 종료를 기다리는 명령 실행 프로그램을 구현했다.
고정된 명령을 실행하는 `exec_ls`에서 시작해, 사용자 입력으로 명령과 인자를 전달하는 `run_command`로 확장했다.
자식의 종료 방식을 구분하고 종료 코드를 부모의 반환값으로 전달해 셸에서도 실행 결과를 확인할 수 있도록 구성했다.

## Environment

- OS: Ubuntu 26.04 LTS on WSL2
- Architecture: x86_64
- GCC: 15.2.0
- GNU Make: 4.4.1
- GDB: 17.1

## Implementation

### exec_ls.c

- `fork()`로 자식 프로세스 생성
- 자식에서 `execvp()`로 `ls -l` 실행
- 실행 실패 시 오류 출력 후 `_exit(127)`로 종료
- 부모에서 `waitpid()`로 자식의 종료를 기다리고 종료 정보 회수
- 자식 종료 후 부모의 완료 메시지 출력

### run_command.c

- 실행할 명령이 없으면 사용법 출력 후 실패 종료
- `argv[1]`을 실행 파일 이름으로 사용
- `&argv[1]`을 인자 배열의 시작 주소로 전달
- 부모에서 자식의 일반 종료와 시그널 종료를 구분
- 일반 종료 시 자식의 종료 코드를 부모의 종료 코드로 반환
- 시그널 종료 시 셸 관례에 따라 `128 + 시그널 번호` 반환

처리 흐름:

사용자 인자 검사 → fork → 자식에서 execvp → 부모에서 waitpid → 종료 정보 해석 → 종료 코드 반환

## API Used
### execvp()

```c
int execvp(const char *file, char *const argv[]);
```

- `file`: 실행할 파일 이름 또는 경로
- `argv`: 실행할 프로그램에 전달할 문자열 포인터 배열. 마지막 요소는 `NULL`
- `file`에 `/`가 없으면 `PATH`에서 실행 파일을 검색한다.
- 성공하면 현재 프로세스의 실행 내용을 교체하고 반환하지 않는다.
- 실패하면 `-1`을 반환하고 `errno`를 설정한다.

### waitpid()

```c
pid_t waitpid(pid_t pid, int *wstatus, int options);
```

- `pid`: 기다릴 자식 PID
- `wstatus`: 종료 정보를 저장할 변수의 주소
- `options`: `0`을 사용해 해당 자식의 종료를 기다림
- 성공하면 종료 정보를 회수한 자식 PID를 반환한다.
- 실패하면 `-1`을 반환한다.

### _exit()

```c
void _exit(int status);
```

- 전달받은 종료 상태를 남기고 프로세스를 종료한다.
- 표준 입출력 버퍼를 비우지 않으며 반환하지 않는다.
- 자식의 `execvp()` 실패 처리에서 사용했다.

### 종료 정보 해석 매크로

| 매크로 | 용도 |
|---|---|
| `WIFEXITED(status)` | 일반 종료 여부 확인 |
| `WEXITSTATUS(status)` | 일반 종료 시 종료 코드 추출 |
| `WIFSIGNALED(status)` | 시그널에 의한 종료 여부 확인 |
| `WTERMSIG(status)` | 시그널 종료 시 원인 시그널 번호 추출 |

## Build / Run

### Build

```bash
gcc -Wall -Wextra -g exec_ls.c -o exec_ls
gcc -Wall -Wextra -g run_command.c -o run_command
```

### Run

```bash
./exec_ls

./run_command ls -l
./run_command /bin/echo hello

./run_command /bin/true
echo $?

./run_command /bin/false
echo $?

./run_command nonexistent_command
echo $?
```

- `/bin/true`: 종료 코드 `0`으로 종료
- `/bin/false`: 종료 코드 `1`로 종료
- `echo $?`: 직전에 실행한 명령의 종료 코드 출력

### Test Results

| 테스트 | 확인 결과 |
|---|---|
| `./exec_ls` | 파일 목록 출력 후 부모의 완료 메시지 출력 |
| `./run_command ls -l` | 명령 검색 및 옵션 전달 성공, 종료 코드 `0` |
| `./run_command /bin/echo hello` | `hello` 출력, 종료 코드 `0` |
| `./run_command /bin/true` | 자식 종료 코드 `0`, 셸의 `$?`도 `0` |
| `./run_command /bin/false` | 자식 종료 코드 `1` 확인 |
| `./run_command nonexistent_command` | 실행 오류 출력, 자식 종료 코드 `127`, 셸의 `$?`도 `127` |

시그널 종료 처리 분기는 구현했지만, 실제 시그널에 의한 종료 테스트는 수행하지 않았다.

## Technical Notes

### fork와 exec의 역할

`fork()`는 새로운 프로세스를 생성하고, `execvp()`는 호출한 프로세스의 실행 내용을 다른 프로그램으로 교체한다.
`execvp()` 성공 전후에 해당 프로세스의 PID는 유지된다.

성공한 `execvp()`는 기존 코드로 돌아오지 않으므로, 호출 다음에 실행 실패 처리를 바로 배치할 수 있다.

```c
execvp(argv[1], &argv[1]);

perror("execvp");
_exit(127);
```

### 인자 배열 재사용

다음과 같이 실행하면:

```bash
./run_command ls -l
```

기존 인자 배열은 다음과 같다.

| 요소 | 내용 |
|---|---|
| `argv[0]` | `"./run_command"` |
| `argv[1]` | `"ls"` |
| `argv[2]` | `"-l"` |
| `argv[3]` | `NULL` |

```c
execvp(argv[1], &argv[1]);
```

`argv[1]`은 실행 파일 이름을 전달하고, `&argv[1]`은 `"ls"`부터 시작하는 인자 배열을 전달한다.
기존 배열 끝의 `NULL`을 그대로 활용하므로 별도의 배열 생성이 필요하지 않다.

```c
argv[1]  == *(argv + 1)
&argv[1] == argv + 1
```

### 종료 정보와 종료 코드

`waitpid()`가 저장하는 `status`에는 종료 방식 등의 정보가 함께 들어 있다.
따라서 `status`를 그대로 종료 코드로 사용하지 않고, 종료 방식을 확인한 뒤 대응하는 매크로로 값을 추출했다.

`WIFEXITED(status)`가 참이라는 것은 일반 종료를 의미하며, 작업 성공을 의미하지는 않는다.
성공 여부는 종료 코드를 통해 별도로 판단한다.

### 부모를 통한 종료 코드 전달

자식과 부모의 종료 코드는 독립적이다.
부모가 항상 `EXIT_SUCCESS`를 반환하면 자식의 명령 실행이 실패해도 셸에서는 성공으로 확인된다.

이를 해결하기 위해 일반 종료 시 추출한 자식의 종료 코드를 부모가 그대로 반환하도록 구현했다.

```c
return exit_status;
```

시그널 종료 시에는 `128 + 시그널 번호`를 반환하도록 구성했다.
이 경우 부모 자체는 시그널로 종료되는 것이 아니라 해당 값을 반환하고 일반 종료한다.

## Troubleshooting

### 부모에서 자식 PID 변수가 초기화되지 않은 문제

- 문제: 자식 분기에서 `pid_child = getpid()`를 실행했지만 부모의 `waitpid()`에서 초기화되지 않은 변수 경고 발생
- 원인: 부모와 자식은 각자의 메모리를 사용하므로 자식의 변수 변경이 부모에게 반영되지 않음
- 해결: `fork()`가 부모에게 반환한 자식 PID를 `waitpid()`에 직접 전달

### 실행 실패 메시지가 출력되지 않을 수 있는 문제

- 문제: 오류 메시지를 `printf()`로 출력한 뒤 `_exit()`를 호출하면 메시지가 버퍼에 남을 수 있음
- 원인: `_exit()`는 표준 입출력 버퍼를 비우지 않음
- 해결: 실행 실패 시 표준 오류로 메시지를 출력하는 `perror()` 사용

### 자식이 실패해도 부모가 성공으로 종료하는 문제

- 문제: 자식이 종료 코드 `127`로 종료해도 부모가 `EXIT_SUCCESS`를 반환하면 셸의 `$?`는 `0`
- 원인: 자식의 종료 상태가 부모의 종료 코드에 자동으로 반영되지 않음
- 해결: `WEXITSTATUS()`로 추출한 종료 코드를 부모가 반환하고, 셸에서 `$?`로 전달 결과 확인

## Takeaways

- 프로세스 생성, 프로그램 실행, 종료 정보 회수를 각각 분리해 구성했다.
- 기존 인자 배열을 재사용해 명령과 가변 개수의 인자를 전달하는 실행기를 구현했다.
- 일반 종료와 작업 성공을 구분하고, 자식의 실행 결과를 호출자인 셸까지 전달했다.