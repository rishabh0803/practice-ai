#include <rikai.h>
#include <stdlib.h>
#include <private_rikai.h>

rikai_matrix *rikai_multiply_matrix(const rikai_matrix *left_matrix, const rikai_matrix *right_matrix, rikai_matrix *product_matrix_container, rikai_err *error)
{
    uint32_t left_matrix_rows, left_matrix_columns;
    uint32_t right_matrix_rows, right_matrix_columns;
    uint32_t product_matrix_rows, product_matrix_columns;
    uint32_t product_matrix_container_rows, product_matrix_container_columns;
    uint32_t r, c, j;
    double *product_matrix_data = NULL;
    double *left_matrix_data = NULL;
    double *right_matrix_data = NULL;

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

    if (product_matrix_container == NULL)
    {
        product_matrix = rikai_matrix_create_new(product_matrix_rows, product_matrix_columns, error);
        if (product_matrix == NULL)
            return NULL;
    }
    else
    {
        product_matrix_container_rows = rikai_matrix_get_row_count(product_matrix_container, error);
        product_matrix_container_columns = rikai_matrix_get_column_count(product_matrix_container, error);

        if (product_matrix_container_rows != product_matrix_rows || product_matrix_container_columns != product_matrix_columns)
        {
            sprintf(error_message, "Provided matrix container has dimensions [%u, %u] but expected [%u, %u]",
                    product_matrix_container_rows, product_matrix_container_columns, product_matrix_rows, product_matrix_columns);
            rikai_set_error(error, error_message, RIKAI_INVALID_MATRIX_CONTAINER_DIMENSIONS);
            return NULL;
        }

        product_matrix = product_matrix_container;
    }

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

rikai_vector *rikai_multiply_matrix_with_vector(const rikai_matrix *matrix, const rikai_vector *vector, rikai_vector *product_vector_container, rikai_err *error)
{
    char error_message[4096];
    uint32_t matrix_rows, matrix_columns;
    uint32_t vector_size;
    uint32_t product_vector_container_size;
    rikai_vector *result_vector = NULL;
    rikai_matrix *product_matrix = NULL;

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

    if (product_vector_container == NULL)
    {
        result_vector = rikai_vector_create_new(1, error);
        if (result_vector == NULL)
            return NULL;
        product_matrix = NULL;
    }
    else
    {
        product_vector_container_size = rikai_vector_get_size(product_vector_container, error);

        if (product_vector_container_size != matrix_rows)
        {
            sprintf(error_message, "Provided vector container has size %u but expected %u", product_vector_container_size, matrix_rows);
            rikai_set_error(error, error_message, RIKAI_INVALID_VECTOR_CONTAINER_SIZE);
            return NULL;
        }

        product_matrix = rikai_vector_get_matrix(product_vector_container, error);
        result_vector = product_vector_container;
    }

    product_matrix = rikai_multiply_matrix(matrix, rikai_vector_get_matrix(vector, error), product_matrix, error);
    if (product_matrix == NULL)
    {
        rikai_vector_destroy(result_vector, error);
        return NULL;
    }

    rikai_vector_set_matrix(result_vector, product_matrix, error);
    return result_vector;
}

rikai_vector *rikai_subtract_vector(const rikai_vector *left_vector, const rikai_vector *right_vector, rikai_vector *difference_vector_container, rikai_err *error)
{
    char error_message[4096];
    uint32_t left_vector_size, right_vector_size;
    uint32_t difference_vector_container_size;
    uint32_t i;
    double *left_data = NULL, *right_data = NULL, *result_data = NULL;
    rikai_vector *result_vector = NULL;

    if (error)
        rikai_clear_error(error);

    if (left_vector == NULL)
    {
        rikai_set_error(error, "Null pointer Argument(1)", RIKAI_NULL_POINTER);
        return NULL;
    }

    if (right_vector == NULL)
    {
        rikai_set_error(error, "Null pointer Argument(2)", RIKAI_NULL_POINTER);
        return NULL;
    }

    left_vector_size = rikai_vector_get_size(left_vector, error);
    right_vector_size = rikai_vector_get_size(right_vector, error);

    if (left_vector_size != right_vector_size)
    {
        sprintf(error_message, "Cannot subtract vectors of size %u and %u", left_vector_size, right_vector_size);
        rikai_set_error(error, error_message, RIKAI_VECTOR_SIZE_MISMATCH);
        return NULL;
    }

    if (difference_vector_container == NULL)
    {
        result_vector = rikai_vector_create_new(left_vector_size, error);
        if (result_vector == NULL)
            return NULL;
    }
    else
    {
        difference_vector_container_size = rikai_vector_get_size(difference_vector_container, error);
        if (difference_vector_container_size != left_vector_size)
        {
            sprintf(error_message, "Provided vector container has size %u but expected %u", difference_vector_container_size, left_vector_size);
            rikai_set_error(error, error_message, RIKAI_INVALID_VECTOR_CONTAINER_SIZE);
            return NULL;
        }
        result_vector = difference_vector_container;
    }

    left_data = rikai_matrix_get_data(rikai_vector_get_matrix(left_vector, error), error);
    right_data = rikai_matrix_get_data(rikai_vector_get_matrix(right_vector, error), error);

    result_data = rikai_matrix_get_data(rikai_vector_get_matrix(result_vector, error), error);

    for (i = 0; i < left_vector_size; i++)
    {
        // [i*1+0] = [i]
        result_data[i] = left_data[i] - right_data[i];
    }

    return result_vector;
}

rikai_vector *rikai_substract_vector_with_scalar(const rikai_vector *vector, double scalar, rikai_vector *difference_vector_container, rikai_err *error)
{
    char error_message[4096];
    uint32_t vector_size;
    uint32_t difference_vector_container_size;
    uint32_t i;
    double value;
    rikai_vector *result_vector = NULL;

    if (error)
        rikai_clear_error(error);

    if (vector == NULL)
    {
        rikai_set_error(error, "Null pointer Argument(1)", RIKAI_NULL_POINTER);
        return NULL;
    }

    vector_size = rikai_vector_get_size(vector, error);

    if (difference_vector_container == NULL)
    {
        result_vector = rikai_vector_create_new(vector_size, error);
        if (result_vector == NULL)
            return NULL;
    }
    else
    {
        difference_vector_container_size = rikai_vector_get_size(difference_vector_container, error);
        if (difference_vector_container_size != vector_size)
        {
            sprintf(error_message, "Provided vector container has size %u but expected %u", difference_vector_container_size, vector_size);
            rikai_set_error(error, error_message, RIKAI_INVALID_VECTOR_CONTAINER_SIZE);
            return NULL;
        }
        result_vector = difference_vector_container;
    }

    for (i = 0; i < vector_size; i++)
    {
        value = rikai_vector_get(vector, i, error);
        value -= scalar;
        rikai_vector_set(result_vector, i, value, error);
    }

    return result_vector;
}

rikai_vector *rikai_multiply_vector_with_scalar(const rikai_vector *vector, double scalar, rikai_vector *product_vector_container, rikai_err *error)
{
    char error_message[4096];
    uint32_t vector_size;
    uint32_t product_vector_container_size;
    uint32_t i;
    double value;
    rikai_vector *result_vector = NULL;

    if (error)
        rikai_clear_error(error);

    if (vector == NULL)
    {
        rikai_set_error(error, "Null pointer Argument(1)", RIKAI_NULL_POINTER);
        return NULL;
    }

    vector_size = rikai_vector_get_size(vector, error);

    if (product_vector_container == NULL)
    {
        result_vector = rikai_vector_create_new(vector_size, error);
        if (result_vector == NULL)
            return NULL;
    }
    else
    {
        product_vector_container_size = rikai_vector_get_size(product_vector_container, error);
        if (product_vector_container_size != vector_size)
        {
            sprintf(error_message, "Provided vector container has size %u but expected %u", product_vector_container_size, vector_size);
            rikai_set_error(error, error_message, RIKAI_INVALID_VECTOR_CONTAINER_SIZE);
            return NULL;
        }
        result_vector = product_vector_container;
    }

    for (i = 0; i < vector_size; i++)
    {
        value = rikai_vector_get(vector, i, error);
        value *= scalar;
        rikai_vector_set(result_vector, i, value, error);
    }

    return result_vector;
}

double rikai_multiple_vector_get_scalar(const rikai_vector *left_vector, const rikai_vector *right_vector, rikai_err *error)
{
    char error_message[4096];
    uint32_t left_vector_size, right_vector_size;
    uint32_t i;
    double result = 0.0;

    if (error)
        rikai_clear_error(error);

    if (left_vector == NULL)
    {
        rikai_set_error(error, "Null pointer Argument(1)", RIKAI_NULL_POINTER);
        return 0.0;
    }

    if (right_vector == NULL)
    {
        rikai_set_error(error, "Null pointer Argument(2)", RIKAI_NULL_POINTER);
        return 0.0;
    }

    left_vector_size = rikai_vector_get_size(left_vector, error);
    right_vector_size = rikai_vector_get_size(right_vector, error);

    if (left_vector_size != right_vector_size)
    {
        sprintf(error_message, "Cannot multiply vectors of size %u and %u", left_vector_size, right_vector_size);
        rikai_set_error(error, error_message, RIKAI_VECTOR_SIZE_MISMATCH);
        return 0.0;
    }

    for (i = 0; i < left_vector_size; i++)
    {
        result += (rikai_vector_get(left_vector, i, error) * rikai_vector_get(right_vector, i, error));
    }

    return result;
}

double rikai_sum_vector(const rikai_vector *vector, rikai_err *error)
{
    uint32_t vector_size;
    uint32_t i;
    double sum = 0.0;

    if (error)
        rikai_clear_error(error);

    if (vector == NULL)
    {
        rikai_set_error(error, "Null pointer Argument", RIKAI_NULL_POINTER);
        return 0.0;
    }

    vector_size = rikai_vector_get_size(vector, error);

    for (i = 0; i < vector_size; i++)
        sum += rikai_vector_get(vector, i, error);

    return sum;
}

double rikai_mean_vector(const rikai_vector *vector, rikai_err *error)
{
    uint32_t vector_size;
    double sum;

    if (error)
        rikai_clear_error(error);

    if (vector == NULL)
    {
        rikai_set_error(error, "Null pointer Argument", RIKAI_NULL_POINTER);
        return 0.0;
    }

    vector_size = rikai_vector_get_size(vector, error);
    if (vector_size == 0)
    {
        rikai_set_error(error, "Vector size is zero", RIKAI_INVALID_SIZE);
        return 0.0;
    }

    sum = rikai_sum_vector(vector, error);
    return sum / vector_size;
}

rikai_vector *rikai_subtract_scalar_from_vector(const rikai_vector *vector, double value, rikai_vector *difference_vector_container, rikai_err *error)
{
    char error_message[4096];
    uint32_t vector_size;
    uint32_t difference_vector_container_size;
    uint32_t i;
    rikai_vector *result_vector = NULL;
    double result_value;

    if (error)
        rikai_clear_error(error);

    if (vector == NULL)
    {
        rikai_set_error(error, "Null pointer Argument(1)", RIKAI_NULL_POINTER);
        return NULL;
    }

    vector_size = rikai_vector_get_size(vector, error);

    if (difference_vector_container == NULL)
    {
        result_vector = rikai_vector_create_new(vector_size, error);
        if (result_vector == NULL)
            return NULL;
    }
    else
    {
        difference_vector_container_size = rikai_vector_get_size(difference_vector_container, error);
        if (difference_vector_container_size != vector_size)
        {
            sprintf(error_message, "Provided vector container has size %u but expected %u", difference_vector_container_size, vector_size);
            rikai_set_error(error, error_message, RIKAI_INVALID_VECTOR_CONTAINER_SIZE);
            return NULL;
        }
        result_vector = difference_vector_container;
    }

    for (i = 0; i < vector_size; i++)
    {
        result_value = rikai_vector_get(vector, i, error) - value;
        rikai_vector_set(result_vector, i, result_value, error);
    }

    return result_vector;
}