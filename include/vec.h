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
    void* data;
    size_t len;
    size_t type_size;
    size_t total_capacity;
} Vec;

size_t rust_type_size(RustType type);

Result new_vec(Vec *vec, const RustType type);
Result vec_push(Vec *vec, const void *item);

#endif