#ifndef __RIKAI__ERROR__H__
#define __RIKAI__ERROR__H__

#include <stdio.h>

typedef struct __rikai_err rikai_err;

rikai_err *rikai_error_create_new();
void __rikai_error_destroy(rikai_err **err);

#define rikai_error_destroy(err) __rikai_error_destroy(&err)

unsigned char rikai_has_error(rikai_err *err);
void rikai_get_error(char *str, unsigned int size, rikai_err *err);
void rikai_clear_error(rikai_err *err);

#endif