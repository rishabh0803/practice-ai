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

void __rikai_vector_destroy(rikai_vector **vector, rikai_err *error)
{
    if (error)
        rikai_clear_error(error);

    if (vector == NULL || *vector == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return;
    }

    rikai_matrix_destroy((*vector)->matrix, error);
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

void rikai_vector_write(const rikai_vector *vector, const char *filename, rikai_err *error)
{
    if (error)
        rikai_clear_error(error);

    if (vector == NULL)
    {
        rikai_set_error(error, "Null pointer Argument(1)", RIKAI_NULL_POINTER);
        return;
    }

    if (filename == NULL)
    {
        rikai_set_error(error, "Null pointer Argument(2)", RIKAI_NULL_POINTER);
        return;
    }

    rikai_matrix_write(vector->matrix, filename, error);
}

rikai_vector *rikai_vector_read(const char *filename, rikai_err *error)
{
    uint16_t error_code = 0;
    rikai_vector *vector = NULL;
    rikai_matrix *matrix = NULL;
    uint32_t rows, columns;

    if (error)
        rikai_clear_error(error);

    if (filename == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return NULL;
    }

    matrix = rikai_matrix_read(filename, error);
    if (matrix == NULL)
    {
        if (error)
        {
            error_code = rikai_get_error_code(error);
            if (error_code == RIKAI_INVALID_DIMENSIONS || error_code == RIKAI_INVALID_ROW_COUNT || error_code == RIKAI_INVALID_COLUMN_COUNT)
            {
                rikai_clear_error(error);
                rikai_set_error(error, "Invalid file format", RIKAI_FILE_FORMAT_ERROR);
            }
            return NULL;
        }
    }

    rikai_matrix_get_dimensions(matrix, &rows, &columns, error);
    if (rows > 1 && columns > 1)
    {
        rikai_set_error(error, "File contains matrix", RIKAI_FILE_CONTAINS_MATRIX);
        rikai_matrix_destroy(matrix, error);
        return NULL;
    }

    vector = (rikai_vector *)malloc(sizeof(rikai_vector));
    if (vector == NULL)
    {
        rikai_matrix_destroy(matrix, error);
        rikai_set_error(error, "Low memory", RIKAI_LOW_MEMORY);
        return NULL;
    }

    if (columns == 1)
    {
        vector->matrix = matrix;
    }
    else
    {
        rikai_matrix_transpose(matrix, error);
        vector->matrix = matrix;
    }

    return vector;
}

rikai_vector *rikai_vector_read_csv(const char *filename, int skip_lines, rikai_err *error)
{

    uint16_t error_code = 0;
    rikai_vector *vector = NULL;
    rikai_matrix *matrix = NULL;
    uint32_t rows, columns;
    rows = 0;
    columns = 0;

    if (error)
        rikai_clear_error(error);

    if (filename == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return NULL;
    }

    matrix = rikai_matrix_read_csv(filename, skip_lines, error);
    if (matrix == NULL)
    {
        if (error)
        {
            error_code = rikai_get_error_code(error);
            if (error_code == RIKAI_INVALID_DIMENSIONS || error_code == RIKAI_INVALID_ROW_COUNT || error_code == RIKAI_INVALID_COLUMN_COUNT)
            {
                rikai_clear_error(error);
                rikai_set_error(error, "Invalid file format", RIKAI_FILE_FORMAT_ERROR);
            }
            return NULL;
        }
    }

    rikai_matrix_get_dimensions(matrix, &rows, &columns, error);
    if (rows > 1 && columns > 1)
    {
        rikai_matrix_destroy(matrix, error);
        rikai_set_error(error, "File contains matrix", RIKAI_FILE_CONTAINS_MATRIX);
        return NULL;
    }

    vector = (rikai_vector *)malloc(sizeof(rikai_vector));
    if (vector == NULL)
    {
        rikai_matrix_destroy(matrix, error);
        rikai_set_error(error, "Low memory", RIKAI_LOW_MEMORY);
        return NULL;
    }

    if (columns == 1)
    {
        vector->matrix = matrix;
    }
    else
    {
        rikai_matrix_transpose(matrix, error);
        vector->matrix = matrix;
    }

    return vector;
}

void rikai_vector_print(FILE *file, const rikai_vector *vector, rikai_err *error)
{
    if (error)
        rikai_clear_error(error);

    if (file == NULL)
    {
        rikai_set_error(error, "Null pointer Argument (1)", RIKAI_NULL_POINTER);
        return;
    }

    if (vector == NULL)
    {
        rikai_set_error(error, "Null pointer Argument (2)", RIKAI_NULL_POINTER);
        return;
    }

    rikai_matrix_print(file, vector->matrix, error);
}