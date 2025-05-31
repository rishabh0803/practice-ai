#ifndef __RIKAI__MATRIX__H__
#define __RIKAI__MATRIX__H__
#include <stdint.h>
#include <rikai_error.h>
#include <rikai_vector.h>

typedef struct __rikai_matrix rikai_matrix;

rikai_matrix *rikai_matrix_create_new(uint32_t rows, uint32_t columns, rikai_err *error);

void __rikai_matrix_destroy(rikai_matrix **matrix, rikai_err *error);
#define rikai_matrix_destroy(matrix, error) __rikai_matrix_destroy(&matrix, error)

void rikai_matrix_set(rikai_matrix *matrix, uint32_t row, uint32_t column, double value, rikai_err *error);

double rikai_matrix_get(const rikai_matrix *matrix, uint32_t row, uint32_t column, rikai_err *error);

void rikai_matrix_get_dimensions(const rikai_matrix *matrix, uint32_t *rows, uint32_t *columns, rikai_err *error);

uint32_t rikai_matrix_get_row_count(const rikai_matrix *matrix, rikai_err *error);

uint32_t rikai_matrix_get_column_count(const rikai_matrix *matrix, rikai_err *error);

void rikai_matrix_write(const rikai_matrix *matrix, const char *filename, rikai_err *error);

rikai_matrix *rikai_matrix_read(const char *filename, rikai_err *error);

rikai_matrix *rikai_matrix_read_csv(const char *filename, int skip_lines, rikai_err *error);

void rikai_matrix_transpose(rikai_matrix *matrix, rikai_err *error);

rikai_matrix *rikai_matrix_transpose_create_new(const rikai_matrix *matrix, rikai_err *error);

void rikai_matrix_print(FILE *file, const rikai_matrix *matrix, rikai_err *error);

void rikai_matrix_fill(rikai_matrix *input_matrix,
                       uint32_t start_row_index, uint32_t start_column_index,
                       uint32_t end_row_index, uint32_t end_column_index,
                       double fill_value, rikai_err *error);

void rikai_matrix_copy(rikai_matrix *target_matrix,
                       uint32_t target_start_row_index, uint32_t target_start_column_index,
                       const rikai_matrix *source_matrix,
                       uint32_t source_start_row_index, uint32_t source_start_column_index,
                       uint32_t source_end_row_index, uint32_t source_end_column_index,
                       rikai_err *error);

rikai_vector *rikai_matrix_get_vector(const rikai_matrix *matrix,
                                      uint32_t start_row_index, uint32_t end_row_index,
                                      uint32_t start_column_index, uint32_t end_column_index,
                                      rikai_err *error);

#endif