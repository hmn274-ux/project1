#include "bench.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef BUILD_FLAGS
#define BUILD_FLAGS "not recorded"
#endif

static const size_t SIZES[] = {1000, 2000, 4000, 8000};
static const unsigned REPETITIONS = 5;
static const uint32_t BASE_SEED = 20260930;

static void printRecords(const Record a[], size_t n) {
    for (size_t i = 0; i < n; i++) {
        printf("%s(%d,%zu)", i ? " " : "", a[i].key, a[i].tag);
    }
    putchar('\n');
}

static int stabilityDemo(void) {
    const Record input[] = {{2, 0}, {1, 1}, {2, 2}};
    puts("Each record is (key, original_position); compare key ONLY.");
    printf("input     : ");
    printRecords(input, 3);
    for (size_t i = 0; i < ALGORITHM_COUNT; i++) {
        Record work[3];
        memcpy(work, input, sizeof(work));
        SortStats stats;
        Validation result;
        if (!ALGORITHMS[i].sort(work, 3, &stats) ||
            !validateResult(input, work, 3, &result) ||
            !result.sorted || !result.permutationPreserved) return 0;
        printf("%-10s: ", ALGORITHMS[i].name);
        printRecords(work, 3);
        printf("  sorted=yes, same records=yes, equal-key order=%s\n",
               result.stableOnInput ? "preserved" : "changed");
    }
    puts("One preserved example does not prove stability for all inputs.");
    return 1;
}

static void printEnvironment(void) {
    puts("implementation: C17, instrumented sorting (counters included in time)");
#ifdef __VERSION__
    printf("compiler: %s\n", __VERSION__);
#endif
    printf("build_flags: %s\n", BUILD_FLAGS);
    printf("sizeof_Record: %zu bytes (includes padding)\n", sizeof(Record));
    printf("sizeof_int: %zu; sizeof_size_t: %zu\n", sizeof(int), sizeof(size_t));
    printf("clock: process CPU time; CLOCKS_PER_SEC=%ld\n", (long)CLOCKS_PER_SEC);
    printf("base_seed: %" PRIu32 "; case_seed=base_seed+n\n", BASE_SEED);
    printf("repetitions: %u (same input on each repeat)\n", REPETITIONS);
    puts("random generator: xorshift32; shuffle: Fisher-Yates using modulo");
    puts("few_unique keys: 0..15; other shapes: permutation of 0..n-1");
    puts("extra_record_bytes excludes indices, pointers, call stack and benchmark buffers");
}

static int runExperiments(int csv) {
    if (csv) {
        puts("input,n,algorithm,repetitions,mean_ms,comparisons,moves,extra_record_bytes,max_recursion_depth,sorted,permutation_preserved,equal_key_order");
    } else {
        puts("Same input, 5 repeats; time=instrumented sort CPU time (milliseconds).");
        puts("Equal-order: yes/no for this input only; n/a means no duplicate keys.");
        printf("%-11s %5s %-9s %10s %12s %12s %9s %5s %7s\n",
               "input", "n", "sort", "mean_ms", "comparisons", "moves",
               "extra_B", "depth", "order");
    }
    for (size_t si = 0; si < sizeof(SIZES) / sizeof(SIZES[0]); si++) {
        size_t n = SIZES[si];
        Record *input = malloc(n * sizeof(*input));
        if (!input) return 0;
        for (int shape = RANDOM_INPUT; shape <= FEW_UNIQUE_INPUT; shape++) {
            makeInput(input, n, (InputShape)shape, BASE_SEED + (uint32_t)n);
            for (size_t ai = 0; ai < ALGORITHM_COUNT; ai++) {
                SortStats stats;
                Validation result;
                double meanMs;
                if (!benchmark(&ALGORITHMS[ai], input, n, REPETITIONS,
                               &stats, &result, &meanMs)) {
                    fprintf(stderr, "Failed: %s, n=%zu, %s\n",
                            INPUT_NAMES[shape], n, ALGORITHMS[ai].name);
                    free(input);
                    return 0;
                }
                const char *order = !result.hasEqualKeys ? "n/a" :
                                   (result.stableOnInput ? "yes" : "no");
                if (csv) {
                    printf("%s,%zu,%s,%u,%.6f,%" PRIu64 ",%" PRIu64
                           ",%zu,%zu,yes,yes,%s\n", INPUT_NAMES[shape], n,
                           ALGORITHMS[ai].name, REPETITIONS, meanMs,
                           stats.comparisons, stats.moves, stats.extraRecordBytes,
                           stats.maxRecursionDepth, order);
                } else {
                    printf("%-11s %5zu %-9s %10.6f %12" PRIu64 " %12" PRIu64
                           " %9zu %5zu %7s\n", INPUT_NAMES[shape], n,
                           ALGORITHMS[ai].name, meanMs, stats.comparisons,
                           stats.moves, stats.extraRecordBytes,
                           stats.maxRecursionDepth, order);
                }
            }
        }
        free(input);
    }
    if (!csv) puts("All rows passed sorting correctness and record-preservation checks.");
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc == 1) return runExperiments(0) ? EXIT_SUCCESS : EXIT_FAILURE;
    if (argc == 2) {
        if (strcmp(argv[1], "--csv") == 0)
            return runExperiments(1) ? EXIT_SUCCESS : EXIT_FAILURE;
        if (strcmp(argv[1], "--demo") == 0)
            return stabilityDemo() ? EXIT_SUCCESS : EXIT_FAILURE;
        if (strcmp(argv[1], "--environment") == 0) {
            printEnvironment();
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[1], "--help") == 0) {
            puts("Usage: ./src/main.out [--csv | --demo | --environment | --help]");
            return EXIT_SUCCESS;
        }
    }
    fprintf(stderr, "Unknown option; use --help.\n");
    return EXIT_FAILURE;
}
