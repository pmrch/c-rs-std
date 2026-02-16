/*
 * test_vec.c - Comprehensive test suite for Vec implementation
 * 
 * Tests cover:
 * - Basic functionality (new, push, pop, get, free)
 * - Edge cases (empty, full, out-of-bounds)
 * - Magic sentinel validation (uninitialized, freed, double-free)
 * - Stress testing (large numbers of elements, growth)
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "vec.h"
#include "result.h"

// Test counter
static int tests_run = 0;
static int tests_passed = 0;

// Color codes for pretty output (Windows Terminal supports these)
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_RESET "\033[0m"
#define COLOR_BLUE "\033[0;34m"

// Forward declare test function, then define it
// This avoids macro expansion issues
#define TEST(name) \
    static void test_##name(void); \
    void run_test_##name(void) { \
        printf(COLOR_BLUE "[ RUN      ] " COLOR_RESET "test_%s\n", #name); \
        tests_run++; \
        test_##name(); \
        tests_passed++; \
        printf(COLOR_GREEN "[       OK ] " COLOR_RESET "test_%s\n\n", #name); \
    } \
    static void test_##name(void)

// Macro to call the test
#define RUN_TEST(name) run_test_##name()

// Assert helpers with better messages
#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            printf(COLOR_RED "[  FAILED  ] " COLOR_RESET "Assertion failed: %s\n", #expr); \
            printf("           at %s:%d\n", __FILE__, __LINE__); \
            exit(1); \
        } \
    } while(0)

#define ASSERT_FALSE(expr) ASSERT_TRUE(!(expr))
#define ASSERT_EQ(a, b) ASSERT_TRUE((a) == (b))
#define ASSERT_NE(a, b) ASSERT_TRUE((a) != (b))
#define ASSERT_NULL(ptr) ASSERT_TRUE((ptr) == NULL)
#define ASSERT_NOT_NULL(ptr) ASSERT_TRUE((ptr) != NULL)

// Helper to check Result success (adapt to your Result type)
// Assuming your Result has either: .is_ok field or you use RESULT_IS_OK() macro
static inline int result_is_ok(Result r) {
    // If your Result type has .is_ok field:
    // return r.is_ok;
    
    // If you use RESULT_IS_OK() macro:
    return r.ok == 0;
    
    // Or if Result has .error_code and 0 means OK:
    // return r.error_code == 0;
}

// ============================================================================
// BASIC FUNCTIONALITY TESTS
// ============================================================================

TEST(vec_new_creates_empty_vec) {
    Vec v = {0};
    Result r = vec_new(&v, TYPE_I32);
    
    ASSERT_TRUE(result_is_ok(r));
    ASSERT_EQ(v.len, 0);
    ASSERT_EQ(v.capacity, 8);
    ASSERT_EQ(v.elem_size, 4);
    ASSERT_EQ(v.magic, VEC_MAGIC_INIT);
    ASSERT_NOT_NULL(v.data);
    
    vec_free(&v);
}

TEST(vec_new_handles_different_types) {
    Vec v8, v16, v32, v64;
    
    // 1-byte types
    vec_new(&v8, TYPE_U8);
    ASSERT_EQ(v8.elem_size, 1);
    vec_free(&v8);
    
    // 2-byte types
    vec_new(&v16, TYPE_I16);
    ASSERT_EQ(v16.elem_size, 2);
    vec_free(&v16);
    
    // 4-byte types
    vec_new(&v32, TYPE_F32);
    ASSERT_EQ(v32.elem_size, 4);
    vec_free(&v32);
    
    // 8-byte types
    vec_new(&v64, TYPE_F64);
    ASSERT_EQ(v64.elem_size, 8);
    vec_free(&v64);
}

TEST(vec_push_adds_single_element) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    int value = 42;
    Result r = vec_push(&v, &value);
    
    ASSERT_TRUE(result_is_ok(r));
    ASSERT_EQ(v.len, 1);
    
    const int *retrieved = (const int*)vec_get(&v, 0);
    ASSERT_NOT_NULL(retrieved);
    ASSERT_EQ(*retrieved, 42);
    
    vec_free(&v);
}

TEST(vec_push_adds_multiple_elements) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    for (int i = 0; i < 5; i++) {
        Result r = vec_push(&v, &i);
        ASSERT_TRUE(result_is_ok(r));
    }
    
    ASSERT_EQ(v.len, 5);
    
    for (int i = 0; i < 5; i++) {
        const int *val = (const int*)vec_get(&v, i);
        ASSERT_EQ(*val, i);
    }
    
    vec_free(&v);
}

TEST(vec_pop_removes_last_element) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        vec_push(&v, &values[i]);
    }
    
    int popped;
    Result r = vec_pop(&v, &popped);
    
    ASSERT_TRUE(result_is_ok(r));
    ASSERT_EQ(popped, 30);
    ASSERT_EQ(v.len, 2);
    
    vec_free(&v);
}

TEST(vec_get_retrieves_elements) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    int values[] = {100, 200, 300};
    for (int i = 0; i < 3; i++) {
        vec_push(&v, &values[i]);
    }
    
    const int *first = (const int*)vec_get(&v, 0);
    const int *middle = (const int*)vec_get(&v, 1);
    const int *last = (const int*)vec_get(&v, 2);
    
    ASSERT_EQ(*first, 100);
    ASSERT_EQ(*middle, 200);
    ASSERT_EQ(*last, 300);
    
    vec_free(&v);
}

TEST(vec_clear_empties_vec) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    for (int i = 0; i < 5; i++) {
        vec_push(&v, &i);
    }
    
    size_t old_capacity = v.capacity;
    Result r = vec_clear(&v);
    
    ASSERT_TRUE(result_is_ok(r));
    ASSERT_EQ(v.len, 0);
    ASSERT_EQ(v.capacity, old_capacity);  // Capacity unchanged
    
    vec_free(&v);
}

TEST(vec_remove_shifts_elements) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        vec_push(&v, &values[i]);
    }
    
    int removed;
    Result r = vec_remove(&v, 2, &removed);  // Remove 30
    
    ASSERT_TRUE(result_is_ok(r));
    ASSERT_EQ(removed, 30);
    ASSERT_EQ(v.len, 4);
    
    // Check remaining elements
    const int *elem0 = (const int*)vec_get(&v, 0);
    const int *elem1 = (const int*)vec_get(&v, 1);
    const int *elem2 = (const int*)vec_get(&v, 2);
    const int *elem3 = (const int*)vec_get(&v, 3);
    
    ASSERT_EQ(*elem0, 10);
    ASSERT_EQ(*elem1, 20);
    ASSERT_EQ(*elem2, 40);  // Shifted left
    ASSERT_EQ(*elem3, 50);  // Shifted left
    
    vec_free(&v);
}

// ============================================================================
// EDGE CASE TESTS
// ============================================================================

TEST(vec_push_triggers_realloc) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    ASSERT_EQ(v.capacity, 8);
    
    // Push 9 elements - should trigger growth
    for (int i = 0; i < 9; i++) {
        vec_push(&v, &i);
    }
    
    ASSERT_EQ(v.len, 9);
    ASSERT_EQ(v.capacity, 12);  // 8 + (8/2) = 12
    
    // Verify all elements are intact
    for (int i = 0; i < 9; i++) {
        const int *val = (const int*)vec_get(&v, i);
        ASSERT_EQ(*val, i);
    }
    
    vec_free(&v);
}

TEST(vec_push_multiple_reallocations) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    // Push enough to trigger multiple growths
    for (int i = 0; i < 25; i++) {
        vec_push(&v, &i);
    }
    
    ASSERT_EQ(v.len, 25);
    // 8 -> 12 -> 18 -> 27, so capacity should be 27
    ASSERT_EQ(v.capacity, 27);
    
    vec_free(&v);
}

TEST(vec_pop_on_empty_vec_fails) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    int out;
    Result r = vec_pop(&v, &out);
    
    ASSERT_FALSE(result_is_ok(r));
    
    vec_free(&v);
}

TEST(vec_pop_until_empty) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        vec_push(&v, &values[i]);
    }
    
    // Pop all elements
    for (int i = 0; i < 3; i++) {
        int out;
        Result r = vec_pop(&v, &out);
        ASSERT_TRUE(result_is_ok(r));
    }
    
    ASSERT_EQ(v.len, 0);
    
    // Try to pop from empty
    int out;
    Result r = vec_pop(&v, &out);
    ASSERT_FALSE(result_is_ok(r));
    
    vec_free(&v);
}

TEST(vec_get_out_of_bounds_returns_null) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    int value = 42;
    vec_push(&v, &value);
    
    const void *ptr = vec_get(&v, 1);   // Out of bounds
    ASSERT_NULL(ptr);
    
    ptr = vec_get(&v, 100);  // Way out of bounds
    ASSERT_NULL(ptr);
    
    vec_free(&v);
}

TEST(vec_get_on_empty_vec_returns_null) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    const void *ptr = vec_get(&v, 0);
    ASSERT_NULL(ptr);
    
    vec_free(&v);
}

TEST(vec_remove_first_element) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        vec_push(&v, &values[i]);
    }
    
    int removed;
    vec_remove(&v, 0, &removed);
    
    ASSERT_EQ(removed, 10);
    ASSERT_EQ(v.len, 2);
    
    const int *first = (const int*)vec_get(&v, 0);
    ASSERT_EQ(*first, 20);
    
    vec_free(&v);
}

TEST(vec_remove_last_element) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        vec_push(&v, &values[i]);
    }
    
    int removed;
    vec_remove(&v, 2, &removed);
    
    ASSERT_EQ(removed, 30);
    ASSERT_EQ(v.len, 2);
    
    vec_free(&v);
}

TEST(vec_remove_out_of_bounds_fails) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    int value = 42;
    vec_push(&v, &value);
    
    int removed;
    Result r = vec_remove(&v, 5, &removed);
    
    ASSERT_FALSE(result_is_ok(r));
    
    vec_free(&v);
}

// ============================================================================
// MAGIC SENTINEL TESTS
// ============================================================================

TEST(vec_push_on_uninitialized_vec_fails) {
    Vec v = {0};  // Not initialized with vec_new!
    int value = 42;
    
    Result r = vec_push(&v, &value);
    ASSERT_FALSE(result_is_ok(r));
}

TEST(vec_push_after_free_fails) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    vec_free(&v);
    
    int value = 42;
    Result r = vec_push(&v, &value);
    ASSERT_FALSE(result_is_ok(r));
}

TEST(vec_pop_on_freed_vec_fails) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    int value = 42;
    vec_push(&v, &value);
    vec_free(&v);
    
    int out;
    Result r = vec_pop(&v, &out);
    ASSERT_FALSE(result_is_ok(r));
}

TEST(vec_get_on_freed_vec_returns_null) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    int value = 42;
    vec_push(&v, &value);
    vec_free(&v);
    
    const void *ptr = vec_get(&v, 0);
    ASSERT_NULL(ptr);
}

TEST(vec_double_free_is_safe) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    vec_free(&v);
    vec_free(&v);  // Should not crash
    vec_free(&v);  // Even triple free
    
    // If we get here, test passed
}

TEST(vec_free_on_uninitialized_is_safe) {
    Vec v = {0};  // Not initialized
    vec_free(&v);  // Should not crash
}

// ============================================================================
// STRESS TESTS
// ============================================================================

TEST(vec_handles_many_elements) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    const int COUNT = 10000;
    
    // Push many elements
    for (int i = 0; i < COUNT; i++) {
        Result r = vec_push(&v, &i);
        ASSERT_TRUE(result_is_ok(r));
    }
    
    ASSERT_EQ(v.len, (size_t)COUNT);
    
    // Verify all values are correct
    for (int i = 0; i < COUNT; i++) {
        const int *val = (const int*)vec_get(&v, i);
        ASSERT_NOT_NULL(val);
        ASSERT_EQ(*val, i);
    }
    
    vec_free(&v);
}

TEST(vec_push_pop_cycle) {
    Vec v = {0};
    vec_new(&v, TYPE_I32);
    
    // Push and pop in cycles
    for (int cycle = 0; cycle < 100; cycle++) {
        // Push 10 elements
        for (int i = 0; i < 10; i++) {
            int value = cycle * 10 + i;
            vec_push(&v, &value);
        }
        
        // Pop 5 elements
        for (int i = 0; i < 5; i++) {
            int out;
            vec_pop(&v, &out);
        }
    }
    
    // Should have 500 elements left (100 cycles * 5 net adds)
    ASSERT_EQ(v.len, 500);
    
    vec_free(&v);
}

TEST(vec_works_with_different_sizes) {
    // Test with largest type (8 bytes)
    Vec v_large;
    vec_new(&v_large, TYPE_I64);
    
    for (int64_t i = 0; i < 100; i++) {
        vec_push(&v_large, &i);
    }
    
    ASSERT_EQ(v_large.len, 100);
    vec_free(&v_large);
    
    // Test with smallest type (1 byte)
    Vec v_small;
    vec_new(&v_small, TYPE_U8);
    
    for (int i = 0; i < 100; i++) {
        uint8_t val = (uint8_t)i;
        vec_push(&v_small, &val);
    }
    
    ASSERT_EQ(v_small.len, 100);
    vec_free(&v_small);
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main(void) {
    printf("\n");
    printf("========================================\n");
    printf("      Vec Test Suite\n");
    printf("========================================\n\n");
    
    // Basic functionality
    RUN_TEST(vec_new_creates_empty_vec);
    RUN_TEST(vec_new_handles_different_types);
    RUN_TEST(vec_push_adds_single_element);
    RUN_TEST(vec_push_adds_multiple_elements);
    RUN_TEST(vec_pop_removes_last_element);
    RUN_TEST(vec_get_retrieves_elements);
    RUN_TEST(vec_clear_empties_vec);
    RUN_TEST(vec_remove_shifts_elements);
    
    // Edge cases
    RUN_TEST(vec_push_triggers_realloc);
    RUN_TEST(vec_push_multiple_reallocations);
    RUN_TEST(vec_pop_on_empty_vec_fails);
    RUN_TEST(vec_pop_until_empty);
    RUN_TEST(vec_get_out_of_bounds_returns_null);
    RUN_TEST(vec_get_on_empty_vec_returns_null);
    RUN_TEST(vec_remove_first_element);
    RUN_TEST(vec_remove_last_element);
    RUN_TEST(vec_remove_out_of_bounds_fails);
    
    // Magic sentinel validation
    RUN_TEST(vec_push_on_uninitialized_vec_fails);
    RUN_TEST(vec_push_after_free_fails);
    RUN_TEST(vec_pop_on_freed_vec_fails);
    RUN_TEST(vec_get_on_freed_vec_returns_null);
    RUN_TEST(vec_double_free_is_safe);
    RUN_TEST(vec_free_on_uninitialized_is_safe);
    
    // Stress tests
    RUN_TEST(vec_handles_many_elements);
    RUN_TEST(vec_push_pop_cycle);
    RUN_TEST(vec_works_with_different_sizes);
    
    printf("========================================\n");
    printf(COLOR_GREEN "  %d/%d tests passed\n" COLOR_RESET, tests_passed, tests_run);
    printf("========================================\n\n");
    
    return 0;
}