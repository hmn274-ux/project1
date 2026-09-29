#include "bench.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char *const INPUT_NAMES[4] = {"random", "sorted", "reverse", "few_unique"};

/* 고정된 32비트 연산으로 플랫폼에 상관없이 같은 입력을 만든다. */
static uint32_t nextRandom(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

/* 실험 범위 n<=8000에서 사용. 난수의 나머지 연산은 미세한 편향이 있다. */
void makeInput(Record a[], size_t n, InputShape shape, uint32_t seed) {
    uint32_t state = seed ? seed : 1;
    for (size_t i = 0; i < n; i++) {
        if (shape == REVERSE_INPUT) a[i].key = (int)(n - 1 - i);
        else if (shape == FEW_UNIQUE_INPUT) a[i].key = (int)(nextRandom(&state) % 16);
        else a[i].key = (int)i;
    }
    if (shape == RANDOM_INPUT) {
        for (size_t i = n; i > 1; i--) {
            size_t j = nextRandom(&state) % i;
            int temp = a[i - 1].key;
            a[i - 1].key = a[j].key;
            a[j].key = temp;
        }
    }
    /* 섞은 뒤의 위치가 '원래 순서'다. 정렬 직전에 tag를 붙인다. */
    for (size_t i = 0; i < n; i++) a[i].tag = i;
}

int validateResult(const Record input[], const Record output[], size_t n,
                   Validation *result) {
    *result = (Validation){1, 1, 1, 0};
    if (n == 0) return 1;
    unsigned char *seen = calloc(n, sizeof(*seen));
    if (!seen) return 0;
    for (size_t i = 0; i < n; i++) {
        size_t tag = output[i].tag;
        if (tag >= n || seen[tag] || input[tag].tag != tag ||
            input[tag].key != output[i].key) {
            result->permutationPreserved = 0;
        } else seen[tag] = 1;
        if (i > 0) {
            if (output[i - 1].key > output[i].key) result->sorted = 0;
            if (output[i - 1].key == output[i].key) {
                result->hasEqualKeys = 1;
                if (output[i - 1].tag > output[i].tag) result->stableOnInput = 0;
            }
        }
    }
    free(seen);
    return 1;
}

static int sameStats(const SortStats *a, const SortStats *b) {
    return a->comparisons == b->comparisons && a->moves == b->moves &&
           a->extraRecordBytes == b->extraRecordBytes &&
           a->maxRecursionDepth == b->maxRecursionDepth;
}

int benchmark(const SortAlgorithm *algorithm, const Record input[], size_t n,
              unsigned repetitions, SortStats *stats, Validation *validation,
              double *meanMs) {
    if (repetitions == 0 || n > SIZE_MAX / sizeof(Record)) return 0;
    Record *work = n ? malloc(n * sizeof(*work)) : NULL;
    if (n && !work) return 0;
    double totalMs = 0;
    for (unsigned r = 0; r < repetitions; r++) {
        if (n) memcpy(work, input, n * sizeof(*work)); /* 시간 측정에서 제외 */
        SortStats current;
        clock_t start = clock();
        int ok = algorithm->sort(work, n, &current);
        clock_t end = clock();
        if (!ok || start == (clock_t)-1 || end == (clock_t)-1 || end < start) {
            free(work);
            return 0;
        }
        totalMs += 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
        Validation check;
        if (!validateResult(input, work, n, &check) || !check.sorted ||
            !check.permutationPreserved ||
            (algorithm->stableByDesign && !check.stableOnInput)) {
            free(work);
            return 0;
        }
        if (r == 0) {
            *stats = current;
            *validation = check;
        } else if (!sameStats(stats, &current)) {
            free(work);
            return 0;
        }
    }
    *meanMs = totalMs / repetitions;
    free(work);
    return 1;
}
