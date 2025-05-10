#ifndef __RIKAI__ERROR__H__
#define __RIKAI__ERROR__H__

#include <stdio.h>
#include <stdint.h>

#define RIKAI_LOW_MEMORY 1
#define RIKAI_INVALID_DIMENSIONS 2
#define RIKAI_INVALID_ROW_COUNT 3
#define RIKAI_INVALID_COLUMN_COUNT 4
#define RIKAI_NULL_POINTER 5
#define RIKAI_INVALID_ROW_AND_COLUMN_INDEX 6
#define RIKAI_INVALID_ROW_INDEX 7
#define RIKAI_INVALID_COLUMN_INDEX 8

typedef struct __rikai_err rikai_err;

rikai_err *rikai_error_create_new();

void __rikai_error_destroy(rikai_err **err);
#define rikai_error_destroy(err) __rikai_error_destroy(&err)

uint8_t rikai_has_error(const rikai_err *err);

void rikai_get_error(char *str, uint32_t size, const rikai_err *err);

void rikai_clear_error(rikai_err *err);

uint32_t rikai_get_error_len(const rikai_err *err);

uint16_t rikai_get_error_code(const rikai_err *err);

#endif