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
    if (error)
        rikai_clear_error(error);

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
    if (error)
        rikai_clear_error(error);

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
    if (error)
        rikai_clear_error(error);

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

    if (error)
        rikai_clear_error(error);

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
    if (error)
        rikai_clear_error(error);

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
    if (error)
        rikai_clear_error(error);

    if (matrix == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return 0;
    }

    return matrix->rows;
}

uint32_t rikai_matrix_get_column_count(const rikai_matrix *matrix, rikai_err *error)
{
    if (error)
        rikai_clear_error(error);

    if (matrix == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return 0;
    }

    return matrix->columns;
}

void rikai_matrix_write(const rikai_matrix *matrix, const char *filename, rikai_err *error)
{
    FILE *f;
    char error_message[4096];

    if (error)
        rikai_clear_error(error);

    if (matrix == NULL)
    {
        rikai_set_error(error, "Null pointer Argument (1)", RIKAI_NULL_POINTER);
        return;
    }

    if (filename == NULL)
    {
        rikai_set_error(error, "Null pointer Argument (2)", RIKAI_NULL_POINTER);
        return;
    }

    f = fopen(filename, "wb");
    if (f == NULL)
    {
        sprintf(error_message, "Unable to create file %s", filename);
        rikai_set_error(error, error_message, RIKAI_FILE_WRITE_ERROR);
        return;
    }

    fwrite(&(matrix->rows), sizeof(uint32_t), 1, f);
    fwrite(&(matrix->columns), sizeof(uint32_t), 1, f);
    fwrite(matrix->data, sizeof(double), matrix->rows * matrix->columns, f);
    fclose(f);
}

rikai_matrix *rikai_matrix_read(const char *filename, rikai_err *error)
{
    FILE *f;
    rikai_matrix *matrix;
    uint32_t rows, columns;
    char error_message[4096];

    if (error)
        rikai_clear_error(error);

    if (filename == NULL)
    {
        rikai_set_error(error, "Null pointer Argument (1)", RIKAI_NULL_POINTER);
        return NULL;
    }

    f = fopen(filename, "rb");
    if (f == NULL)
    {
        sprintf(error_message, "Unable to read file %s", filename);
        rikai_set_error(error, error_message, RIKAI_FILE_READ_ERROR);
        return NULL;
    }

    // Verify the length of the file is correct or not depending on the contents of first 8 bytes
    fread(&rows, sizeof(uint32_t), 1, f);
    fread(&columns, sizeof(uint32_t), 1, f);

    matrix = rikai_matrix_create_new(rows, columns, error);
    if (matrix == NULL)
    {
        fclose(f);
        return NULL;
    }

    fread(matrix->data, sizeof(double), rows * columns, f);
    fclose(f);

    return matrix;
}

rikai_matrix *rikai_matrix_read_csv(const char *filename, int skip_lines, rikai_err *error)
{
    FILE *f;
    char error_message[4096];
    char m, last_character;
    char str[51];
    double value;
    rikai_matrix *matrix;
    uint32_t rows = 0, columns = 0;
    uint32_t previous_row_commas, current_row_commas;
    long data_starts_at;
    uint32_t r, c;
    int i;

    if (error)
        rikai_clear_error(error);

    if (filename == NULL)
    {
        rikai_set_error(error, "Null pointer Argument (1)", RIKAI_NULL_POINTER);
        return NULL;
    }

    f = fopen(filename, "r");
    if (f == NULL)
    {
        sprintf(error_message, "Unable to read file %s", filename);
        rikai_set_error(error, error_message, RIKAI_FILE_READ_ERROR);
        return NULL;
    }

    while (skip_lines > 0)
    {
        m = fgetc(f);
        if (feof(f))
            break;
        if (m == '\n')
            skip_lines--;
    }

    if (skip_lines > 0)
    {
        sprintf(error_message, "Invalid csv format of file %s", filename);
        rikai_set_error(error, error_message, RIKAI_CSV_FORMAT_ERROR);
        fclose(f);
        return NULL;
    }

    data_starts_at = ftell(f);

    previous_row_commas = 0;
    current_row_commas = 0;
    rows = 0;
    while (1)
    {
        m = fgetc(f);
        if (feof(f))
            break;
        if (m == ',')
            current_row_commas++;
        if (m == '\n')
        {
            if (previous_row_commas != 0 && previous_row_commas != current_row_commas)
            {
                sprintf(error_message, "Invalid csv format of file %s", filename);
                rikai_set_error(error, error_message, RIKAI_CSV_FORMAT_ERROR);
                fclose(f);
                return NULL;
            }

            previous_row_commas = current_row_commas;
            current_row_commas = 0;
            rows++;
        }
        last_character = m;
    }

    columns = previous_row_commas + 1;
    if (columns == 0)
    {
        sprintf(error_message, "Empty csv file %s", filename);
        rikai_set_error(error, error_message, RIKAI_CSV_EMPTY_ERROR);
        fclose(f);
        return NULL;
    }

    if (last_character != '\n')
        rows++;

    matrix = rikai_matrix_create_new(rows, columns, error);
    if (matrix == NULL)
    {
        fclose(f);
        return NULL;
    }

    fseek(f, data_starts_at, SEEK_SET);
    r = 0;
    c = 0;
    i = 0;

    while (1)
    {
        m = fgetc(f);
        if (feof(f))
            break;

        if (m == ',' || m == '\n')
        {
            str[i] = '\0';
            value = atof(str);
            rikai_matrix_set(matrix, r, c, value, error);
            i = 0;
            c++;

            if (columns == c)
            {
                c = 0;
                r++;
            }
        }
        else
        {
            str[i++] = m;
        }
    }

    fclose(f);
    return matrix;
}
