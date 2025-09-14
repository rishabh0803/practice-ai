#include <rikai.h>

// ----------------------------- Usage of Data Provider --------------------------
void do_something_like_fit(rikai_data_provider *data_provider, rikai_err *error)
{
    uint32_t j, c;
    double value;
    uint32_t rows_filled_by_provider;
    rikai_matrix *matrix = NULL;
    uint32_t matrix_rows;
    uint32_t number_of_columns;

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

// ----------------------------- Main Function ----------------------------------
int main()
{
    char error_message[4096];
    rikai_err *error = rikai_error_create_new();

    rikai_data_provider *data_provider = rikai_data_provider_create_new(
        "..\\datasets\\kaggle_2_train.csv",
        0, default_get_next_batch,
        error);

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