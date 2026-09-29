#ifndef SORT_H
#define SORT_H

#include <stddef.h>
#include <stdint.h>

/* tag는 입력에서의 위치다. 정렬은 key만 비교하고 tag는 함께 옮긴다. */
typedef struct {
    int key;
    size_t tag;
} Record;

typedef struct {
    uint64_t comparisons;
    uint64_t moves;
    size_t extraRecordBytes; /* 추가 Record 공간만: 인덱스/호출 스택 제외 */
    size_t maxRecursionDepth; /* 정렬 재귀 기준, 반복 구현은 1 */
} SortStats;

/* n=0이면 a=NULL도 허용한다. stats는 항상 유효한 포인터여야 한다.
 * 각 함수는 stats를 초기화한다. 성공은 1, 메모리 확보 실패는 0이다. */
typedef int (*SortFunction)(Record a[], size_t n, SortStats *stats);

typedef struct {
    const char *name;
    int stableByDesign;
    SortFunction sort;
} SortAlgorithm;

extern const SortAlgorithm ALGORITHMS[3];
#define ALGORITHM_COUNT 3

void resetStats(SortStats *stats);
int compareKeys(const Record *a, const Record *b, SortStats *stats);
void swapRecords(Record *a, Record *b, SortStats *stats);
int insertionSort(Record a[], size_t n, SortStats *stats);
int mergeSort(Record a[], size_t n, SortStats *stats);
int heapSort(Record a[], size_t n, SortStats *stats);

#endif
