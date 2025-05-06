#include <rikai.h>
#include <stdlib.h>

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
    }

    if (rows == 0)
    {
    }

    if (columns == 0)
    {
    }

    rikai_matrix *matrix;
    matrix = (rikai_matrix *)malloc(sizeof(rikai_matrix));

    if (matrix == NULL)
    {
    }

    matrix->data = (double *)malloc(rows * columns * sizeof(double));
    if (matrix->data == NULL)
    {
        free(matrix);
        matrix = NULL;
    }

    matrix->rows = rows;
    matrix->columns = columns;
    return matrix;
}

void __rikai_matrix_destory(rikai_matrix **matrix, rikai_err *error)
{
    if (matrix == NULL || *matrix == NULL)
    {
    }

    free((*matrix)->data);
    (*matrix)->data = NULL;

    free(*matrix);
    *matrix = NULL;
}

void rikai_matrix_set(rikai_matrix *matrix, uint32_t row, uint32_t column, double value, rikai_err *error)
{
    if (matrix == NULL)
    {
    }

    if (row >= matrix->rows && column >= matrix->columns)
    {
    }

    if (row >= matrix->rows)
    {
    }

    if (column >= matrix->columns)
    {
    }

    matrix->data[row * matrix->columns + column] = value;
}

double rikai_matrix_get(const rikai_matrix *matrix, uint32_t row, uint32_t column, rikai_err *error)
{
    if (matrix == NULL)
    {
    }

    if (row >= matrix->rows && column >= matrix->columns)
    {
    }

    if (row >= matrix->rows)
    {
    }

    if (column >= matrix->columns)
    {
    }

    return matrix->data[row * matrix->columns + column];
}

void rikai_matrix_get_dimensions(const rikai_matrix *matrix, uint32_t *rows, uint32_t *columns, rikai_err *error)
{
    if (matrix == NULL)
    {
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
    }

    return matrix->rows;
}

uint32_t rikai_matrix_get_column_count(const rikai_matrix *matrix, rikai_err *error)
{
    if (matrix == NULL)
    {
    }

    return matrix->columns;
}