#ifndef IO_H
#define IO_H

#include <stdio.h>
#include "result.h"

typedef struct {
    FILE* handle;
    int closed;
} File;

long long get_filesize(const char *filename);
Result file_open(File* out, const char* path, const char* mode);
Result file_close(File *in);
Result read_to_string(const char *path, char** buffer);

#endif