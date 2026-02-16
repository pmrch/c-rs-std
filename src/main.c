#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>

#include "io.h"
#include "result.h"
#include "vec.h"

#include "logging.h"

<<<<<<< HEAD
double now_seconds(void);
double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}
=======
#if defined(_WIN32)
#include <windows.h>
double now_seconds(void) {
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)freq.QuadPart;
}
#else
#include <time.h>
double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}
#endif
>>>>>>> 452b3f7edaba3913dd820391b9396046c511c7ca

int main() {
    char *buffer = NULL;
    Result read = read_to_string("oi.txt", &buffer);
    if (read.err) {
        LOG_ERROR("Error: %s", result_description(read.err));
        return -1;
    }

    printf("%s\n", buffer);
    free(buffer);
    getchar();

    Vec nv;
    Result new_vec_res = vec_new(&nv, TYPE_I64);
    if (new_vec_res.err) {
        LOG_ERROR("Error: %s", result_description(new_vec_res.err));
        return -1;
    }

    printf("\nNew vec details:\n");
    printf("  - Size of type: %zd bytes\n", nv.elem_size);
    printf("  - Total capacity: %zd\n", nv.capacity);
    printf("  - Number of elements: %zd\n", nv.len);

    int64_t pushed = (int64_t)13e7;
    double start = now_seconds();
    for (int64_t i = 0; i < pushed; i++) {
        Result vec_pushed = vec_push(&nv, &(int64_t){2 * i});
        
        if (vec_pushed.err) {
            printf("Error: %s", result_description(vec_pushed.err));
            return -1;
        }
    }

    double elapsed = now_seconds() - start;
    printf("Pushed %zu elements in %.3f ms (%.2f M ops/sec)\n", 
<<<<<<< HEAD
        nv.len, elapsed * 1000.0, ((double)nv.len / elapsed) / 1e6);
=======
        nv.len, elapsed * 1000.0, (nv.len / elapsed) / 1e6);
>>>>>>> 452b3f7edaba3913dd820391b9396046c511c7ca
    printf("\nFilled vec details:\n");
    printf("  - Size of type: %zd bytes\n", nv.elem_size);
    printf("  - Total capacity: %zd\n", nv.capacity);
    printf("  - Number of elements: %zd\n", nv.len);

    const size_t index = 100;
    const void* got = vec_get(&nv, index);
    if (!got) {
        LOG_WARN("Failed to get %d-th element of Vec", 0);
        return -1;
    }

    printf("\nGot the following result at index %zu of nv: %zu\n", 
        index, *(const int64_t*)got
    );

    void *out_item = malloc(sizeof(int64_t));
    Result popped = vec_pop(&nv, out_item);
    if (popped.err) {
        LOG_ERROR("Failed to pop last item");
        return -1;
    }


    printf("Popped '%zu' from vec!\n", *(int64_t*)out_item);
    Result cleared = vec_clear(&nv);
    if (cleared.err) {
        LOG_ERROR("Failed to clear Vec nv");
        return -1;
    }

    printf("\nCurrent vec stats: \n");
    printf("  - Size of type: %zd bytes\n", nv.elem_size);
    printf("  - Total capacity: %zd\n", nv.capacity);
    printf("  - Number of elements: %zd\n\n", nv.len);
    
    int *is_empty = malloc(sizeof(int));
    Result checked_empty = vec_is_empty(&nv, is_empty);
    if (checked_empty.err) {
        free(is_empty);
    }

    if (!*is_empty) {
        void *out_iteem = malloc(sizeof(int64_t));
        Result poppedd = vec_pop(&nv, out_iteem);

        if (poppedd.err) free(out_item);
        else printf("popped: %zu\n", *(int64_t*)out_iteem);
    } else LOG_WARN("Tried to use vec_pop on an empty array!"); 

    if (vec_remove(&nv, 1, NULL).err) {
        
    }

    vec_free(&nv);
    return 0;
}