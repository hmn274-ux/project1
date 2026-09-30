# 정렬 비교 실험 실행 안내

삽입·병합·힙 정렬을 동일한 입력으로 비교합니다. 아래 명령은 저장소의 Makefile이 있는 폴더에서 실행합니다. GCC와 make가 필요하고, 파일 저장 자동화에는 Python 3가 필요합니다.

## 한 번에 검사·실험·안정성 시연·환경 출력

```bash
make test run demo && ./src/main.out --environment
```

필요한 C 실행 파일은 make가 컴파일합니다. 오류가 발생하면 해당 오류를 먼저 해결합니다. 위 명령은 결과를 터미널에 출력하며 기존 측정 파일은 덮어쓰지 않습니다.

| 명령 | 역할 |
|---|---|
| `make test` | 정렬·원소 보존·안정성·측정/검증 함수 검사 |
| `make run` | 48개 조건을 각각 5회 실행해 평균 CPU 시간 등 출력 |
| `make demo` | (key, 원래 위치) 형태로 안정성 시연 |
| `./src/main.out --environment` | 컴파일러·옵션·자료형 크기·측정 설정 출력 |
| `make results` | C 실행 파일과 Python 보조 프로그램으로 결과 파일 생성 |

`make results`는 `report/results.csv`, `report/environment.json`, `report/stability_demo.txt`를 새 결과로 덮어씁니다. 기존 측정을 유지하려면 먼저 별도로 보관하세요.

## 저장된 결과

- `report/results.csv`, `report/environment.json`, `report/stability_demo.txt`: AI 작업 환경의 기존 측정. GCC 13.3.0.
- `report/user_codespaces_2026-09-30/`: 사용자가 Codespaces에서 실행하고 제공한 터미널 출력을 별도로 저장한 결과. GCC 14.2.0. results.csv(48개 조건), environment.json, test_result.txt, stability_demo.txt.
- 두 결과는 서로 다른 실행이며 시간은 합쳐서 평균 내지 않습니다. 사용자 측 CPU 모델·정확한 측정 시각·소스 커밋은 제공된 출력으로 확인되지 않았습니다.

## 표 읽는 법

- `mean_ms`: 동일 입력 5회 정렬의 평균 CPU 시간(ms). 비교·이동 계수 기록과 정렬 함수 내부의 메모리 할당/해제를 포함하며, 입력 생성·복사·결과 검증·출력은 제외합니다.
- `comparisons`, `moves`: 정렬 1회의 키 비교·원소 복사 횟수. 교환 1회는 이동 3회입니다.
- `extra_record_bytes`: 추가 Record 저장 공간. 인덱스·포인터·호출 스택·실험용 배열은 제외합니다.
- `max_recursion_depth`: 정렬 재귀의 최대 깊이. 반복 구현은 1입니다.
- `equal_key_order`: 해당 입력에서 같은 키의 순서를 유지하면 yes, 바뀌면 no, 중복 키가 없으면 n/a입니다. 한 입력의 yes만으로 일반적인 안정성을 증명하지는 않습니다.

보고서에는 저장소 URL을 넣고, 제출용 ZIP은 GitHub의 Code > Download ZIP으로 다운로드합니다.
