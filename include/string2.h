#ifndef STRING_2_H
#define STRING_2_H

#include "result.h"
#include "vec.h"

typedef Vec Vec_u8;
typedef struct {
    Vec_u8 vec;
    uint32_t magic;
} String;

Result string_new(String* restrict s);
Result string_from(String* restrict s, const char* str);

#endif