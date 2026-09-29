#include "sort.h"
#include <stdlib.h>

/* [left, right) 구간: right 위치는 포함하지 않는다. */
static void mergeRange(Record a[], Record buffer[], size_t left,
                       size_t right, size_t depth, SortStats *stats) {
    if (depth > stats->maxRecursionDepth) stats->maxRecursionDepth = depth;
    if (right - left < 2) return;
    size_t mid = left + (right - left) / 2;
    mergeRange(a, buffer, left, mid, depth + 1, stats);
    mergeRange(a, buffer, mid, right, depth + 1, stats);

    size_t i = left, j = mid, k = left;
    while (i < mid && j < right) {
        /* 같으면 원래 앞쪽 구간인 왼쪽을 먼저 선택: 안정성 보장 */
        if (compareKeys(&a[i], &a[j], stats) <= 0) buffer[k++] = a[i++];
        else buffer[k++] = a[j++];
        stats->moves++;
    }
    while (i < mid) {
        buffer[k++] = a[i++];
        stats->moves++;
    }
    while (j < right) {
        buffer[k++] = a[j++];
        stats->moves++;
    }
    for (k = left; k < right; k++) {
        a[k] = buffer[k];
        stats->moves++;
    }
}

int mergeSort(Record a[], size_t n, SortStats *stats) {
    resetStats(stats);
    if (n < 2) return 1;
    if (n > SIZE_MAX / sizeof(Record)) return 0;
    Record *buffer = malloc(n * sizeof(*buffer));
    if (!buffer) return 0;
    stats->extraRecordBytes = n * sizeof(*buffer);
    mergeRange(a, buffer, 0, n, 1, stats);
    free(buffer);
    return 1;
}
