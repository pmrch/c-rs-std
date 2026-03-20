/*
 * vec.c - Dynamic Array (Vector) Implementation in C
 *
 * PURPOSE: This file implements a type-safe, dynamically-resizable array
 * container similar to Rust's Vec<T> or C++'s std::vector. It provides a
 * growable array that can hold elements of various Rust-like primitive types.
 *
 * WHY THIS EXISTS: C doesn't have built-in dynamic arrays or generics. This
 * provides a reusable container that:
 *   1. Automatically grows when you add elements (no manual realloc management)
 *   2. Maintains type information (element size) for proper memory management
 *   3. Provides bounds checking and error handling via Result types
 *   4. Offers a safer, more Rust-like API for C programming
 */

#include "vec.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "result.h"

// Map RustType enums to their byte sizes
size_t rust_type_size(const RustType type) {
    switch (type) {
        // 1-byte types: single byte integers, booleans, and chars
        case TYPE_I8:    // Signed 8-bit integer
        case TYPE_U8:    // unsigned 8-bit integer
        case TYPE_BOOL:  // boolean (true/false)
        case TYPE_CHAR:  // single character
            return 1;

        // 2-byte types: short integers
        case TYPE_I16:  // signed 16-bit integer
        case TYPE_U16:  // unsigned 16-bit integer
            return 2;

        // 4-byte types: standard integers and single-precision floats
        case TYPE_I32:  // signed 32-bit integer
        case TYPE_U32:  // unsigned 32-bit integer
        case TYPE_F32:  // 32-bit floating point
            return 4;

        // 8-byte types: large integers and double-precision floats
        case TYPE_I64:  // signed 64-bit integer
        case TYPE_U64:  // unsigned 64-bit integer
        case TYPE_F64:  // 64-bit floating point (double)
            return 8;

        // Invalid type - should never happen in correct usage
        default:
            LOG_ERROR("rust_type_size: invalid RustType '%d' provided!", type);
            return 0;  // Or handle error
    }
}

/* Growth strategy: 1.5x current capacity for amortized O(1) push performance.
 * Checks for overflow on extremely large vectors. */
size_t grow_capacity(const size_t current_capacity) {
    // Check if growing by 1.5x would overflow size_t
    if (current_capacity > SIZE_MAX / 3 * 2) {
        return SIZE_MAX;  // Cap at maximum possible size
    }

    // Grow by 50%: new_size = current + (current / 2) = current * 1.5
    return current_capacity + (current_capacity >> 1);
}

/* Human-readable Rust-adjacent type names for logging */
static const char* rust_type_name(const RustType type) {
    switch (type) {
        case TYPE_I8:
            return "i8";
        case TYPE_U8:
            return "u8";
        case TYPE_BOOL:
            return "bool";
        case TYPE_CHAR:
            return "char";
        case TYPE_I16:
            return "i16";
        case TYPE_U16:
            return "u16";
        case TYPE_I32:
            return "i32";
        case TYPE_U32:
            return "u32";
        case TYPE_F32:
            return "f32";
        case TYPE_I64:
            return "i64";
        case TYPE_U64:
            return "u64";
        case TYPE_F64:
            return "f64";
        default:
            return "unknown";
    }
}

/* Initialize vector with initial capacity of 8 elements */
Result vec_new(Vec* restrict vec, const RustType type) {
    const char* type_name = rust_type_name(type);
    LOG_INFO("Creating new Vec of type %s", type_name);

    size_t type_size = rust_type_size(type);
    if (type_size == 0) {
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

    LOG_DEBUG("new_vec: initialized Vec [elem_size=%zu, capacity=%zu, len=%zu]", vec->elem_size,
              vec->capacity, vec->len);

    vec->magic = VEC_MAGIC_INIT;  // Mark as initialized
    return RESULT_OK();
}

/* Add element to end of vector, growing capacity if needed */
Result vec_push(Vec* restrict vec, const void* item) {
    // Checks whether the Vec is allocated and whether its data is
    // allocated.
    if (!vec || !item || vec->magic != VEC_MAGIC_INIT) {
        LOG_ERROR("vec_push: invalid argument (vec or item is NULL)");
        return RESULT_ERR(ERR_INVALID);
    }

    if (!vec->data) {
        LOG_ERROR("vec_push: Vec data was not allocated!");
        return RESULT_ERR(ERR_INVALID);
    }

    // Checks whether the next element would overflow in memory
    if (vec->len == vec->capacity) {
        size_t new_capacity = grow_capacity(vec->capacity);
        // LOG_DEBUG("vec_push: growing Vec from %zu to %zu elements",
        // vec->capacity, new_capacity);

        // Temporarily clones original data and increases capacity
        void* temp = realloc(vec->data, new_capacity * vec->elem_size);

        // Checks whether realloc was successful
        if (!temp) {
            LOG_ERROR("vec_push: failed to realloc Vec (new capacity = %zu)", new_capacity);
            return RESULT_ERR(ERR_MEMORY);
        }

        vec->data = temp;
        vec->capacity = new_capacity;
    }

    char* dest = (char*)vec->data + (vec->len * vec->elem_size);
    memcpy(dest, item, vec->elem_size);
    vec->len++;

    return RESULT_OK();
}

/* Remove and return last element (LIFO). Capacity unchanged. */
Result vec_pop(Vec* restrict vec, void* out_item) {
    if (!vec || !vec->data || vec->magic != VEC_MAGIC_INIT) {
        LOG_ERROR(
            "vec_pop error: invalid argument (vec or its data is NULL) or not "
            "initialized!");
        return RESULT_ERR(ERR_INVALID);
    }

    if (!out_item) {
        LOG_ERROR("vec_pop error: out_item was NULL or uninitialized!");
        return RESULT_ERR(ERR_INVALID);
    }

    if (vec->len == 0) {
        LOG_WARN("vec_pop warning: tried to pop element of an empty vec!");
        return RESULT_ERR(ERR_INVALID);
    }

    const char* last_item = (const char*)vec->data + ((vec->len - 1) * vec->elem_size);

    memcpy(out_item, last_item, vec->elem_size);

    vec->len--;
    LOG_DEBUG(
        "vec_pop: Successfully popped last item of vec! New number of "
        "elements: %zu",
        vec->len);
    return RESULT_OK();
}

/* Reset length to 0 without deallocating (fast reuse) */
Result vec_clear(Vec* vec) {
    if (!vec || !vec->data || vec->magic != VEC_MAGIC_INIT) {
        LOG_ERROR("vec_clear: attempted access on uninitialized Vec");
        return RESULT_ERR(ERR_INVALID);
    }

    LOG_DEBUG("Successfully cleared vec with %zu elements", vec->len);
    vec->len = 0;

    return RESULT_OK();
}

Result vec_is_empty(const Vec* restrict vec, int* is_empty) {
    if (!vec || !vec->data || vec->magic != VEC_MAGIC_INIT) {
        LOG_ERROR("Failed to check vec for being empty or not. Passed invalid Vec!");
        return RESULT_ERR(ERR_INVALID);
    }

    *is_empty = vec->len == 0 ? 1 : 0;
    return RESULT_OK();
}

/* Remove element at index, shifting remaining elements left. O(n) operation. */
Result vec_remove(Vec* restrict vec, const size_t index, void* removed) {
    // Checks whether the provided pointer to Vec is valid
    if (!vec || !vec->data || vec->magic != VEC_MAGIC_INIT) {
        LOG_ERROR("Failed to check vec for being empty or not. Passed invalid Vec!");
        return RESULT_ERR(ERR_INVALID);
    }

    // Checks whether Vec is empty, if yes, we return early without doing
    // anyhting
    if (vec->len == 0) {
        return RESULT_OK();
    }

    /* Checks whether provided item index which we wish to remove. Returns early
     * with error if it's larger than the available number of elements */
    if (vec->len < index) {
        LOG_ERROR("Invalid index %zu larger than number of elements: %zu", index, vec->len);
        return RESULT_ERR(ERR_INVALID);
    }

    /* Example: Remove index 2 from [10, 20, 30, 40, 50]
     * Result: [10, 20, 40, 50] - we shift 40,50 left to fill the gap */

    // Point to the element being removed
    // Cast to char* (1 byte) for correct pointer arithmetic
    char* old_data = (char*)vec->data + (vec->elem_size * index);

    // Copy out the removed element if caller wants it
    if (removed) memcpy(removed, old_data, vec->elem_size);

    // Point to the element after the one being removed
    const char* next = old_data + vec->elem_size;

    // How many elements come after the removed one?
    // Example: len = 5, index = 2 → 5 - 2 - 1 = 2 elements (indices 3, 4)
    size_t num_elems_to_move = vec->len - index - 1;

    // Shift remaining elements left to fill the gap
    // Use memmove (not memcpy) because src and dest overlap
    if (num_elems_to_move > 0) {
        memmove(old_data, next, num_elems_to_move * vec->elem_size);
    }

    vec->len--;
    return RESULT_OK();
}

Result vec_insert(Vec* restrict vec, const size_t index, const void* item) {
    // Checks whether the provided pointer to Vec is valid
    if (!vec || !vec->data || vec->magic != VEC_MAGIC_INIT) {
        LOG_ERROR("Failed to check vec for being empty or not. Passed invalid Vec!");
        return RESULT_ERR(ERR_INVALID);
    }

    if (!item) {
        LOG_ERROR("You tried to insert a NULL!");
        return RESULT_ERR(ERR_INVALID);
    }

    if (index > vec->len) {
        LOG_ERROR("Index out of bounds, tried inserting at index %zu when maximum is %zu", index,
                  vec->len - 1);
        return RESULT_ERR(ERR_INVALID);
    }

    char* base = (char*)vec->data;
    char* insertion_address = base + (index * vec->elem_size);
    char* end_address = base + (vec->len * vec->elem_size);

    memmove(insertion_address + vec->elem_size, insertion_address,
            (size_t)end_address - (size_t)insertion_address);

    memcpy(insertion_address, item, vec->elem_size);
    vec->len++;

    LOG_INFO("Successfully inserted an item");
    return RESULT_OK();
}

/* Get pointer to element at index. Returns NULL if invalid.
 * WARNING: Pointer becomes invalid after vec_free() or reallocation. */
const void* vec_get(const Vec* restrict vec, size_t index) {
    if (!vec || !vec->data || vec->magic != VEC_MAGIC_INIT) {
        LOG_ERROR("vec_get: attempted access on uninitialized Vec");
        return NULL;
    } else if (index >= vec->len) {
        LOG_ERROR("vec_get: tried to access invalid index %zu (len=%zu)", index, vec->len);
        return NULL;
    }

    LOG_DEBUG("vec_get: returning element at index %zu", index);
    return (const char*)vec->data + index * vec->elem_size;
}

/* Free vector's heap allocation. Sets data to NULL (safe to call multiple
 * times). NOTE: Does not free the Vec struct itself (caller owns it). */
void vec_free(Vec* restrict vec) {
    if (!vec || !vec->data || vec->elem_size == 0 || vec->magic != VEC_MAGIC_INIT) {
        LOG_WARN(
            "vec_free error: tried to free Vec that had no data or tried to "
            "double-free it");
        return;
    }

    LOG_DEBUG("[VFREE] data=%p elem_size=%zu num_elem=%zu magic=0x%x\n", vec->data, vec->elem_size,
              vec->len, vec->magic);

    LOG_DEBUG("vec_free: freeing Vec with %zu elements (capacity=%zu, allocated=%s)", vec->len,
              vec->capacity, calculate_memory_footprint(vec->capacity * vec->elem_size));

    free(vec->data);
    vec->data = NULL;
    vec->len = 0;
    vec->capacity = 0;
    vec->elem_size = 0;
    vec->magic = VEC_MAGIC_FREED;

    LOG_INFO("vec_free: Vec successfully freed");
}

/* Format byte count as human-readable string (GB/MB/KB/bytes).
 * WARNING: Returns static buffer - not thread-safe, use immediately. */
const char* calculate_memory_footprint(const size_t allocation) {
    static char buf[64];

    if (allocation == 0) {
        snprintf(buf, sizeof(buf), "0 bytes");
    } else if (allocation >= 1024 * 1024 * 1024) {
        snprintf(buf, sizeof(buf), "%.0f GB", (double)allocation / (1024.0 * 1024 * 1024));
    } else if (allocation >= 1024 * 1024) {
        snprintf(buf, sizeof(buf), "%.0f MB", (double)allocation / (1024.0 * 1024));
    } else if (allocation >= 1024) {
        snprintf(buf, sizeof(buf), "%.0f KB", (double)allocation / 1024.0);
    } else {
        snprintf(buf, sizeof(buf), "%zu bytes", allocation);
    }

    return buf;
}