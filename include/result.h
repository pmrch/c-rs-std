#ifndef RESULT_H
#define RESULT_H

typedef enum {
    OK = 0,
    ERR_IO,
    ERR_MEMORY,
    ERR_INVALID,
    ERR_NOT_FOUND,
    ERR_EMPTY
} ErrorCode;

typedef struct {
    int ok;
    ErrorCode err;
} Result;

const char* result_description(ErrorCode code);

#define RESULT_OK() ((Result) { 0, OK })
#define RESULT_ERR(e) ((Result) { 1, e })
#endif