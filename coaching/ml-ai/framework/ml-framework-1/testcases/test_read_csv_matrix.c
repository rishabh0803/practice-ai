#include <stdio.h>
#include <rikai.h>

int main()
{
    rikai_err *error = NULL;
    rikai_matrix *matrix;

    char error_message[501];
    double value;

    uint32_t rows, columns;

    error = rikai_error_create_new();
    if (error == NULL)
    {
        printf("Error: Out of memory\n");
        return -1;
    }

    // Read the matrix from the file
    matrix = rikai_matrix_read_csv("matrix.csv",0, error);
    if (rikai_has_error(error))
    {
        rikai_get_error(error_message, 501, error);
        printf("%s\n", error_message);
        rikai_matrix_destory(matrix, error);
        return -1;
    }

    // Get and print the values from the matrix
    rikai_matrix_get_dimensions(matrix, &rows, &columns, error);
    for (uint32_t i = 0; i < rows; i++)
    {
        for (uint32_t j = 0; j < columns; j++)
        {
            value = rikai_matrix_get(matrix, i, j, error);
            if (rikai_has_error(error))
            {
                rikai_get_error(error_message, 501, error);
                printf("%s\n", error_message);
                rikai_matrix_destory(matrix, error);
                return -1;
            }

            printf("%lf   ", value);
        }
        printf("\n");
    }

    // Clean up
    rikai_matrix_destory(matrix, error);
    rikai_error_destroy(error);

    return 0;
}