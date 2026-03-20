#include "string2.h"

#include <stdio.h>

#include "logging.h"
#include "result.h"
#include "vec.h"

Result string_new(String* restrict s) {
    if (!s) {
        LOG_ERROR("fmt, ...");
        return RESULT_ERR(ERR_INVALID);
    }

    return vec_new(&s->vec, TYPE_U8);
}