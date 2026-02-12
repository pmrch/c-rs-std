#ifndef VEC_H
#define VEC_H

#include <stddef.h>
#include "result.h"

typedef enum {
    // Signed integers
    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64,
    
    // Unsigned integers
    TYPE_U8,
    TYPE_U16,
    TYPE_U32,
    TYPE_U64,
    
    // Floating point
    TYPE_F32,
    TYPE_F64,
    
    // Boolean (1 byte like Rust)
    TYPE_BOOL,
    
    // Character (1 byte like Rust)
    TYPE_CHAR,
} RustType;

typedef struct {
    void* data;             // Stored data
    size_t len;             // Number of elements
    size_t elem_size;       // Size of variable type in bytes
    size_t capacity;        // Max number of elements in Vec
} Vec;

size_t rust_type_size(const RustType type);

Result vec_clear(Vec *vec);
Result vec_pop(Vec *vec, void *out_item);
Result vec_new(Vec *restrict vec, const RustType type);
Result vec_push(Vec *restrict vec, const void *restrict item);

void vec_free(Vec *restrict vec);

const void *vec_get(const Vec *vec, size_t index);

#endif