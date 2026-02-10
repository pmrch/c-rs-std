#include "result.h"

const char* result_description(ErrorCode code) {
    switch (code) {
        case OK:          return "Success";
        case ERR_IO:      return "I/O Error: Could not read or open file";
        case ERR_MEMORY:  return "Memory Error: Heap allocation failed";
        case ERR_INVALID: return "Invalid Argument: Check your pointers";
        default:          return "Unknown error occurred";
    }
}