# 이번 코드의 검증 기록

- 실행 장소: ChatGPT의 Linux 실행 환경. GitHub Codespaces에서 실행한 결과는 아니다.
- 컴파일러: GCC 13.3.0, C17, `-Wall -Wextra -Wpedantic -O2`. 컴파일 경고 없음.
- `make test`: 3,313개 입력 × 세 정렬, 총 156,307개 검사 통과.
- `make demo`: 삽입·병합은 같은 값의 순서를 유지했고 힙은 뒤집는 사례 확인.
- `make results`: 48개 조건 × 5회 실행. 모든 결과의 정렬 정확성과 원소 보존 확인.
- 중복이 많은 입력의 네 크기에서 삽입·병합의 상대 순서 유지, 힙의 순서 변경 확인.
- AddressSanitizer/UndefinedBehaviorSanitizer: 테스트 통과.
  실행 환경의 `/proc` 접근 제한으로 LeakSanitizer가 실행되지 않아
  `ASAN_OPTIONS=detect_leaks=0 ./tests/test_sanitize.out`으로 누수 검사만 제외했다.
  따라서 누수 검사까지 통과했다고 주장하지 않는다.
- 편집기/컨테이너 JSON 구문 확인. 이 환경에는 Docker가 없어 컨테이너 이미지
  빌드와 Codespaces UI 실행은 검증하지 않았다.

원본 수치는 `results.csv`, 실행 환경과 소스 해시는 `environment.json`에 있다.
최종 제출 보고서는 이 측정값의 의미와 알고리즘 학습 내용을 별도로 정리해야 한다.
