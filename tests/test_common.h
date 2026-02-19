#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdio.h>

#define RUN_TEST(passed, failed, test_func) do { \
    fprintf(stdout, "test " #test_func "..."); \
    fflush(stdout); \
    if (test_func() == -1) { \
        fprintf(stdout, " FAILED\n"); \
        (failed)++; \
    } else { \
        fprintf(stdout, " ok\n"); \
        (passed)++; \
    } \
} while(0)

#endif