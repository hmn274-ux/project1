#include "sort.h"

/* 두 자식의 서브트리가 이미 최대 힙일 때 root를 내려 힙을 복구한다.
 * n은 현재 힙의 크기다. 뒤쪽의 정렬 완료 구간은 건드리지 않는다. */
static void siftDown(Record a[], size_t n, size_t root, SortStats *stats) {
    while (root < n / 2) { /* 자식이 있는 동안만 실행 */
        size_t child = 2 * root + 1;
        if (child + 1 < n &&
            compareKeys(&a[child], &a[child + 1], stats) < 0) {
            child++; /* 더 큰 자식 선택. 같으면 왼쪽 선택 */
        }
        if (compareKeys(&a[root], &a[child], stats) >= 0) break;
        swapRecords(&a[root], &a[child], stats);
        root = child;
    }
}

int heapSort(Record a[], size_t n, SortStats *stats) {
    resetStats(stats);
    if (n < 2) return 1;
    stats->extraRecordBytes = sizeof(Record); /* 교환용 임시 원소 */

    /* 마지막 부모부터 루트까지: 아래쪽 힙을 먼저 완성한다. */
    for (size_t i = n / 2; i > 0; i--) siftDown(a, n, i - 1, stats);

    for (size_t end = n - 1; end > 0; end--) {
        swapRecords(&a[0], &a[end], stats); /* 최댓값의 최종 위치 확정 */
        siftDown(a, end, 0, stats); /* 확정한 위치를 제외하고 복구 */
    }
    return 1;
}
