#ifndef __PRIVATE_RIKAI__ERROR__H__
#define __PRIVATE_RIKAI__ERROR__H__

#include <stdio.h>
#include <stdint.h>

typedef struct __rikai_err rikai_err;

void rikai_set_error(rikai_err *err, const char *str, uint16_t fallback_error_code);

#endif