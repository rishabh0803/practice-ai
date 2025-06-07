#ifndef __RIKAI__MATH__H__
#define __RIKAI__MATH__H__

#include <stdio.h>
#include <stdint.h>
#include <rikai_error.h>
#include <rikai_vector.h>
#include <rikai_matrix.h>

rikai_matrix *rikai_multiply_matrix(const rikai_matrix *left_matrix, const rikai_matrix *right_matrix, rikai_err *error);

rikai_vector *rikai_multiply_matrix_with_vector(const rikai_matrix *matrix, const rikai_vector *vector, rikai_err *error);

rikai_vector *rikai_substract_vector(const rikai_vector *left_vector, const rikai_vector *right_vector, rikai_err *error);

#endif
