#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#define LOG_LEVEL 2
#include <stdio.h>

#define RUN_TEST(passed, failed, test_func, index) do { \
    printf("\nRunning Test #%d: " #test_func "\n", index); \
    (index++); \
    if (test_func() == -1) { \
        printf("❌ Failed: " #test_func "\n"); \
        (failed)++; \
    } else { \
        printf("✅ Passed: " #test_func "\n"); \
        (passed)++; \
    } \
} while(0)

#endif