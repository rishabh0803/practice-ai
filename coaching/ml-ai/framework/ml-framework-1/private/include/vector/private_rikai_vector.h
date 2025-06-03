#ifndef __PRIVATE_RIKAI__VECTOR__H__
#define __PRIVATE_RIKAI__VECTOR__H__

#include <stdio.h>
#include <stdint.h>
#include <rikai_error.h>

typedef struct __rikai_vector rikai_vector;
typedef struct __rikai_matrix rikai_matrix;

rikai_matrix *rikai_vector_get_matrix(const rikai_vector *vector, rikai_err *error);

void rikai_vector_set_matrix(rikai_vector *vector, rikai_matrix *matrix, rikai_err *error);

#endif