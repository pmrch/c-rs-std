#include "io.h"
#include "result.h"
#include "vec.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char *buffer = NULL;
    Result read = read_to_string("oi.txt", &buffer);
    if (read.err) {
        printf("Error: %s", result_description(read.err));
        return -1;
    }

    printf("Read: %s\n", buffer);
    free(buffer);

    Vec nv;
    Result new_vec_res = new_vec(&nv, TYPE_CHAR);
    if (new_vec_res.err) {
        printf("Error: %s", result_description(new_vec_res.err));
        return -1;
    }

    printf("\nNew vec details:\n");
    printf("  - Size of type: %ld bytes\n", nv.type_size);
    printf("  - Total capacity: %ld bytes\n", nv.total_capacity);
    printf("  - Number of elements: %ld\n", nv.len);
    
    Result vec_pushed = vec_push(&nv, &(int16_t){2});
    if (vec_pushed.err) {
        printf("Error: %s", result_description(vec_pushed.err));
        return -1;
    }

    printf("\nModified vec details:\n");
    printf("  - Size of type: %ld bytes\n", nv.type_size);
    printf("  - Total capacity: %ld bytes\n", nv.total_capacity);
    printf("  - Number of elements: %ld\n", nv.len);

    return 0;
}
