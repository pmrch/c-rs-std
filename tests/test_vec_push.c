#include "vec.h"
#include "result.h"
#include "test_common.h"
#include "test_vec_common.h"

#include <stdint.h>
#include <stdio.h>


DEFINE_PUSH_TESTS(u8, TYPE_U8)

int main() {
    int passed = 0;
    int failed = 0;
    int index = 1;

    RUN_TEST(passed, failed, push_vec_one_u8, index);
    RUN_TEST(passed, failed, push_vec_some_u8, index);
    return 0;
}