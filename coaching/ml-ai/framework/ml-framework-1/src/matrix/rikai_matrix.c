#include <rikai.h>
#include <stdlib.h>
#include <private_rikai.h>
#include <rikai_vector.h>

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

void __rikai_matrix_destroy(rikai_matrix **matrix, rikai_err *error)
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

void rikai_matrix_write_csv(const rikai_matrix *matrix, const char *filename, int skip_lines, rikai_err *error)
{
    FILE *f;
    char error_message[4096];
    uint32_t r, c;

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

    f = fopen(filename, "w");
    if (f == NULL)
    {
        sprintf(error_message, "Unable to create file %s", filename);
        rikai_set_error(error, error_message, RIKAI_FILE_WRITE_ERROR);
        return;
    }

    for (r = 0; r < matrix->rows; r++)
    {
        for (c = 0; c < matrix->columns; c++)
        {
            fprintf(f, "%lf", matrix->data[r * matrix->columns + c]);
            if (c != matrix->columns - 1)
                fputc(',', f);
        }
        fputc('\n', f);
    }
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

void rikai_matrix_transpose(rikai_matrix *matrix, rikai_err *error)
{
    uint32_t a, b;
    uint32_t new_rows, new_columns;
    double value;
    double *new_data;

    if (error)
        rikai_clear_error(error);

    if (matrix == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return;
    }

    if (matrix->rows == 1 && matrix->columns == 1)
        return;

    if (matrix->rows == 1 || matrix->columns == 1)
    {
        a = matrix->rows;
        matrix->rows = matrix->columns;
        matrix->columns = a;
        return;
    }

    if (matrix->rows == matrix->columns)
    {
        // Square matrix
        for (a = 0; a < matrix->rows; a++)
        {
            for (b = a + 1; b < matrix->columns; b++)
            {
                value = matrix->data[a * matrix->columns + b];
                matrix->data[a * matrix->columns + b] = matrix->data[b * matrix->columns + a];
                matrix->data[b * matrix->columns + a] = value;
            }
        }

        return;
    }

    // Non-square matrix
    new_rows = matrix->columns;
    new_columns = matrix->rows;
    new_data = (double *)malloc(new_rows * new_columns * sizeof(double));
    if (new_data == NULL)
    {
        rikai_set_error(error, "Low memory", RIKAI_LOW_MEMORY);
        return;
    }

    for (a = 0; a < matrix->rows; a++)
    {
        for (b = 0; b < matrix->columns; b++)
        {
            new_data[b * new_columns + a] = matrix->data[a * matrix->columns + b];
        }
    }
    free(matrix->data);
    matrix->data = new_data;
    matrix->rows = new_rows;
    matrix->columns = new_columns;
}

rikai_matrix *rikai_matrix_transpose_create_new(const rikai_matrix *matrix, rikai_err *error)
{
    uint32_t a, b;
    rikai_matrix *new_matrix;
    if (error)
        rikai_clear_error(error);

    if (matrix == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return NULL;
    }

    new_matrix = rikai_matrix_create_new(matrix->columns, matrix->rows, error);
    if (new_matrix == NULL)
        return NULL;

    for (a = 0; a < matrix->rows; a++)
    {
        for (b = 0; b < matrix->columns; b++)
        {
            new_matrix->data[b * new_matrix->columns + a] = matrix->data[a * matrix->columns + b];
        }
    }

    return new_matrix;
}

void rikai_matrix_print(FILE *file, const rikai_matrix *matrix, rikai_err *error)
{
    uint32_t a, b;

    if (error)
        rikai_clear_error(error);

    if (file == NULL)
    {
        rikai_set_error(error, "Null pointer Argument (1)", RIKAI_NULL_POINTER);
        return;
    }

    if (matrix == NULL)
    {
        rikai_set_error(error, "Null pointer Argument (2)", RIKAI_NULL_POINTER);
        return;
    }

    for (a = 0; a < matrix->rows; a++)
    {
        for (b = 0; b < matrix->columns; b++)
        {
            fprintf(file, "%12.6lf ", rikai_matrix_get(matrix, a, b, error));
        }
        fprintf(file, "\n");
    }
}

void rikai_matrix_fill(rikai_matrix *input_matrix,
                       uint32_t start_row_index, uint32_t start_column_index,
                       uint32_t end_row_index, uint32_t end_column_index,
                       double fill_value, rikai_err *error)
{
    uint32_t r, c;
    if (error)
        rikai_clear_error(error);

    if (input_matrix == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return;
    }

    if (end_row_index >= input_matrix->rows)
        end_row_index = input_matrix->rows - 1;

    if (end_column_index >= input_matrix->columns)
        end_column_index = input_matrix->columns - 1;

    for (r = start_row_index; r <= end_row_index; r++)
    {
        for (c = start_column_index; c <= end_column_index; c++)
        {
            input_matrix->data[r * input_matrix->columns + c] = fill_value;
        }
    }
}

void rikai_matrix_copy(rikai_matrix *target_matrix,
                       uint32_t target_start_row_index, uint32_t target_start_column_index,
                       const rikai_matrix *source_matrix,
                       uint32_t source_start_row_index, uint32_t source_start_column_index,
                       uint32_t source_end_row_index, uint32_t source_end_column_index,
                       rikai_err *error)
{

    uint32_t target_r, target_c;
    uint32_t source_r, source_c;
    uint32_t target_row_end_index, target_column_end_index;

    if (error)
        rikai_clear_error(error);

    if (target_matrix == NULL)
    {
        rikai_set_error(error, "Null pointer Argument (1)", RIKAI_NULL_POINTER);
        return;
    }

    if (source_matrix == NULL)
    {
        rikai_set_error(error, "Null pointer Argument (2)", RIKAI_NULL_POINTER);
        return;
    }

    if (target_start_row_index >= target_matrix->rows)
        return;

    if (target_start_column_index >= target_matrix->columns)
        return;

    if (source_end_row_index >= source_matrix->rows)
        source_end_row_index = source_matrix->rows - 1;

    if (source_end_column_index >= source_matrix->columns)
        source_end_column_index = source_matrix->columns - 1;

    target_row_end_index = target_start_row_index + (source_end_row_index - source_start_row_index);
    if (target_row_end_index >= target_matrix->rows)
        target_row_end_index = target_matrix->rows - 1;

    target_column_end_index = target_start_column_index + (source_end_column_index - source_start_column_index);
    if (target_column_end_index >= target_matrix->columns)
        target_column_end_index = target_matrix->columns - 1;

    source_r = source_start_row_index;
    for (target_r = target_start_row_index; target_r <= target_row_end_index; target_r++)
    {
        source_c = source_start_column_index;
        for (target_c = target_start_column_index; target_c <= target_column_end_index; target_c++)
        {
            target_matrix->data[target_r * target_matrix->columns + target_c] =
                source_matrix->data[source_r * source_matrix->columns + source_c];
            source_c++;
        }
        source_r++;
    }
}

rikai_vector *rikai_matrix_get_vector(const rikai_matrix *matrix,
                                      uint32_t start_row_index, uint32_t start_column_index,
                                      uint32_t end_row_index, uint32_t end_column_index,
                                      rikai_err *error)
{
    uint32_t r, c, i;
    uint32_t vector_size;
    uint32_t rows_to_pick, columns_to_pick;
    char error_message[4096];

    rikai_vector *vector;
    if (error)
        rikai_clear_error(error);

    if (matrix == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return NULL;
    }

    if (start_row_index >= matrix->rows || start_column_index >= matrix->columns)
    {
        sprintf(error_message, "Cannot create vector using (%u, %u) - (%u, %u)", start_row_index, start_column_index,
                end_row_index, end_column_index);
        rikai_set_error(error, error_message, RIKAI_GET_VECTOR_ERROR);
        return NULL;
    }

    if (end_row_index >= matrix->rows || end_column_index >= matrix->columns)
    {
        sprintf(error_message, "Cannot create vector using (%u, %u) - (%u, %u)", start_row_index, start_column_index,
                end_row_index, end_column_index);
        rikai_set_error(error, error_message, RIKAI_GET_VECTOR_ERROR);
        return NULL;
    }

    rows_to_pick = end_row_index - start_row_index + 1;
    columns_to_pick = end_column_index - start_column_index + 1;

    if (rows_to_pick > 1 && columns_to_pick > 1)
    {
        sprintf(error_message, "Cannot create vector using (%u, %u) - (%u, %u)", start_row_index, start_column_index,
                end_row_index, end_column_index);
        rikai_set_error(error, error_message, RIKAI_GET_VECTOR_ERROR);
        return NULL;
    }

    if (rows_to_pick == 1)
        vector_size = columns_to_pick;
    else
        vector_size = rows_to_pick;

    vector = rikai_vector_create_new(vector_size, error);
    if (vector == NULL)
        return NULL;

    if (rows_to_pick == 1)
    {
        r = start_row_index;
        i = 0;
        for (c = start_column_index; c <= end_column_index; c++)
        {
            rikai_vector_set(vector, i, matrix->data[r * matrix->columns + c], error);
            ++i;
        }
    }
    else
    {
        c = start_column_index;
        i = 0;
        for (r = start_row_index; r <= end_row_index; r++)
        {
            rikai_vector_set(vector, i, matrix->data[r * matrix->columns + c], error);
            ++i;
        }
    }

    return vector;
}

double *rikai_matrix_get_data(const rikai_matrix *matrix, rikai_err *error)
{
    if (error)
        rikai_clear_error(error);

    if (matrix == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return NULL;
    }

    return matrix->data;
}