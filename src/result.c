#include "result.h"

const char* result_description(ErrorCode code) {
    switch (code) {
        case OK:
            return "Success";
        case ERR_IO:
            return "I/O Error: Could not read or open file";
        case ERR_EMPTY:
            return "Tried to perform an altering function on an empty variable";
        case ERR_MEMORY:
            return "Memory Error: Heap allocation failed";
        case ERR_INVALID:
            return "Invalid Argument: Check your pointers";
        case ERR_NOT_FOUND:
            return "A required resource was not found";
        default:
            return "Unknown error occurred";
    }
}