#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "result.h"
#include "vec.h"
#include "logging.h"
#include "mystdlib.h"

int main() {
    char *buffer = NULL;
    Result read = read_to_string("oi.txt", &buffer);
    if (read.err) {
        LOG_ERROR("Error: %s", result_description(read.err));
        return -1;
    }

    printf("%s\n", buffer);
    free(buffer);
    pause();

    Vec nv;
    Result new_vec_res = new_vec(&nv, TYPE_I64);
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

    printf("\nGot the following result at index %zu of nv: %lld\n", 
        index, *(const int64_t*)got
    );

    vec_free(&nv);
    return 0;
}
