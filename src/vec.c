#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "vec.h"
#include "result.h"

size_t rust_type_size(RustType type) {
    switch (type) {
        case TYPE_I8:
        case TYPE_U8:
        case TYPE_BOOL:
        case TYPE_CHAR:
            return 1;
        
        case TYPE_I16:
        case TYPE_U16:
            return 2;
        
        case TYPE_I32:
        case TYPE_U32:
        case TYPE_F32:
            return 4;
        
        case TYPE_I64:
        case TYPE_U64:
        case TYPE_F64:
            return 8;
        
        default:
            perror("[Error]: Invalid Type Provided!\n");
            return 0;  // Or handle error
    }
}

Result new_vec(Vec *vec, const RustType type) {
    size_t type_size = rust_type_size(type);
    size_t allocated = type_size * 8;

    vec->data = malloc(allocated);
    if (!vec->data) {  // Check malloc, not vec
        perror("Failed to allocate memory for new Vec\n");
        return RESULT_ERR(ERR_MEMORY);
    }

    vec->total_capacity = allocated;
    vec->type_size = type_size;
    vec->len = 0;

    return RESULT_OK();
}

Result vec_push(Vec *vec, const void *item) {
    if (!vec || !item) { return RESULT_ERR(ERR_INVALID); }
    if (!vec->data) { return RESULT_ERR(ERR_INVALID); }

    printf("data ptr: %p, capacity: %zu, len: %zu\n", 
          vec->data, vec->total_capacity, vec->len);

    char *dest = (char*)vec->data + (vec->len * vec->type_size);
    printf("Writing to offset: %zu (len=%zu * type_size=%zu)\n",
        vec->len * vec->type_size, vec->len, vec->type_size);
    
    printf("item ptr: %p, copying %zu bytes\n", item, vec->type_size);
    printf("dest ptr: %p\n", dest);
    memcpy(dest, item, vec->type_size);
    vec->len++;

    return RESULT_OK();
}