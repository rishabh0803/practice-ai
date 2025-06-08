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

    difference = rikai_substract_vector(left_vector, right_vector, error);
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

    product_matrix = rikai_multiply_matrix(left_matrix, right_matrix, error);
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