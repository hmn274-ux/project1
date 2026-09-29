#include "sort.h"

const SortAlgorithm ALGORITHMS[3] = {
    {"insertion", 1, insertionSort},
    {"merge", 1, mergeSort},
    {"heap", 0, heapSort}
};

void resetStats(SortStats *stats) {
    *stats = (SortStats){0, 0, 0, 1};
}

int compareKeys(const Record *a, const Record *b, SortStats *stats) {
    stats->comparisons++;
    /* key 차이를 빼서 반환하면 INT_MIN/INT_MAX에서 오버플로할 수 있다. */
    return (a->key > b->key) - (a->key < b->key);
}

void swapRecords(Record *a, Record *b, SortStats *stats) {
    Record temp = *a;
    *a = *b;
    *b = temp;
    stats->moves += 3;
}
