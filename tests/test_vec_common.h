#ifndef TEST_VEC_COMMON_H
#define TEST_VEC_COMMON_H

#include "vec.h"

inline static int test_vec_new(RustType type) {
    Vec vec = {0};
    Result res = vec_new(&vec, type);
    int ok = !res.err && vec.data && vec.magic == VEC_MAGIC_INIT && vec.capacity > 0;
    vec_free(&vec);
    return ok ? 0 : -1;
}

inline static int test_vec_push(RustType type) {
    Vec vec = {0};
    
    Result res = vec_new(&vec, type);
    int ok_new = !res.err && vec.data && vec.magic == VEC_MAGIC_INIT && vec.capacity > 0;
    if (ok_new == 0) { return -1; }
    
    uint8_t pushed = 2;
    Result res_p = vec_push(&vec, &pushed);
    int ok_p = !res_p.err && vec.data && vec.magic == VEC_MAGIC_INIT && vec.capacity > 0;
    if (ok_p == 0 || ((uint8_t*)vec.data)[0] != pushed) { return -1; }
    
    vec_free(&vec);
    return 0;
}

// Functions to test creation of new Vec in all types
int create_vec_u8();    int create_vec_u16();
int create_vec_u32();   int create_vec_u64();

int create_vec_i8();    int create_vec_i16();
int create_vec_i32();   int create_vec_i64();

int create_vec_f32();   int create_vec_f64();
int create_vec_bool();  int create_vec_char();

// Functions to test pushing
int push_vec_one_u8();

#endif