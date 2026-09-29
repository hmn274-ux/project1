#include "sort.h"

int insertionSort(Record a[], size_t n, SortStats *stats) {
    resetStats(stats);
    if (n < 2) return 1;
    stats->extraRecordBytes = sizeof(Record);

    for (size_t i = 1; i < n; i++) {
        Record temp = a[i];
        stats->moves++; /* 임시 원소로 복사 */
        size_t j = i;
        while (j > 0) {
            if (compareKeys(&a[j - 1], &temp, stats) <= 0) break;
            /* 같은 값은 밀지 않으므로 먼저 온 원소가 앞에 남는다. */
            a[j] = a[j - 1];
            stats->moves++;
            j--;
        }
        a[j] = temp;
        stats->moves++; /* 제자리에 다시 써도 이동 1회로 센다. */
    }
    return 1;
}
