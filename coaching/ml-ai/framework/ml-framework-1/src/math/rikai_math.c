#include <rikai.h>
#include <stdlib.h>
#include <private_rikai.h>

rikai_matrix *rikai_multiply_matrix(const rikai_matrix *left_matrix, const rikai_matrix *right_matrix, rikai_err *error)
{
    uint32_t left_matrix_rows, left_matrix_columns;
    uint32_t right_matrix_rows, right_matrix_columns;
    uint32_t product_matrix_rows, product_matrix_columns;
    uint32_t r, c, j;
    double *product_matrix_data;
    double *left_matrix_data;
    double *right_matrix_data;

    rikai_matrix *product_matrix = NULL;
    char error_message[4096];
    double value = 0.0;

    if (error)
        rikai_clear_error(error);

    if (left_matrix == NULL)
    {
        rikai_set_error(error, "Null pointer Argument(1)", RIKAI_NULL_POINTER);
        return NULL;
    }

    if (right_matrix == NULL)
    {
        rikai_set_error(error, "Null pointer Argument(2)", RIKAI_NULL_POINTER);
        return NULL;
    }

    rikai_matrix_get_dimensions(left_matrix, &left_matrix_rows, &left_matrix_columns, error);
    rikai_matrix_get_dimensions(right_matrix, &right_matrix_rows, &right_matrix_columns, error);
    if (left_matrix_columns != right_matrix_rows)
    {
        sprintf(error_message, "Cannot multiply matrices with dimensions [%u, %u] x [%u, %u]",
                left_matrix_rows, left_matrix_columns, right_matrix_rows, right_matrix_columns);
        rikai_set_error(error, error_message, RIKAI_INVALID_DIMENSIONS_FOR_MATRIX_MULTIPLICATION);
        return NULL;
    }

    product_matrix_rows = left_matrix_rows;
    product_matrix_columns = right_matrix_columns;
    product_matrix = rikai_matrix_create_new(product_matrix_rows, product_matrix_columns, error);
    if (product_matrix == NULL)
        return NULL;

    left_matrix_data = rikai_matrix_get_data(left_matrix, error);
    right_matrix_data = rikai_matrix_get_data(right_matrix, error);
    product_matrix_data = rikai_matrix_get_data(product_matrix, error);

    for (r = 0; r < product_matrix_rows; r++)
    {
        for (c = 0; c < product_matrix_columns; c++)
        {
            value = 0.0;
            for (j = 0; j < left_matrix_columns; j++)
            {
                value += left_matrix_data[r * left_matrix_columns + j] * right_matrix_data[j * right_matrix_columns + c];
            }
            product_matrix_data[r * product_matrix_columns + c] = value;
        }
    }

    return product_matrix;
}

rikai_vector *rikai_multiply_matrix_with_vector(const rikai_matrix *matrix, const rikai_vector *vector, rikai_err *error)
{
    char error_message[4096];
    uint32_t matrix_rows, matrix_columns;
    uint32_t vector_size;
    rikai_vector *result_vector = NULL;

    if (error)
        rikai_clear_error(error);

    if (matrix == NULL)
    {
        rikai_set_error(error, "Null pointer Argument(1)", RIKAI_NULL_POINTER);
        return NULL;
    }

    if (vector == NULL)
    {
        rikai_set_error(error, "Null pointer Argument(2)", RIKAI_NULL_POINTER);
        return NULL;
    }

    rikai_matrix_get_dimensions(matrix, &matrix_rows, &matrix_columns, error);
    vector_size = rikai_vector_get_size(vector, error);

    if (matrix_columns != vector_size)
    {
        sprintf(error_message, "Cannot multiply matrix [%u x %u] with vector of size %u", matrix_rows, matrix_columns, vector_size);
        rikai_set_error(error, error_message, RIKAI_INVALID_DIMENSIONS_FOR_MATRIX_VECTOR_MULTIPLICATION);
        return NULL;
    }

    // TODO:
    // Create a new vector to hold the result
    // call multiply_matrix with the matrix and vector->matrix
    // set the result matrix to result_vector -> matri

    return result_vector;
}