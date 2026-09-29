#ifndef BENCH_H
#define BENCH_H

#include "sort.h"

typedef enum { RANDOM_INPUT, SORTED_INPUT, REVERSE_INPUT, FEW_UNIQUE_INPUT } InputShape;
extern const char *const INPUT_NAMES[4];

typedef struct {
    int sorted;
    int permutationPreserved;
    int stableOnInput;
    int hasEqualKeys;
} Validation;

void makeInput(Record a[], size_t n, InputShape shape, uint32_t seed);
/* input[i].tag == i가 전제다. 입력 생성 함수와 테스트가 이를 보장한다. */
int validateResult(const Record input[], const Record output[], size_t n,
                   Validation *result);
int benchmark(const SortAlgorithm *algorithm, const Record input[], size_t n,
              unsigned repetitions, SortStats *stats, Validation *validation,
              double *meanMs);

#endif
