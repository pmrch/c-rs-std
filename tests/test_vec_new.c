#include <stddef.h>
#include <stdio.h>

#include "test_common.h"
#include "test_vec_common.h"
#include "vec.h"

int create_vec_u8()   { return test_vec_new(TYPE_U8); }
int create_vec_u16()  { return test_vec_new(TYPE_U16); }
int create_vec_u32()  { return test_vec_new(TYPE_U32); }
int create_vec_u64()  { return test_vec_new(TYPE_U64); }

int create_vec_i8()   { return test_vec_new(TYPE_I8);  }
int create_vec_i16()  { return test_vec_new(TYPE_I16); }
int create_vec_i32()  { return test_vec_new(TYPE_I32); }
int create_vec_i64()  { return test_vec_new(TYPE_I64); }

int create_vec_f32()  { return test_vec_new(TYPE_F32); }
int create_vec_f64()  { return test_vec_new(TYPE_F32); }

int create_vec_bool() { return test_vec_new(TYPE_BOOL); }
int create_vec_char() { return test_vec_new(TYPE_CHAR); }

int main(void) {
    int passed = 0;     
    int failed = 0;
    int index = 1;

    RUN_TEST(passed, failed, create_vec_u8,  index);
    RUN_TEST(passed, failed, create_vec_u16, index);
    RUN_TEST(passed, failed, create_vec_u32, index);
    RUN_TEST(passed, failed, create_vec_u64, index);

    RUN_TEST(passed, failed, create_vec_i8,  index);
    RUN_TEST(passed, failed, create_vec_i16, index);
    RUN_TEST(passed, failed, create_vec_i32, index);
    RUN_TEST(passed, failed, create_vec_i64, index);

    RUN_TEST(passed, failed, create_vec_f32, index);
    RUN_TEST(passed, failed, create_vec_f64, index);

    RUN_TEST(passed, failed, create_vec_bool, index);
    RUN_TEST(passed, failed, create_vec_char, index);

    printf("\nFailed: %d\nPassed: %d\n", failed, passed);
    printf(failed > 0 ? "❌ Some tests failed\n" : "✅ All vec_new tests passed\n");
    return failed > 0 ? -1 : 0;
}