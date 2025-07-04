#ifndef WFC_TEST_H_
#define WFC_TEST_H_

#include <cstdio>

static int test_count = 0;
static int pass_count = 0;

static void test_case(bool condition) {
  if (!condition) {
    printf("==> Test %d failed.\n", test_count);
  }
  else {
    printf("==> Test %d passed.\n", test_count);
    ++pass_count;
  }

  ++test_count;
}

static void test_results() {
  printf("-----------------------\n");
  if (pass_count == test_count) {
    printf("All tests passed.\n");
  }
  else {
    printf("%d out of %d tests passed.\n", pass_count, test_count);
  }
}

#endif // !WFC_TEST_H_
