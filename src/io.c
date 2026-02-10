#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "io.h"
#include "result.h"

long long get_filesize(const char *filename) {
    struct stat st;

    if(stat(filename, &st) == 0) {
        return (long long)st.st_size;
    } else {
        return -1;
    }
}

Result file_open(File *out, const char *path, const char *mode) {
    out->handle = fopen(path, mode);
    if (out->handle == NULL || get_filesize(path) < 0) {
        return RESULT_ERR(ERR_IO); 
    }

    return RESULT_OK();
}

Result file_close(File *in) {
    if (!in || !in->handle) {
        return RESULT_ERR(ERR_IO);
    }

    fclose(in->handle);
    in->handle = NULL;
    in->closed = 1;

    return RESULT_OK();
}

Result read_to_string(const char *path, char **out_buffer) {
    if (!out_buffer) return RESULT_ERR(ERR_INVALID);
    *out_buffer = NULL;

    File file;
    Result r = file_open(&file, path, "rb");
    if (r.err) return r;

    long long size = get_filesize(path);
    if (size < 0) {
        file_close(&file);
        return RESULT_ERR(ERR_IO);
    }

    char *buffer = malloc(size + 1);
    if (!buffer) {
        file_close(&file);
        return RESULT_ERR(ERR_MEMORY);
    }

    size_t read = fread(buffer, 1, size, file.handle);
    if (read != (size_t)size) {
        free(buffer);
        file_close(&file);
        return RESULT_ERR(ERR_IO);
    }

    buffer[size] = '\0';
    *out_buffer = buffer;

    file_close(&file);
    return RESULT_OK();
}
