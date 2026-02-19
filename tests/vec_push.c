#include <bits/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "vec.h"
#include "test_common.h"
#include "test_vec_common.h"

#define RUN_VEC_PUSH_TESTS_TYPE(passed, failed, name) \
    RUN_TEST(passed, failed, push_vec_one_##name); \
    RUN_TEST(passed, failed, push_vec_some_##name); \
    RUN_TEST(passed, failed, push_vec_alot_##name);

#define RUN_VEC_PUSH_TESTS_UNSIGNED(passed, failed) \
    RUN_VEC_PUSH_TESTS_TYPE(passed, failed, u8);  \
    RUN_VEC_PUSH_TESTS_TYPE(passed, failed, u16); \
    RUN_VEC_PUSH_TESTS_TYPE(passed, failed, u32); \
    RUN_VEC_PUSH_TESTS_TYPE(passed, failed, u64); \

#define RUN_VEC_PUSH_TESTS_SIGNED(passed, failed) \
    RUN_VEC_PUSH_TESTS_TYPE(passed, failed, i8);  \
    RUN_VEC_PUSH_TESTS_TYPE(passed, failed, i16); \
    RUN_VEC_PUSH_TESTS_TYPE(passed, failed, i32); \
    RUN_VEC_PUSH_TESTS_TYPE(passed, failed, i64); \

#define RUN_VEC_PUSH_TESTS_FLOAT(passed, failed) \
    RUN_VEC_PUSH_TESTS_TYPE(passed, failed, f32); \
    RUN_VEC_PUSH_TESTS_TYPE(passed, failed, f64); \

#define RUN_VEC_PUSH_TESTS_MISC(passed, failed) \
    RUN_VEC_PUSH_TESTS_TYPE(passed, failed, bool); \
    RUN_VEC_PUSH_TESTS_TYPE(passed, failed, char); \

#define RUN_ALL_VEC_PUSH_TEST(passed, failed) \
    RUN_VEC_PUSH_TESTS_UNSIGNED(passed, failed); \
    RUN_VEC_PUSH_TESTS_SIGNED(passed, failed); \
    RUN_VEC_PUSH_TESTS_FLOAT(passed, failed); \
    RUN_VEC_PUSH_TESTS_MISC(passed, failed); \

#define DEFINE_PUSH_TESTS_UNSIGNED() \
    DEFINE_PUSH_TESTS(u8, TYPE_U8) \
    DEFINE_PUSH_TESTS(u16, TYPE_U16) \
    DEFINE_PUSH_TESTS(u32, TYPE_U32) \
    DEFINE_PUSH_TESTS(u64, TYPE_U64)

#define DEFINE_PUSH_TESTS_SIGNED() \
    DEFINE_PUSH_TESTS(i8, TYPE_I8) \
    DEFINE_PUSH_TESTS(i16, TYPE_I16) \
    DEFINE_PUSH_TESTS(i32, TYPE_I32) \
    DEFINE_PUSH_TESTS(i64, TYPE_I64) 

#define DEFINE_PUSH_TESTS_FLOAT() \
    DEFINE_PUSH_TESTS(f32, TYPE_F32) \
    DEFINE_PUSH_TESTS(f64, TYPE_F64) 

#define DEFINE_PUSH_TESTS_MISC() \
    DEFINE_PUSH_TESTS(char, TYPE_CHAR) \
    DEFINE_PUSH_TESTS(bool, TYPE_BOOL) 

DEFINE_PUSH_TESTS_UNSIGNED()
DEFINE_PUSH_TESTS_SIGNED()
DEFINE_PUSH_TESTS_FLOAT()
DEFINE_PUSH_TESTS_MISC()

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    
    int passed = 0;
    int failed = 0;

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    RUN_ALL_VEC_PUSH_TEST(passed, failed);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (double)(end.tv_sec - start.tv_sec) * 1000.0 
        + (double)(end.tv_nsec - start.tv_nsec) / 1e6;

    fprintf(stdout, "\ntest result: %s. %d passed; %d failed; finished in %.3fs\n",
        failed > 0 ? "FAILED" : "ok", passed, failed, elapsed / 1000.0);

    fprintf(stdout, "Finished tests for vec_new()!\nPress any key to continue...\n");
    
    getchar();
    system("clear");
    return 0;
}