#ifndef __RIKAI__LOGISTIC_REGRESSION__H__
#define __RIKAI__LOGISTIC_REGRESSION__H__

#include <rikai_error.h>
#include <rikai_vector.h>
#include <rikai_matrix.h>
#include <rikai_data_provider.h>

rikai_vector *rikai_logistic_regression_fit(
    const rikai_matrix *x_matrix,
    const rikai_vector *y_vector,
    uint64_t number_of_iterations,
    double learning_rate,
    uint8_t (*on_iteration_complete)(uint64_t, double, rikai_vector *),
    rikai_err *error);

#endif