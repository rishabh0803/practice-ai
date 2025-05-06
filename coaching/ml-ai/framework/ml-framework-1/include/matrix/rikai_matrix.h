#ifndef __RIKAI_MATRIX__H__
#define __RIKAI_MATRIX__H__
#include <stdint.h>
#include <rikai_error.h>

typedef struct __rikai_matrix rikai_matrix;

rikai_matrix * rikai_matrix_create_new(uint32_t rows, uint32_t columns, rikai_err *error);

void __rikai_matrix_destory(rikai_matrix **matrix, rikai_err *error);
#define rikai_matrix_destory(matrix, error) __rikai_matrix_destory(&matrix, error)

void rikai_matrix_set(rikai_matrix *matrix, uint32_t row, uint32_t column, double value, rikai_err *error);

double rikai_matrix_get(const rikai_matrix *matrix, uint32_t row, uint32_t column, rikai_err *error);

void rikai_matrix_get_dimensions(const rikai_matrix *matrix, uint32_t *rows, uint32_t *columns, rikai_err *error);

uint32_t rikai_matrix_get_row_count(const rikai_matrix *matrix, rikai_err *error);

uint32_t rikai_matrix_get_column_count(const rikai_matrix *matrix, rikai_err *error);

#endif