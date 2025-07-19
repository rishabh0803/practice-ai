#include <rikai.h>
#include <private_rikai.h>
#include <stdio.h>
#include <math.h>

double rikai_mean_squared_error(const rikai_vector *left_vector, const rikai_vector *right_vector, rikai_err *error)
{
    char error_message[256];
    uint32_t left_size, right_size, i;
    rikai_matrix *matrix = NULL;
    double *data = NULL;

    rikai_vector *difference = NULL;
    rikai_matrix *left_matrix = NULL;
    rikai_matrix *right_matrix = NULL;
    rikai_matrix *product_matrix = NULL;

    double *left_data = NULL;
    double *right_data = NULL;
    double mean_squared_error = 0.0;

    if (error)
        rikai_clear_error(error);

    if (left_vector == NULL)
    {
        rikai_set_error(error, "Null pointer Argument (1)", RIKAI_NULL_POINTER);
        return 0.0;
    }

    if (right_vector == NULL)
    {
        rikai_set_error(error, "Null pointer Argument (2)", RIKAI_NULL_POINTER);
        return 0.0;
    }

    left_size = rikai_vector_get_size(left_vector, error);
    right_size = rikai_vector_get_size(right_vector, error);

    if (left_size != right_size)
    {
        sprintf(error_message, "Unable to calculate mean squared error: left vector size (%u) does not match right vector size (%u)",
                left_size, right_size);
        rikai_set_error(error, error_message, RIKAI_VECTOR_SIZE_MISMATCH);
        return 0.0;
    }

    difference = rikai_subtract_vector(left_vector, right_vector, NULL, error);
    if (difference == NULL)
    {
        mean_squared_error = 0.0;
        left_data = rikai_matrix_get_data(rikai_vector_get_matrix(left_vector, error), error);
        right_data = rikai_matrix_get_data(rikai_vector_get_matrix(right_vector, error), error);

        for (i = 0; i < left_size; ++i)
        {
            mean_squared_error += pow(left_data[i] - right_data[i], 2);
        }

        return mean_squared_error / left_size;
    }

    right_matrix = rikai_vector_get_matrix(difference, error);
    left_matrix = rikai_matrix_transpose_create_new(right_matrix, error);
    if (left_matrix == NULL)
    {
        goto fallback;
    }

    product_matrix = rikai_multiply_matrix(left_matrix, right_matrix, NULL, error);
    if (product_matrix == NULL)
    {
        goto fallback;
    }

    mean_squared_error = rikai_matrix_get(product_matrix, 0, 0, error) / left_size;
    rikai_vector_destroy(difference, error);
    rikai_matrix_destroy(product_matrix, error);
    rikai_matrix_destroy(left_matrix, error);
    return mean_squared_error;

fallback:
    matrix = rikai_vector_get_matrix(difference, error);
    data = rikai_matrix_get_data(matrix, error);
    mean_squared_error = 0.0;
    for (i = 0; i < left_size; ++i)
    {
        mean_squared_error += pow(data[i], 2);
    }
    rikai_vector_destroy(difference, error);
    rikai_matrix_destroy(product_matrix, error);
    rikai_matrix_destroy(left_matrix, error);
    return mean_squared_error / left_size;
}

double rikai_r2_score(const rikai_vector *y_true, const rikai_vector *y_pred, rikai_err *error)
{
    char error_message[256];
    uint32_t y_true_size, y_pred_size;
    rikai_vector *difference_vector = NULL;
    rikai_vector *difference_vector_T = NULL;

    double mean_y = 0.0;
    double ss_residual = 0.0;
    double ss_total = 0.0;

    if (error)
        rikai_clear_error(error);

    if (y_true == NULL)
    {
        rikai_set_error(error, "Null pointer Argument(1)", RIKAI_NULL_POINTER);
        return 0.0;
    }

    if (y_pred == NULL)
    {
        rikai_set_error(error, "Null pointer Argument(2)", RIKAI_NULL_POINTER);
        return 0.0;
    }

    y_true_size = rikai_vector_get_size(y_true, error);
    y_pred_size = rikai_vector_get_size(y_pred, error);

    if (y_true_size != y_pred_size)
    {
        sprintf(error_message, "Unable to calculate R^2 score: sSize mismatch between y_true (%u) and y_pred (%u)", y_true_size, y_pred_size);
        rikai_set_error(error, error_message, RIKAI_SIZE_MISMATCH);
        return 0.0;
    }

    difference_vector = rikai_subtract_vector(y_true, y_pred, NULL, error);
    if (rikai_has_error(error))
        return 0.0;

    difference_vector_T = rikai_vector_transpose_create_new(difference_vector, NULL, error);
    if (rikai_has_error(error))
        return 0.0;

    ss_residual = rikai_multiple_vector_get_scalar(difference_vector_T, difference_vector, error);

    mean_y = rikai_mean_vector(y_true, error);
    difference_vector = rikai_subtract_scalar_from_vector(y_true, mean_y, NULL, error);
    if (rikai_has_error(error))
        return 0.0;

    difference_vector_T = rikai_vector_transpose_create_new(difference_vector, NULL, error);
    if (rikai_has_error(error))
        return 0.0;

    ss_total = rikai_multiple_vector_get_scalar(difference_vector_T, difference_vector, error);
    if (rikai_has_error(error))
        return 0.0;

    rikai_vector_destroy(difference_vector, error);
    rikai_vector_destroy(difference_vector_T, error);

    return 1.0 - (ss_residual / ss_total);
}
