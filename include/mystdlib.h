#ifndef MY_STD_LIB_H
#define MY_STD_LIB_H

#include <stdio.h>
#include <stdlib.h>

void pause() {
    char* suspense = malloc(2);
    if (!suspense) { return; }

    fgets(suspense, 2, stdin);
    free(suspense);
}

#endif