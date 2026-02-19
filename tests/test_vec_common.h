#ifndef TEST_VEC_COMMON_H
#define TEST_VEC_COMMON_H

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "result.h"
#include "vec.h"


// --- Core test functions ---
inline static int test_vec_new(RustType type);
inline static int test_vec_push_single(RustType type);
inline static int test_vec_push_some(RustType type);
inline static int test_vec_push_alot(RustType type);

inline static bool matches(const RustType type, void *val1, void *val2);

#define RUSTTYPE_MAP_INT \
    X(TYPE_U8,  uint8_t,  UINT8_MAX)  \
    X(TYPE_U16, uint16_t, UINT16_MAX) \
    X(TYPE_U32, uint32_t, UINT32_MAX) \
    X(TYPE_U64, uint64_t, UINT64_MAX) \
    X(TYPE_I8,  int8_t,   INT8_MAX)   \
    X(TYPE_I16, int16_t,  INT16_MAX)  \
    X(TYPE_I32, int32_t,  INT32_MAX)  \
    X(TYPE_I64, int64_t,  INT64_MAX)  \
    X(TYPE_BOOL, bool,    BOOL_MAX)   \
    X(TYPE_CHAR, char,    CHAR_MAX)

#define RUSTTYPE_MAP_FLOAT \
    X(TYPE_F32, float)  \
    X(TYPE_F64, double)

// --- Macro to declare test functions for a type ---
#define DECLARE_CREATE_TESTS() \
    int create_vec_u8();    int create_vec_u16();  \
    int create_vec_u32();   int create_vec_u64();  \
    int create_vec_i8();    int create_vec_i16();  \
    int create_vec_i32();   int create_vec_i64();  \
    int create_vec_f32();   int create_vec_f64();  \
    int create_vec_bool();  int create_vec_char(); 

#define DECLARE_PUSH_TESTS(name, type) \
    int push_vec_one_##name(void); \
    int push_vec_some_##name(void); \
    int push_vec_alot_##name(void);

#define DEFINE_PUSH_TESTS(name, type) \
    int push_vec_one_##name(void)  { return test_vec_push_single(type); } \
    int push_vec_some_##name(void) { return test_vec_push_some(type);   } \
    int push_vec_alot_##name(void) { return test_vec_push_alot(type);   } \

#define DECLARE_PUSH_UNSIGNED() \
    DECLARE_PUSH_TESTS(u8, TYPE_U8) \
    DECLARE_PUSH_TESTS(u16, TYPE_U16) \
    DECLARE_PUSH_TESTS(u32, TYPE_U32) \
    DECLARE_PUSH_TESTS(u64, TYPE_U64)

#define DECLARE_PUSH_SIGNED() \
    DECLARE_PUSH_TESTS(i8, TYPE_I8) \
    DECLARE_PUSH_TESTS(i16, TYPE_I16) \
    DECLARE_PUSH_TESTS(i32, TYPE_I32) \
    DECLARE_PUSH_TESTS(i64, TYPE_I64)

#define DECLARE_PUSH_FLOAT() \
    DECLARE_PUSH_TESTS(f32, TYPE_F32) \
    DECLARE_PUSH_TESTS(f64, TYPE_F64)

#define DECLARE_PUSH_MISC() \
    DECLARE_PUSH_TESTS(char, TYPE_CHAR) \
    DECLARE_PUSH_TESTS(bool, TYPE_BOOL)

#define DECLARE_ALL_PUSH() \
    DECLARE_PUSH_UNSIGNED() \
    DECLARE_PUSH_SIGNED() \
    DECLARE_PUSH_FLOAT() \
    DECLARE_PUSH_MISC()

#define TEST_VEC_PUSH_STRESS_INT(c_type, rust_type, num_iter, type_max) do { \
    Vec vec = {0}; \
    Result res = vec_new(&vec, rust_type); \
    int ok_new = !res.err && vec.data && vec.magic == VEC_MAGIC_INIT && vec.capacity > 0; \
    if (ok_new == 0) { return -1; } \
    \
    srand((uint32_t)time(NULL)); \
    for (size_t i = 0; i < (size_t)num_iter; i++) { \
        if (type_max == 0) { \
            return 0; \
        }\
        assert(type_max != 0); \
        c_type pushed = (c_type)((c_type)(rand()) % ((c_type)(type_max))); \
        Result res_p = vec_push(&vec, &pushed); \
        \
        int ok_p = !res_p.err && vec.data && vec.magic == VEC_MAGIC_INIT && vec.capacity > 0; \
        c_type *item = &((c_type*)vec.data)[i]; \
        \
        bool m = false; \
        m = (size_t)*item != (size_t)pushed; \
        if (ok_p == 0 || !item || m) { \
            printf("Failed at: %zu\n", i); \
            vec_free(&vec); \
            return -1; \
        } \
    } \
    vec_free(&vec); \
} while(0)

#define TEST_VEC_PUSH_STRESS_FLOAT(c_type, rust_type, num_iter) do { \
    Vec vec = {0}; \
    Result res = vec_new(&vec, rust_type); \
    int ok_new = !res.err && vec.data && vec.magic == VEC_MAGIC_INIT && vec.capacity > 0; \
    if (ok_new == 0) { return -1; } \
    srand((uint32_t)time(NULL)); \
    for (size_t i = 0; i < (size_t)num_iter; i++) { \
        c_type pushed = (c_type)((double)rand() / (double)RAND_MAX); \
        Result res_p = vec_push(&vec, &pushed); \
        int ok_p = !res_p.err && vec.data && vec.magic == VEC_MAGIC_INIT; \
        c_type *item = &((c_type*)vec.data)[i]; \
        bool m = matches(rust_type, item, &pushed); \
        if (ok_p == 0 || !item || !m) { \
            printf("Failed at: %zu\n", i); \
            vec_free(&vec); \
            return -1; \
        } \
    } \
    vec_free(&vec); \
} while(0)

inline static bool matches(const RustType type, void *val1, void *val2) {
    if (!val1 || !val2) { return false; }

    if (type == TYPE_F32) {
        float *item1 = (float*)val1;
        float *item2 = (float*)val2;

        return fabsf(*item1 - *item2) <= 1e-6f;
    } else {
        double *item1 = (double*)val1;
        double *item2 = (double*)val2;

        return fabs(*item1 - *item2) <= 1e-10;
    }
}

inline static int test_vec_new(RustType type) {
    Vec vec = {0};
    Result res = vec_new(&vec, type);
    int ok = !res.err && vec.data && vec.magic == VEC_MAGIC_INIT && vec.capacity > 0;
    vec_free(&vec);
    return ok ? 0 : -1;
}

inline static int test_vec_push_single(RustType type) {
    switch (type) {
        // Integer types
        #define X(type_enum, ctype, maxval) \
            case type_enum: TEST_VEC_PUSH_STRESS_INT(ctype, type_enum, 1, maxval); break;
        RUSTTYPE_MAP_INT
        #undef X

        // Float types  
        #define X(type_enum, ctype) \
            case type_enum: TEST_VEC_PUSH_STRESS_FLOAT(ctype, type_enum, 1); break;
        RUSTTYPE_MAP_FLOAT
        #undef X

        default:
            printf("Unknown RustType %d\n", type);
            return -1;
    }

    return 0;
}

inline static int test_vec_push_some(RustType type) {
    switch (type) {
        // Integer types
        // 1. X is defined for integer case
        #define X(type_enum, ctype, maxval) \
            case type_enum: TEST_VEC_PUSH_STRESS_INT(ctype, type_enum, 1000, maxval); break;

        // 2. RUSTTYPE_MAP_INT expands, substituting X for each entry
        RUSTTYPE_MAP_INT
        /* becomes:
         * case TYPE_U8:  TEST_VEC_PUSH_STRESS_INT(uint8_t,  TYPE_U8,  1000, 255);    break;
         * case TYPE_U16: TEST_VEC_PUSH_STRESS_INT(uint16_t, TYPE_U16, 1000, UINT16_MAX); break;
         * ... etc
        */

        // 3. X is undefined
        #undef X

        // Float types
        // 4. X is redefined for float case with DIFFERENT signature
        #define X(type_enum, ctype) \
            case type_enum: TEST_VEC_PUSH_STRESS_FLOAT(ctype, type_enum, 1000); break;

        // 5. RUSTTYPE_MAP_FLOAT expands with new X
        RUSTTYPE_MAP_FLOAT
        /* becomes:
         * case TYPE_F32: TEST_VEC_PUSH_STRESS_FLOAT(float,  TYPE_F32, 1000); break;
         * case TYPE_F64: TEST_VEC_PUSH_STRESS_FLOAT(double, TYPE_F64, 1000); break;
        */

        // 6. X undefined again
        #undef X

        default:
            printf("Unknown RustType %d\n", type);
            return -1;
    }

    return 0;
}

inline static int test_vec_push_alot(RustType type) {
    switch (type) {
        // Integer types
        // 1. X is defined for integer case
        #define X(type_enum, ctype, maxval) \
            case type_enum: TEST_VEC_PUSH_STRESS_INT(ctype, type_enum, 14e7, maxval); break;

        // 2. RUSTTYPE_MAP_INT expands, substituting X for each entry
        RUSTTYPE_MAP_INT
        /* becomes:
         * case TYPE_U8:  TEST_VEC_PUSH_STRESS_INT(uint8_t,  TYPE_U8,  1000, 255);    break;
         * case TYPE_U16: TEST_VEC_PUSH_STRESS_INT(uint16_t, TYPE_U16, 1000, UINT16_MAX); break;
         * ... etc
        */

        // 3. X is undefined
        #undef X

        #define X(type_enum, ctype) \
            case type_enum: TEST_VEC_PUSH_STRESS_FLOAT(ctype, type_enum, 14e7); break;

        RUSTTYPE_MAP_FLOAT

        #undef X

        default:
            printf("Unknown RustType %d\n", type);
            return -1;
    }

    return 0;
}

// Functions to test creation of new Vec in all types
DECLARE_CREATE_TESTS()

// Functions to test pushing
DECLARE_ALL_PUSH()

#endif