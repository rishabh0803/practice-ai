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
#define RIKAI_FILE_READ_ERROR 9
#define RIKAI_FILE_WRITE_ERROR 10
#define RIKAI_CSV_FORMAT_ERROR 11
#define RIKAI_CSV_EMPTY_ERROR 12
#define RIKAI_INVALID_SIZE 13
#define RIKAI_INVALID_INDEX 14
#define RIKAI_FILE_FORMAT_ERROR 15
#define RIKAI_FILE_CONTAINS_MATRIX 16
#define RIKAI_GET_VECTOR_ERROR 17
#define RIKAI_INVALID_DIMENSIONS_FOR_MATRIX_MULTIPLICATION 18
#define RIKAI_INVALID_DIMENSIONS_FOR_MATRIX_VECTOR_MULTIPLICATION 19
#define RIKAI_VECTOR_SIZE_MISMATCH 20
#define RIKAI_INVALID_MATRIX_CONTAINER_DIMENSIONS 21
#define RIKAI_INVALID_VECTOR_CONTAINER_SIZE 23

#define rikai_on_error_throw(error, error_handler) goto error_handler

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