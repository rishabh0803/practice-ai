#include <rikai.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    rikai_err *error = NULL;
    rikai_matrix *matrix;

    uint32_t r, c;
    uint32_t rows, columns;
    char *filename;
    char error_message[4096];
    double n, d;

    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <filename> <rows> <columns>\n", argv[0]);
        return EXIT_FAILURE;
    }

    filename = argv[1];
    rows = atol(argv[2]);
    columns = atol(argv[3]);

    error = rikai_error_create_new();
    if (error == NULL)
    {
        fprintf(stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }

    matrix = rikai_matrix_create_new(rows, columns, error);
    if (matrix == NULL)
    {
        fprintf(stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }

    srand(time(NULL));
    for (r = 0; r < rows; r++)
    {
        for (c = 0; c < columns; c++)
        {
            n = rand();
            d = rand();
            if (d == 0)
                d = 3.0;
            rikai_matrix_set(matrix, r, c, (double)n / d, error);
        }
    }

    rikai_matrix_write_csv(matrix, filename, 0, error);
    if (rikai_has_error(error))
    {
        rikai_get_error(error_message, 4096, error);
        fprintf(stderr, "Error writing matrix to CSV: %s\n", error_message);
        return EXIT_FAILURE;
    }
    else
    {
        printf("Matrix written to %s successfully.\n", filename);
    }

    rikai_matrix_destroy(matrix, error);
    rikai_error_destroy(error);

    return 0;
}