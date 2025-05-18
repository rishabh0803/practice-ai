#include <rikai.h>
#include <stdlib.h>
#include <private_rikai.h>

typedef struct __rikai_vector
{
    rikai_matrix *matrix;
} rikai_vector;

rikai_vector *rikai_vector_create_new(uint32_t size, rikai_err *error)
{
    if (error)
        rikai_clear_error(error);

    if (size == 0)
    {
        rikai_set_error(error, "Invalid size(0)", RIKAI_INVALID_SIZE);
        return NULL;
    }

    rikai_vector *vector;
    vector = (rikai_vector *)malloc(sizeof(rikai_vector));
    if (vector == NULL)
    {
        rikai_set_error(error, "Low memory", RIKAI_LOW_MEMORY);
        return NULL;
    }

    vector->matrix = rikai_matrix_create_new(size, 1, error);
    if (vector->matrix == NULL)
    {
        free(vector);
        return NULL;
    }

    return vector;
}

void __rikai_vector_destory(rikai_vector **vector, rikai_err *error)
{
    if (error)
        rikai_clear_error(error);

    if (vector == NULL || *vector == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return;
    }

    rikai_matrix_destory((*vector)->matrix, error);
    free(*vector);
    *vector = NULL;
}

void rikai_vector_set(rikai_vector *vector, uint32_t index, double value, rikai_err *error)
{
    char error_message[70];
    if (error)
        rikai_clear_error(error);

    if (vector == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return;
    }

    if (index >= rikai_matrix_get_row_count(vector->matrix, error))
    {
        sprintf(error_message, "Invalid index :(%u)", index);
        rikai_set_error(error, error_message, RIKAI_INVALID_INDEX);
        return;
    }

    rikai_matrix_set(vector->matrix, index, 0, value, error);
}

double rikai_vector_get(const rikai_vector *vector, uint32_t index, rikai_err *error)
{
    char error_message[70];

    if (error)
        rikai_clear_error(error);

    if (vector == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return 0.0;
    }

    if (index >= rikai_matrix_get_row_count(vector->matrix, error))
    {
        sprintf(error_message, "Invalid index :(%u)", index);
        rikai_set_error(error, error_message, RIKAI_INVALID_INDEX);
        return 0.0;
    }

    return rikai_matrix_get(vector->matrix, index, 0, error);
}

uint32_t rikai_vector_get_size(const rikai_vector *vector, rikai_err *error)
{
    if (error)
        rikai_clear_error(error);

    if (vector == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return 0;
    }

    return rikai_matrix_get_row_count(vector->matrix, error);
}

void rikai_vector_write(const rikai_vector *vector, const char *filename, rikai_err *error);

rikai_vector *rikai_vector_read(const char *filename, rikai_err *error);

rikai_vector *rikai_vector_read_csv(const char *filename, int skip_lines, rikai_err *error);