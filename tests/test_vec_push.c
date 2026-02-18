#include "vec.h"
#include "result.h"
#include "test_common.h"
#include "test_vec_common.h"

#include <stdint.h>
#include <stdio.h>

int push_vec_one_u8() { return test_vec_push(TYPE_U8); }

int main() {
    int passed = 0;
    int failed = 0;
    int index = 1;

    RUN_TEST(passed, failed, push_vec_one_u8, index);
    return 0;
}