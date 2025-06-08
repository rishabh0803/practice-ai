#ifndef __RIKAI__ML__MATH__H__
#define __RIKAI__ML__MATH__H__

#include <stdio.h>
#include <stdint.h>
#include <rikai_error.h>
#include <rikai_vector.h>
#include <rikai_matrix.h>
#include <rikai_math.h>

double rikai_mean_squared_error(const rikai_vector *left_vector, const rikai_vector *right_vector, rikai_err *error);

#endif
