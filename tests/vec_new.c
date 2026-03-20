#include <bits/time.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "test_common.h"
#include "test_vec_common.h"
#include "vec.h"

int create_vec_u8() { return test_vec_new(TYPE_U8); }
int create_vec_u16() { return test_vec_new(TYPE_U16); }
int create_vec_u32() { return test_vec_new(TYPE_U32); }
int create_vec_u64() { return test_vec_new(TYPE_U64); }

int create_vec_i8() { return test_vec_new(TYPE_I8); }
int create_vec_i16() { return test_vec_new(TYPE_I16); }
int create_vec_i32() { return test_vec_new(TYPE_I32); }
int create_vec_i64() { return test_vec_new(TYPE_I64); }

int create_vec_f32() { return test_vec_new(TYPE_F32); }
int create_vec_f64() { return test_vec_new(TYPE_F64); }

int create_vec_bool() { return test_vec_new(TYPE_BOOL); }
int create_vec_char() { return test_vec_new(TYPE_CHAR); }

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    struct timespec start, end;

    int passed = 0;
    int failed = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    RUN_TEST(passed, failed, create_vec_u8);
    RUN_TEST(passed, failed, create_vec_u16);
    RUN_TEST(passed, failed, create_vec_u32);
    RUN_TEST(passed, failed, create_vec_u64);

    RUN_TEST(passed, failed, create_vec_i8);
    RUN_TEST(passed, failed, create_vec_i16);
    RUN_TEST(passed, failed, create_vec_i32);
    RUN_TEST(passed, failed, create_vec_i64);

    RUN_TEST(passed, failed, create_vec_f32);
    RUN_TEST(passed, failed, create_vec_f64);

    RUN_TEST(passed, failed, create_vec_bool);
    RUN_TEST(passed, failed, create_vec_char);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (double)(end.tv_sec - start.tv_sec) * 1000.0 +
                     (double)(end.tv_nsec - start.tv_nsec) / 1e6;

    fprintf(stdout,
            "\ntest result: %s. %d passed; %d failed; finished in %.3fs\n",
            failed > 0 ? "FAILED" : "ok", passed, failed, elapsed);

    fprintf(stdout,
            "Finished tests for vec_new()!\nPress any key to continue...\n");

    getchar();
    system("clear");

    return failed > 0 ? -1 : 0;
}