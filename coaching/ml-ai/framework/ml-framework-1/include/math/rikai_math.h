#ifndef __RIKAI__MATH__H__
#define __RIKAI__MATH__H__

#include <stdio.h>
#include <stdint.h>
#include <rikai_error.h>
#include <rikai_vector.h>
#include <rikai_matrix.h>

rikai_matrix *rikai_multiply_matrix(const rikai_matrix *left_matrix, const rikai_matrix *right_matrix, rikai_matrix *product_matrix_container, rikai_err *error);

rikai_vector *rikai_multiply_matrix_with_vector(const rikai_matrix *matrix, const rikai_vector *vector, rikai_vector *product_vector_container, rikai_err *error);

rikai_vector *rikai_multiply_vector_with_scalar(const rikai_vector *vector, double scalar, rikai_vector *product_vector_container, rikai_err *error);

double rikai_multiple_vector_get_scalar(const rikai_vector *left_vector, const rikai_vector *right_vector, rikai_err *error);

rikai_vector *rikai_subtract_vector(const rikai_vector *left_vector, const rikai_vector *right_vector, rikai_vector *difference_vector_container, rikai_err *error);

rikai_vector *rikai_subtract_scalar_from_vector(const rikai_vector *vector, double value, rikai_vector *difference_vector_container, rikai_err *error);

double rikai_sum_vector(const rikai_vector *vector, rikai_err *error);

double rikai_mean_vector(const rikai_vector *vector, rikai_err *error);

rikai_vector *rikai_sigmoid_vector(const rikai_vector *vector, rikai_vector *sigmoid_vector_container, rikai_err *error);

rikai_vector *rikai_log_vector(const rikai_vector *vector, rikai_vector *log_vector_container, rikai_err *error);

rikai_vector *rikai_hadamard_product_vector(const rikai_vector *left_vector, const rikai_vector *right_vector, rikai_vector *product_vector_container, rikai_err *error);

rikai_vector *rikai_subtract_vector_from_scalar(double value, const rikai_vector *vector, rikai_vector *difference_vector_container, rikai_err *error);

rikai_vector *rikai_sum_vectors(const rikai_vector *left_vector, const rikai_vector *right_vector, rikai_vector *sum_vector_container, rikai_err *error);

#endif
