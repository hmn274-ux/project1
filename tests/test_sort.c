#include "bench.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t checks = 0;
static size_t cases = 0;

#define CHECK(condition) do { \
    checks++; \
    if (!(condition)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

/* 검증 기준만 key/tag로 정렬한다. 실제 세 정렬은 tag를 비교하지 않는다. */
static int referenceCompare(const void *left, const void *right) {
    const Record *a = left, *b = right;
    if (a->key != b->key) return (a->key > b->key) - (a->key < b->key);
    return (a->tag > b->tag) - (a->tag < b->tag);
}

static void checkCase(const int keys[], size_t n) {
    Record *input = n ? malloc(n * sizeof(*input)) : NULL;
    Record *work = n ? malloc(n * sizeof(*work)) : NULL;
    Record *reference = n ? malloc(n * sizeof(*reference)) : NULL;
    CHECK(n == 0 || (input && work && reference));
    for (size_t i = 0; i < n; i++) input[i] = (Record){keys[i], i};
    if (n) {
        memcpy(reference, input, n * sizeof(*reference));
        qsort(reference, n, sizeof(*reference), referenceCompare);
    }
    for (size_t ai = 0; ai < ALGORITHM_COUNT; ai++) {
        if (n) memcpy(work, input, n * sizeof(*work));
        SortStats stats;
        Validation validation;
        CHECK(ALGORITHMS[ai].sort(work, n, &stats));
        CHECK(validateResult(input, work, n, &validation));
        CHECK(validation.sorted && validation.permutationPreserved);
        if (ALGORITHMS[ai].stableByDesign) CHECK(validation.stableOnInput);
        for (size_t i = 0; i < n; i++) {
            CHECK(work[i].key == reference[i].key);
            if (ALGORITHMS[ai].stableByDesign) CHECK(work[i].tag == reference[i].tag);
        }
    }
    free(reference);
    free(work);
    free(input);
    cases++;
}

static void exhaustiveSmallArrays(void) {
    /* -1,0,1로 만들 수 있는 길이 0~7의 모든 배열: 중복/안정성 집중 확인 */
    size_t combinations = 1;
    for (size_t n = 0; n <= 7; n++) {
        for (size_t code = 0; code < combinations; code++) {
            int keys[7];
            size_t value = code;
            for (size_t i = 0; i < n; i++) {
                keys[i] = (int)(value % 3) - 1;
                value /= 3;
            }
            checkCase(keys, n);
        }
        combinations *= 3;
    }
}

static void edgeAndGeneratedCases(void) {
    const int limits[] = {INT_MAX, 0, INT_MIN, INT_MAX, -1, INT_MIN};
    checkCase(limits, sizeof(limits) / sizeof(limits[0]));
    const size_t lengths[] = {2, 3, 8, 31, 32, 33, 127, 257};
    for (size_t i = 0; i < sizeof(lengths) / sizeof(lengths[0]); i++) {
        Record input[257];
        int keys[257];
        for (int shape = RANDOM_INPUT; shape <= FEW_UNIQUE_INPUT; shape++) {
            makeInput(input, lengths[i], (InputShape)shape, (uint32_t)(700 + i));
            for (size_t j = 0; j < lengths[i]; j++) keys[j] = input[j].key;
            checkCase(keys, lengths[i]);
        }
    }
}

static void checkCounters(void) {
    Record a[8];
    SortStats stats;
    makeInput(a, 8, SORTED_INPUT, 1);
    CHECK(insertionSort(a, 8, &stats));
    CHECK(stats.comparisons == 7 && stats.moves == 14);
    CHECK(stats.extraRecordBytes == sizeof(Record) && stats.maxRecursionDepth == 1);
    makeInput(a, 8, REVERSE_INPUT, 1);
    CHECK(insertionSort(a, 8, &stats));
    CHECK(stats.comparisons == 28 && stats.moves == 42);

    makeInput(a, 4, SORTED_INPUT, 1);
    CHECK(mergeSort(a, 4, &stats));
    CHECK(stats.comparisons == 4 && stats.moves == 16);
    CHECK(stats.extraRecordBytes == 4 * sizeof(Record) && stats.maxRecursionDepth == 3);

    a[0] = (Record){2, 0};
    a[1] = (Record){2, 1};
    CHECK(heapSort(a, 2, &stats));
    CHECK(stats.comparisons == 1 && stats.moves == 3);
    CHECK(a[0].tag == 1 && a[1].tag == 0); /* 불안정성을 보이는 반례 */
    for (size_t ai = 0; ai < ALGORITHM_COUNT; ai++) {
        CHECK(ALGORITHMS[ai].sort(NULL, 0, &stats));
        CHECK(stats.comparisons == 0 && stats.moves == 0 && stats.extraRecordBytes == 0);
    }
}

static void checkValidatorAndBenchmark(void) {
    const Record original[] = {{2, 0}, {1, 1}, {2, 2}};
    Record valid[] = {{1, 1}, {2, 0}, {2, 2}};
    Validation result;
    CHECK(validateResult(original, valid, 3, &result));
    CHECK(result.sorted && result.permutationPreserved && result.stableOnInput);
    Record unstable[] = {{1, 1}, {2, 2}, {2, 0}};
    CHECK(validateResult(original, unstable, 3, &result));
    CHECK(result.sorted && result.permutationPreserved && !result.stableOnInput);
    CHECK(validateResult(original, original, 3, &result));
    CHECK(!result.sorted && result.permutationPreserved);
    valid[2] = valid[1];
    CHECK(validateResult(original, valid, 3, &result));
    CHECK(!result.permutationPreserved); /* 소실/중복 검출 */
    valid[2] = (Record){2, 99};
    CHECK(validateResult(original, valid, 3, &result));
    CHECK(!result.permutationPreserved); /* 범위 밖 tag */
    valid[2] = (Record){9, 2};
    CHECK(validateResult(original, valid, 3, &result));
    CHECK(!result.permutationPreserved); /* 원소 값의 변조 */

    for (size_t ai = 0; ai < ALGORITHM_COUNT; ai++) {
        SortStats stats;
        double meanMs;
        CHECK(benchmark(&ALGORITHMS[ai], original, 3, 5, &stats, &result, &meanMs));
        CHECK(result.sorted && result.permutationPreserved && meanMs >= 0);
        CHECK(result.stableOnInput == ALGORITHMS[ai].stableByDesign);
        CHECK(!benchmark(&ALGORITHMS[ai], original, 3, 0, &stats, &result, &meanMs));
    }
}

int main(void) {
    exhaustiveSmallArrays();
    edgeAndGeneratedCases();
    checkCounters();
    checkValidatorAndBenchmark();
    printf("PASS: %zu input cases x 3 algorithms; %zu checks.\n", cases, checks);
    puts("Correctness, record preservation, stability, counters and benchmark verified.");
    return EXIT_SUCCESS;
}
