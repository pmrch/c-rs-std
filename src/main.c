#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "io.h"
#include "result.h"
#include "vec.h"
#include "logging.h"

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

    for (int64_t i = 0; i < 10000; i++) {
        Result vec_pushed = vec_push(&nv, &(int64_t){2 * i});
        
        if (vec_pushed.err) {
            printf("Error: %s", result_description(vec_pushed.err));
            return -1;
        }
    }

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

    printf("Cleared vec\n");
    printf("Current vec stats: \n");
    printf("  - Size of type: %zd bytes\n", nv.elem_size);
    printf("  - Total capacity: %zd\n", nv.capacity);
    printf("  - Number of elements: %zd\n", nv.len);

    void *out_iteem = malloc(sizeof(int64_t));
    Result poppedd = vec_pop(&nv, out_item);
    if (poppedd.err) {
        LOG_ERROR("Failed to pop last item");
        return -1;
    }
    
    printf("popped: %zu", *(int64_t*)out_iteem);
    vec_free(&nv);
    return 0;
}