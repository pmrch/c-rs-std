#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "io.h"
#include "logging.h"
#include "result.h"

int get_filesize(const char *filename) {
    struct stat st;

    if(stat(filename, &st) == 0) {
        LOG_DEBUG("get_filesize: %s = %lld bytes", filename, (long long)st.st_size);
        return (int)st.st_size;
    } else {
        LOG_WARN("get_filesize: failed to stat file %s", filename);
        return -1;
    }
}

Result file_open(File *out, const char *path, const char *mode) {
    LOG_INFO("Opening file '%s' with mode '%s'", path, mode);

    out->handle = fopen(path, mode);
    out->closed = 0;
    if (out->handle == NULL || get_filesize(path) < 0) {
        LOG_ERROR("file_open error: failed to open file '%s'", path);
        return RESULT_ERR(ERR_IO); 
    }

    LOG_DEBUG("file_open: successfully opened '%s'", path);
    return RESULT_OK();
}

Result file_close(File *in) {
    if (!in || !in->handle || in->closed) {
        LOG_ERROR("file_close error: passed argument file was uninitialized or already closed");
        return RESULT_ERR(ERR_IO);
    }

    fclose(in->handle);
    in->handle = NULL;
    in->closed = 1;

    LOG_DEBUG("file_close: file successfully closed");
    return RESULT_OK();
}

Result read_to_string(const char *path, char **out_buffer) {
    if (!out_buffer) {
        LOG_ERROR("read_to_string error: output buffer pointer was not initialized");
        return RESULT_ERR(ERR_INVALID);
    }

    *out_buffer = NULL;
    LOG_INFO("Reading entire file '%s' into string", path);

    File file;
    Result r = file_open(&file, path, "rb");
    if (r.err) {
        LOG_ERROR("read_to_string error: failed to open file '%s'", path);
        return r;
    }

    int size = get_filesize(path);
    if (size < 0) {
        LOG_ERROR("read_to_string error: file size was negative for '%s'", path);
        file_close(&file);
        return RESULT_ERR(ERR_IO);
    }

    LOG_DEBUG("read_to_string: allocating buffer of size %d", size + 1);
    char *buffer = malloc((size_t)size + 1);
    if (!buffer) {
        LOG_ERROR("read_to_string error: failed to allocate buffer for file '%s'", path);
        file_close(&file);
        return RESULT_ERR(ERR_MEMORY);
    }

    size_t read = fread(buffer, 1, (size_t)size, file.handle);
    if (read != (size_t)size) {
        if (feof(file.handle))
            LOG_ERROR("read_to_string: unexpected EOF while reading %d bytes from '%s'", size, path);
        else if (ferror(file.handle))
            LOG_ERROR("read_to_string: IO error while reading %d bytes from '%s'", size, path);
        else
            LOG_ERROR("read_to_string: unknown read error (%zu/%d bytes read) from '%s'", read, size, path);

        free(buffer);
        file_close(&file);
        return RESULT_ERR(ERR_IO);
    }

    buffer[size] = '\0';
    *out_buffer = buffer;

    LOG_INFO("read_to_string: successfully read %d bytes from '%s'", size, path);
    file_close(&file);
    return RESULT_OK();
}
