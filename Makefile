CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -O2
CPPFLAGS = -Isrc
SORT_SRC = src/sort.c src/insertionSort.c src/mergeSort.c src/heapSort.c
COMMON_SRC = $(SORT_SRC) src/bench.c
HEADERS = src/sort.h src/bench.h

.PHONY: all run test demo results debug sanitize clean
all: src/main.out

src/main.out: src/main.c $(COMMON_SRC) $(HEADERS) Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) -DBUILD_FLAGS='"$(CFLAGS)"' -o $@ src/main.c $(COMMON_SRC)

tests/test_sort.out: tests/test_sort.c $(COMMON_SRC) $(HEADERS) Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ tests/test_sort.c $(COMMON_SRC)

run: src/main.out
	@./src/main.out

demo: src/main.out
	@./src/main.out --demo

test: tests/test_sort.out
	@./tests/test_sort.out

results: src/main.out
	@python3 tools/run_experiment.py

debug: src/main.debug.out
src/main.debug.out: src/main.c $(COMMON_SRC) $(HEADERS) Makefile
	$(CC) $(CPPFLAGS) -std=c17 -Wall -Wextra -Wpedantic -O0 -g -o $@ src/main.c $(COMMON_SRC)

sanitize:
	$(CC) $(CPPFLAGS) -std=c17 -Wall -Wextra -Wpedantic -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -o tests/test_sanitize.out tests/test_sort.c $(COMMON_SRC)
	@./tests/test_sanitize.out

clean:
	rm -f src/*.out tests/*.out
