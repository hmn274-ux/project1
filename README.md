# project1 - 삽입·병합·힙 정렬 비교

2026-2 고급알고리즘 정렬 비교 과제의 C 코드와 측정 결과.

저장소: https://github.com/hmn274-ux/project1

## 실행

GitHub의 **Code → Codespaces → Create codespace on main**으로 연 뒤 터미널에서:

```sh
make test       # 정확성·원소 보존·안정성·계수 규칙 검사
make demo       # 같은 값의 순서가 뒤집히는 사례
make run        # 48개 조건의 성능 비교 표
make results    # CSV와 실행 환경, 안정성 예제를 report/에 저장
```

C17 컴파일러와 make만 있으면 C 코드가 실행된다. `make results`에는 Python 3
표준 모듈만 사용한다. 외부 패키지는 필요 없다.

로컬 Docker 사용 시 `docker compose up -d` 후 `docker compose exec lab bash`로
들어가 같은 명령을 실행한다. `make sanitize`는 GCC/Clang의 AddressSanitizer와
UndefinedBehaviorSanitizer를 사용할 수 있는 환경에서 추가 검사를 수행한다.

VS Code의 C 실행 버튼과 Ctrl+Shift+B는 전체 프로젝트의 `make run`을 실행한다.
F5는 안정성 예제를 디버그한다. C 파일 하나만 따로 컴파일하지 않는다.

## 파일

| 경로 | 역할 |
| --- | --- |
| `src/insertionSort.c` | 삽입 정렬 |
| `src/mergeSort.c` | 보조 배열을 사용하는 재귀 병합 정렬 |
| `src/heapSort.c` | 최대 힙과 반복문 기반 힙 정렬 |
| `src/sort.h`, `src/sort.c` | 원소·측정값 정의, 비교·교환 |
| `src/bench.h`, `src/bench.c` | 동일 입력 생성, 시간 측정, 결과 검사 |
| `src/main.c` | 입력 크기·반복 횟수·출력 설정 |
| `tests/test_sort.c` | 테스트 |
| `tools/run_experiment.py` | 측정 결과와 환경 정보 저장 |
| `report/METHOD.md` | 측정 기준과 결과 해석 시 주의점 |

`report/results.csv`는 실제 측정값이며 실행할 때마다 시간은 달라진다.
`report/environment.json`에서 측정 환경을 확인한다. 이 파일이 있다고 해서
최종 제출용 보고서 PDF까지 완성된 것은 아니다.

## 참고 및 AI 활용

- 강의자료 주제 02(기본 정렬), 주제 03(분할 정복과 머지 정렬)의 절차 및
  비교·이동 계수 기준을 참고했다.
- 교수님 샘플: https://github.com/lec-algorithm/hw1-sample-2026
- 실습 환경 원본: https://github.com/lec-algorithm/algorithm-env
- `Dockerfile`과 `compose.yml`은 제공된 샘플의 환경 설정을 사용했다.
  편집기 설정은 이 프로젝트의 명령에 맞춰 작성했다.
- 정렬과 측정 코드는 ChatGPT의 도움으로 작성·검증했다. 샘플의
  `(key, tag)` 안정성 검증 아이디어를 사용하되, 원소 타입을 `Record`로
  고정해 구조를 단순화했다. 힙 정렬 AI 학습 내용은 최종 보고서에 별도 첨부한다.

제출용 ZIP은 GitHub 저장소의 **Code → Download ZIP**으로 받는다.
