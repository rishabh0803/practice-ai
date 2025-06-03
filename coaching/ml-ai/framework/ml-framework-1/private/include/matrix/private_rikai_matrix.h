#ifndef __PRIVATE_RIKAI__MATRIX__H__
#define __PRIVATE_RIKAI__MATRIX__H__

#include <stdio.h>
#include <stdint.h>
#include <rikai_error.h>

typedef struct __rikai_matrix rikai_matrix;

double *rikai_matrix_get_data(const rikai_matrix *matrix, rikai_err *error);

#endif