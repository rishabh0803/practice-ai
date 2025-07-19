#include <rikai.h>
#include <private_rikai.h>
#include <stdio.h>
#include <stdlib.h>

rikai_vector *rikai_linear_regression_predict(const rikai_matrix *x_matrix, const rikai_vector *model_vector, rikai_err *error)
{

    rikai_matrix *X_matrix = NULL;
    rikai_vector *Y_HAT_vector = NULL;

    char error_message[4096];
    uint32_t x_matrix_rows, x_matrix_cols;
    uint32_t model_vector_size;

    if (error)
        rikai_clear_error(error);

    if (x_matrix == NULL)
    {
        rikai_set_error(error, "Null pointer argument (1)", RIKAI_NULL_POINTER);
        goto catch_error;
    }

    if (model_vector == NULL)
    {
        rikai_set_error(error, "Null pointer argument (2)", RIKAI_NULL_POINTER);
        goto catch_error;
    }

    rikai_matrix_get_dimensions(x_matrix, &x_matrix_rows, &x_matrix_cols, error);
    model_vector_size = rikai_vector_get_size(model_vector, error);

    if (model_vector_size != x_matrix_cols + 1)
    {
        sprintf(error_message, "Size mismatch: model_vector size (%u), required size (%u)", model_vector_size, x_matrix_cols + 1);
        rikai_set_error(error, error_message, RIKAI_SIZE_MISMATCH);
        goto catch_error;
    }

    X_matrix = rikai_matrix_create_new(x_matrix_rows, x_matrix_cols + 1, error);
    if (X_matrix == NULL)
        goto catch_error;

    // Copy x_matrix to X_matrix and add a bias term (1.0) in the first column
    rikai_matrix_fill(X_matrix, 0, 0, x_matrix_rows - 1, 0, 1.0, NULL);
    rikai_matrix_copy(X_matrix, 0, 1, x_matrix, 0, 0, x_matrix_rows - 1, x_matrix_cols - 1, NULL);

    // Create a vector to hold the predictions
    Y_HAT_vector = rikai_multiply_matrix_with_vector(X_matrix, model_vector, NULL, error);
    if (Y_HAT_vector == NULL)
        goto catch_error;

catch_error:
    rikai_matrix_destroy(X_matrix, error);

    return Y_HAT_vector;
}

// --------------------------------------------------------------------------------------------

// Write test cases for linear regression accuracy
int main(int argc, char *argv[])
{
    char error_message[4096];
    rikai_err *error = NULL;
    rikai_matrix *dataset = NULL;
    uint32_t dataset_rows, dataset_cols;

    rikai_matrix *x = NULL;
    uint32_t x_rows, x_cols;

    rikai_vector *model = NULL;
    rikai_vector *y = NULL;
    rikai_vector *y_hat = NULL;

    double r2_score;
    char *dataset_filename;
    char *model_filename;

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <dataset> <model_filename>\n", argv[0]);
        return 1;
    }

    dataset_filename = argv[1];
    model_filename = argv[2];

    error = rikai_error_create_new();
    if (error == NULL)
    {
        fprintf(stderr, "Out of memory\n");
        return 1;
    }

    // Load the dataset
    dataset = rikai_matrix_read_csv(dataset_filename, 0, error);
    rikai_on_error_throw(error, err);

    // Load model
    model = rikai_vector_read_csv(model_filename, 0, error);
    rikai_on_error_throw(error, err);

    // Get dataset dimensions
    rikai_matrix_get_dimensions(dataset, &dataset_rows, &dataset_cols, error);
    x = rikai_matrix_create_new(dataset_rows, dataset_cols - 1, error);
    rikai_on_error_throw(error, err);

    // Get dimensions of x
    rikai_matrix_get_dimensions(x, &x_rows, &x_cols, error);

    // Copy dataset to x, excluding the last column
    rikai_matrix_copy(x, 0, 0, dataset, 0, 0, dataset_rows - 1, dataset_cols - 2, error);

    // Get the last column as y
    y = rikai_matrix_get_vector(dataset, 0, dataset_cols - 1, dataset_rows - 1, dataset_cols - 1, error);
    rikai_on_error_throw(error, err);
    rikai_matrix_destroy(dataset, error);

    // Predict y_hat using the model
    y_hat = rikai_linear_regression_predict(x, model, error);
    rikai_on_error_throw(error, err);

    // Calculate R2 score
    r2_score = rikai_r2_score(y, y_hat, error);

    printf("R2 Score: %f\n", r2_score);
    goto exit_program;

err:
    rikai_get_error(error_message, sizeof(error_message), error);
    printf("Error: %s\n", error_message);
exit_program:
    rikai_matrix_destroy(dataset, error);
    rikai_matrix_destroy(x, error);
    rikai_vector_destroy(y, error);
    rikai_vector_destroy(y_hat, error);
    rikai_vector_destroy(model, error);
    rikai_error_destroy(error);

    return 0;
}
