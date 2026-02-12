#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "vec.h"
#include "logging.h"
#include "result.h"

size_t rust_type_size(const RustType type) {
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
            LOG_ERROR("rust_type_size: invalid RustType '%d' provided!", type);
            return 0;  // Or handle error
    }
}

static const char *rust_type_name(const RustType type) {
    switch (type) {
        case TYPE_I8:   return "i8";
        case TYPE_U8:   return "u8";
        case TYPE_BOOL: return "bool";
        case TYPE_CHAR: return "char";
        case TYPE_I16:  return "i16";
        case TYPE_U16:  return "u16";
        case TYPE_I32:  return "i32";
        case TYPE_U32:  return "u32";
        case TYPE_F32:  return "f32";
        case TYPE_I64:  return "i64";
        case TYPE_U64:  return "u64";
        case TYPE_F64:  return "f64";
        default:        return "unknown";
    }
}

Result vec_new(Vec *restrict vec, const RustType type) {
    const char* type_name = rust_type_name(type);
    LOG_INFO("Creating new Vec of type %s", type_name);

    size_t type_size = rust_type_size(type);
    if (type_size == 0)  {
        LOG_ERROR("new_vec: invalid type size (type = %s)", type_name);
        return RESULT_ERR(ERR_INVALID);
    }

    size_t initial_capacity = 8; 
    vec->data = malloc(initial_capacity * type_size);
    if (!vec->data) {  // Check malloc, not vec
        LOG_ERROR("new_vec: failed to allocate initial Vec buffer (size = %zu)", 
            initial_capacity * type_size);

        return RESULT_ERR(ERR_MEMORY);
    }

    vec->capacity = initial_capacity;
    vec->elem_size = type_size;
    vec->len = 0;

    LOG_DEBUG("new_vec: initialized Vec [elem_size=%zu, capacity=%zu, len=%zu]",
              vec->elem_size, vec->capacity, vec->len);

    return RESULT_OK();
}

Result vec_push(Vec *restrict vec, const void *restrict item) {
    // Checks whether the Vec is allocated and whether its data is
    // allocated.
    if (!vec || !item) { 
        LOG_ERROR("vec_push: invalid argument (vec or item is NULL)");
        return RESULT_ERR(ERR_INVALID); 
    }

    if (!vec->data) { 
        LOG_ERROR("vec_push: Vec data was not allocated!");
        return RESULT_ERR(ERR_INVALID); 
    }

    // Checks whether the next element would overflow in memory
    if (vec->len == vec ->capacity) {
        size_t new_capacity = vec->capacity * 2;
        LOG_DEBUG("vec_push: growing Vec from %zu to %zu elements", vec->capacity, new_capacity);

        // Temporarily clones original data and increases capacity
        char* temp = realloc(vec->data, new_capacity * vec->elem_size);

        // Checks whether realloc was successful
        if (!temp) {
            LOG_ERROR("vec_push: failed to realloc Vec (new capacity = %zu)", new_capacity);
            return RESULT_ERR(ERR_MEMORY);
        }

        // 
        vec->data = temp;
        vec->capacity = new_capacity;
    }

    char *dest = (char*)vec->data + (vec->len * vec->elem_size);
    memcpy(dest, item, vec->elem_size);
    vec->len++;

    return RESULT_OK();
}

Result vec_pop(Vec *vec, void *out_item) {
    if (!vec || !vec->data) {
        LOG_ERROR("vec_pop error: invalid argument (vec or its data is NULL) or not initialized!");
        return RESULT_ERR(ERR_INVALID);
    }

    if (!out_item) {
        LOG_ERROR("vec_pop error: out_item was NULL or uninitialized!");
        return RESULT_ERR(ERR_INVALID);
    }

    const void *last_item = vec_get(vec, vec->len - 1);
    if (!last_item) {
        LOG_ERROR("vec_pop error: Failed to get last element of Vec!");
        return RESULT_ERR(ERR_INVALID);
    }

    memcpy(out_item, last_item, vec->elem_size);

    vec->len--;
    LOG_INFO("Successfully popped last item of vec! New number of elements: %zu", vec->len);
    return RESULT_OK();
}

Result vec_clear(Vec *vec) {
    if (!vec || !vec->data) {
        LOG_ERROR("vec_clear: attempted access on uninitialized Vec");
        return RESULT_ERR(ERR_INVALID);
    }

    vec->len = 0;
    return RESULT_OK();
}

const void *vec_get(const Vec *vec, size_t index) {
    if (!vec || !vec->data) {
        LOG_ERROR("vec_get: attempted access on uninitialized Vec");
        return NULL;
    } else if (index > vec->len) {
        LOG_ERROR("vec_get: tried to access invalid index %zu (len=%zu)", index, vec->len);
        return NULL;
    }

    LOG_DEBUG("vec_get: returning element at index %zu", index);
    return (const char*)vec->data + index * vec->elem_size;
}

void vec_free(Vec *restrict vec) {
    if (!vec) {
        LOG_WARN("vec_free: called with NULL Vec pointer");
        return;
    }

    if (vec->data) {
        LOG_DEBUG("vec_free: freeing Vec with %zu elements (capacity=%zu)",
                  vec->len, vec->capacity);

        free(vec->data);
        vec->data = NULL;
    }

    vec->len = 0;
    vec->capacity = 0;

    LOG_INFO("vec_free: Vec successfully freed");
}
