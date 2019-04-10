
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int tests_failed = 0;
int tests_ran = 0;

int test(char* str, int assertion) {
    tests_ran += 1;

    if (assertion) {
        printf("passed : '%s'\n", str);
    } else {
        printf("FAILED : '%s'\n", str);
        tests_failed += 1;
    }

    return assertion;
}

int test_equal(char* str, int v1, int v2) {
    tests_ran += 1;

    if (v1 == v2) {
        printf("passed : '%s' (%i == %i)\n", str, v1, v2);
    } else {
        printf("FAILED : '%s' (%i != %i)\n", str, v1, v2);
        tests_failed += 1;
    }

    return v1 == v2;
}

#define TEST(x) test(#x, x)
#define TEST_EQ(x, y) test_equal(#x, x, y)

int main() {
    // real basics
    TEST(1 == 1);
    TEST(2 + 2 == 4);
    TEST_EQ(1 + 3, 4);

    // stdlib stuff
    TEST(strcmp("foo", "bar") != 0);
    TEST_EQ(printf("test"), 4);
    TEST_EQ(printf("test %i", 100),  8);

    // syscall stuff
    TEST(getpid() > 0);
    TEST_EQ(errno, SUCCESS);

    // other programs
    // mkpipe()
    // subprocess() -> fork(); exec(); wait()
    // TEST(subprocess("echo foo") == 0);

    return tests_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

