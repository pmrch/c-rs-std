#include "io.h"
#include "result.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char *buffer = NULL;
    Result read = read_to_string("oi.txt", &buffer);
    if (read.err) {
        printf("Error: %s", result_description(read.err));
    }

    printf("Read: %s\n", buffer);

    free(buffer);
    return 0;
}
