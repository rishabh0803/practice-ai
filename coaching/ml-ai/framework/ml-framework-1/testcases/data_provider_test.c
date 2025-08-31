#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <rikai.h>
#include <private_rikai.h>

typedef struct __rikai_data_provider rikai_data_provider;

typedef struct __rikai_data_provider
{
    char *filename;
    FILE *file;
    uint32_t number_of_columns;
    uint8_t number_of_lines_in_header;
    void (*get_next_batch)(rikai_data_provider *data_provider, rikai_matrix *matrix, uint32_t *rows_provided, rikai_err *error);
} rikai_data_provider;

rikai_data_provider *rikai_data_provider_create_new(
    const char *filename,
    uint8_t number_of_lines_in_header,
    void (*get_next_batch)(rikai_data_provider *self, rikai_matrix *matrix, uint32_t *rows_provided, rikai_err *error),
    rikai_err *error)
{
    rikai_data_provider *data_provider = NULL;
    if (error)
        rikai_clear_error(error);

    if (filename == NULL)
    {
        rikai_set_error(error, "Null pointer argument 1", RIKAI_NULL_POINTER);
        return NULL;
    }

    if (get_next_batch == NULL)
    {
        rikai_set_error(error, "Null pointer argument 3", RIKAI_NULL_POINTER);
        return NULL;
    }

    data_provider = (rikai_data_provider *)malloc(sizeof(rikai_data_provider));
    if (data_provider == NULL)
    {
        rikai_set_error(error, "Low memory", RIKAI_LOW_MEMORY);
        return NULL;
    }

    data_provider->number_of_lines_in_header = number_of_lines_in_header;
    data_provider->get_next_batch = get_next_batch;

    data_provider->filename = (char *)malloc(strlen(filename) + 1);
    if (data_provider->filename == NULL)
    {
        free(data_provider);
        rikai_set_error(error, "Low memory", RIKAI_LOW_MEMORY);
        return NULL;
    }

    strcpy(data_provider->filename, filename);
    data_provider->file = NULL;
    data_provider->number_of_columns = 0;

    return data_provider;
}

void rikai_data_provider_destroy(rikai_data_provider *data_provider, rikai_err *error)
{
    if (error)
        rikai_clear_error(error);
    if (data_provider == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return;
    }

    if (data_provider->filename)
        free(data_provider->filename);
    if (data_provider->file)
        fclose(data_provider->file);
    free(data_provider);
}

uint32_t rikai_data_provider_get_column_count(rikai_data_provider *data_provider, rikai_err *error)
{
    int skip_lines;
    char error_message[4096];
    char m;
    uint32_t number_of_columns;

    if (error)
        rikai_clear_error(error);
    if (data_provider == NULL)
    {
        rikai_set_error(error, "Null pointer data", RIKAI_NULL_POINTER);
        return 0;
    }

    if (data_provider->number_of_columns == 0)
    {
        data_provider->file = fopen(data_provider->filename, "r");
        if (data_provider->file == NULL)
        {
            sprintf(error_message, "Cannot open file %s", data_provider->filename);
            rikai_set_error(error, error_message, RIKAI_FILE_READ_ERROR);
            return 0;
        }

        skip_lines = data_provider->number_of_lines_in_header;
        while (skip_lines > 0)
        {
            m = fgetc(data_provider->file);
            if (feof(data_provider->file))
                break;
            if (m == '\n')
                skip_lines--;
        }

        if (skip_lines > 0)
        {
            sprintf(error_message, "Invalid CSV format of file %s", data_provider->filename);
            rikai_set_error(error, error_message, RIKAI_CSV_FORMAT_ERROR);
            fclose(data_provider->file);
            data_provider->file = NULL;
            return 0;
        }

        number_of_columns = 0;
        while (1)
        {
            m = fgetc(data_provider->file);
            if (feof(data_provider->file) || m == '\n')
                break;

            if (m == ',')
                number_of_columns++;
        }

        if (m != '\n')
        {
            sprintf(error_message, "Invalid CSV format of file %s", data_provider->filename);
            rikai_set_error(error, error_message, RIKAI_CSV_FORMAT_ERROR);
            fclose(data_provider->file);
            data_provider->file = NULL;
            return 0;
        }

        data_provider->number_of_columns = number_of_columns + 1;
        fclose(data_provider->file);
        data_provider->file = NULL;
    }

    return data_provider->number_of_columns;
}

// ----------------------------------------------------------------------------------------

void do_something_like_fit(rikai_data_provider *data_provider, rikai_err *error)
{
    uint32_t j, c;
    double value;
    uint32_t rows_filled_by_provider;
    rikai_matrix *matrix = NULL;
    uint32_t matrix_rows;
    uint32_t number_of_columns;

    if (error)
        rikai_clear_error(error);
    if (data_provider == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return;
    }

    number_of_columns = rikai_data_provider_get_column_count(data_provider, error);
    matrix_rows = 70;
    matrix = rikai_matrix_create_new(matrix_rows, number_of_columns, error);

    while (1)
    {
        data_provider->get_next_batch(data_provider, matrix, &rows_filled_by_provider, error);
        if (rikai_has_error(error))
            break;

        printf("Rows filled by provider: %u\n", rows_filled_by_provider);

        if (rows_filled_by_provider == 0)
        {
            printf("No more rows to read.\n");
            break;
        }

        for (j = 0; j < rows_filled_by_provider; j++)
        {
            for (c = 0; c < number_of_columns; c++)
            {
                value = rikai_matrix_get(matrix, j, c, error);
                printf("%f     ", value);
            }
            printf("\n");
        }

        if (rows_filled_by_provider < matrix_rows)
        {
            printf("No more rows to read.\n");
            break;
        }
    }

    rikai_matrix_destroy(matrix, error);
}

// ----------------------------------------------------------------------------------------

void kalu(rikai_data_provider *data_provider, rikai_matrix *matrix, uint32_t *rows_provided, rikai_err *error)
{
    char m;
    char str[51];
    double value;
    uint32_t r, c;
    int i;
    int number_of_columns;
    int skip_lines;
    char error_message[4096];
    uint32_t matrix_rows, matrix_columns;

    if (error)
        rikai_clear_error(error);

    if (rows_provided)
        *rows_provided = 0;

    if (matrix == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return;
    }

    number_of_columns = rikai_data_provider_get_column_count(data_provider, error);
    if (number_of_columns == 0)
        return;

    rikai_matrix_get_dimensions(matrix, &matrix_rows, &matrix_columns, error);
    if (matrix_columns != number_of_columns)
    {
        sprintf(error_message, "Matrix has %u columns but data provider has %u columns", matrix_columns, number_of_columns);
        rikai_set_error(error, error_message, RIKAI_INVALID_MATRIX_CONTAINER_DIMENSIONS);
        return;
    }

    if (data_provider->file == NULL)
    {
        data_provider->file = fopen(data_provider->filename, "r");
        if (data_provider->file == NULL)
        {
            sprintf(error_message, "Cannot open file %s", data_provider->filename);
            rikai_set_error(error, error_message, RIKAI_FILE_READ_ERROR);
            return;
        }

        skip_lines = data_provider->number_of_lines_in_header;
        while (skip_lines > 0)
        {
            m = fgetc(data_provider->file);
            if (feof(data_provider->file))
                break;
            if (m == '\n')
                skip_lines--;
        }
    }

    i = 0;
    r = 0;
    c = 0;
    while (r < matrix_rows)
    {
        m = fgetc(data_provider->file);
        if (feof(data_provider->file))
            break;
        if (m == '\n' || m == ',')
        {
            str[i] = '\0';
            i = 0;
            value = atof(str);
            rikai_matrix_set(matrix, r, c, value, error);
            c++;
            if (c == matrix_columns)
            {
                r++;
                c = 0;
            }
        }
        else
        {
            str[i++] = m;
        }
    }

    if (rows_provided)
        *rows_provided = r;
}

int main()
{
    char error_message[4096];
    rikai_err *error = rikai_error_create_new();
    rikai_data_provider *data_provider = rikai_data_provider_create_new("..\\datasets\\linear_data.csv", 0, kalu, error);
    if (rikai_has_error(error))
    {
        printf("Error \n");
        return 0;
    }

    do_something_like_fit(data_provider, error);
    if (rikai_has_error(error))
    {
        rikai_get_error(error_message, 4000, error);
        printf("Some issues: %s\n", error_message);
        return 0;
    }

    rikai_data_provider_destroy(data_provider, error);
    rikai_error_destroy(error);
    return 0;
}