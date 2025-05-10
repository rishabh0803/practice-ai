#include <rikai.h>
#include <stdlib.h>
#include <private_rikai.h>

typedef struct __rikai_matrix
{
    uint32_t rows;
    uint32_t columns;
    double *data;
} rikai_matrix;

rikai_matrix *rikai_matrix_create_new(uint32_t rows, uint32_t columns, rikai_err *error)
{
    if (rows == 0 && columns == 0)
    {
        rikai_set_error(error, "Invalid row count(0) and column count(0)", RIKAI_INVALID_DIMENSIONS);
        return NULL;
    }

    if (rows == 0)
    {
        rikai_set_error(error, "Invalid row count(0)", RIKAI_INVALID_ROW_COUNT);
        return NULL;
    }

    if (columns == 0)
    {
        rikai_set_error(error, "Invalid column count(0)", RIKAI_INVALID_COLUMN_COUNT);
        return NULL;
    }

    rikai_matrix *matrix;
    matrix = (rikai_matrix *)malloc(sizeof(rikai_matrix));

    if (matrix == NULL)
    {
        rikai_set_error(error, "Low memory", RIKAI_LOW_MEMORY);
        return NULL;
    }

    matrix->data = (double *)malloc(rows * columns * sizeof(double));
    if (matrix->data == NULL)
    {
        rikai_set_error(error, "Low memory", RIKAI_LOW_MEMORY);
        free(matrix);
        return NULL;
    }

    matrix->rows = rows;
    matrix->columns = columns;
    return matrix;
}

void __rikai_matrix_destory(rikai_matrix **matrix, rikai_err *error)
{
    if (matrix == NULL || *matrix == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return;
    }

    free((*matrix)->data);
    (*matrix)->data = NULL;

    free(*matrix);
    *matrix = NULL;
}

void rikai_matrix_set(rikai_matrix *matrix, uint32_t row, uint32_t column, double value, rikai_err *error)
{
    char error_message[70];
    if (matrix == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return;
    }

    if (row >= matrix->rows && column >= matrix->columns)
    {
        sprintf(error_message, "Invalid row :(%u) and column :(%u) index", row, column);
        rikai_set_error(error, error_message, RIKAI_INVALID_ROW_AND_COLUMN_INDEX);
        return;
    }

    if (row >= matrix->rows)
    {
        sprintf(error_message, "Invalid row index :(%u)", row);
        rikai_set_error(error, error_message, RIKAI_INVALID_ROW_INDEX);
        return;
    }

    if (column >= matrix->columns)
    {
        sprintf(error_message, "Invalid column index :(%u)", column);
        rikai_set_error(error, error_message, RIKAI_INVALID_COLUMN_INDEX);
        return;
    }

    matrix->data[row * matrix->columns + column] = value;
}

double rikai_matrix_get(const rikai_matrix *matrix, uint32_t row, uint32_t column, rikai_err *error)
{
    char error_message[70];
    if (matrix == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return 0.0;
    }

    if (row >= matrix->rows && column >= matrix->columns)
    {
        sprintf(error_message, "Invalid row :(%u) and column :(%u) index", row, column);
        rikai_set_error(error, error_message, RIKAI_INVALID_ROW_AND_COLUMN_INDEX);
        return 0.0;
    }

    if (row >= matrix->rows)
    {
        sprintf(error_message, "Invalid row index :(%u)", row);
        rikai_set_error(error, error_message, RIKAI_INVALID_ROW_INDEX);
        return 0.0;
    }

    if (column >= matrix->columns)
    {
        sprintf(error_message, "Invalid column index :(%u)", column);
        rikai_set_error(error, error_message, RIKAI_INVALID_COLUMN_INDEX);
        return 0.0;
    }

    return matrix->data[row * matrix->columns + column];
}

void rikai_matrix_get_dimensions(const rikai_matrix *matrix, uint32_t *rows, uint32_t *columns, rikai_err *error)
{
    if (matrix == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return;
    }

    if (rows)
        *rows = matrix->rows;
    if (columns)
        *columns = matrix->columns;
}

uint32_t rikai_matrix_get_row_count(const rikai_matrix *matrix, rikai_err *error)
{
    if (matrix == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return 0;
    }

    return matrix->rows;
}

uint32_t rikai_matrix_get_column_count(const rikai_matrix *matrix, rikai_err *error)
{
    if (matrix == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return 0;
    }

    return matrix->columns;
}