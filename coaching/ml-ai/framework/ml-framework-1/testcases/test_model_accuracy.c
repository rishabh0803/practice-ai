#include <stdio.h>
#include <stdlib.h>
#include <rikai.h>
#include <private_rikai.h>

double rikai_r2_score(const rikai_vector *y_true, const rikai_vector *y_pred, rikai_err *error)
{
    // This function calculates the R2 score (coefficient of determination) for the given true and predicted values.
    // It should return the R2 score as a double value.

    if (y_true == NULL || y_pred == NULL)
    {
        rikai_set_error(error, "Null pointer argument", RIKAI_NULL_POINTER);
        return 0.0;
    }

    uint32_t size = rikai_vector_get_size(y_true, error);
    if (size != rikai_vector_get_size(y_pred, error))
    {
        rikai_set_error(error, "Size mismatch between y_true and y_pred", RIKAI_SIZE_MISMATCH);
        return 0.0;
    }

    // TODO: can be moved to a utility function
    double mean_y = 0.0;
    for (uint32_t i = 0; i < size; i++)
    {
        double true_value = rikai_vector_get(y_true, i, error);
        mean_y += true_value;
    }

    mean_y /= size;

    // Vectorized calculation of R2 score
    rikai_vector *numerator_diff = rikai_vector_create_new(size, error);
    rikai_vector *denominator_diff = rikai_vector_create_new(size, error);

    numerator_diff = rikai_subtract_vector(y_true, y_pred, numerator_diff, error);
    denominator_diff = rikai_subtract_scalar_from_vector(y_true, mean_y, denominator_diff, error);

    rikai_vector *numerator_diff_t = rikai_vector_transpose_create_new(numerator_diff, NULL, error);
    rikai_vector *denominator_diff_t = rikai_vector_transpose_create_new(denominator_diff, NULL, error);

    double ss_residual_vector = rikai_multiple_vector_get_scalar(numerator_diff_t, numerator_diff, error);
    double ss_total_vector = rikai_multiple_vector_get_scalar(denominator_diff_t, denominator_diff, error);

    printf("Vectorized R2 Score Calculation: SS Residual = %f, SS Total = %f\n", ss_residual_vector, ss_total_vector);

    return 1.0 - (ss_residual_vector / ss_total_vector);
}

int main(int c, char *v[])
{
    if (c < 3)
    {
        fprintf(stderr, "Usage: %s <test_dataset> <model_filename>\n", v[0]);
        return 1;
    }

    rikai_err *error = rikai_error_create_new();
    if (error == NULL)
    {
        fprintf(stderr, "Out Of memory\n");
        return 1;
    }

    char *test_filename = v[1];
    char *model_filename = v[2];

    char error_message[4096];
    rikai_matrix *dataset = NULL;
    rikai_matrix *input_matrix = NULL;

    rikai_vector *y = NULL;
    rikai_vector *predicted_y = NULL;

    rikai_vector *model = NULL;

    // Load the test dataset
    dataset = rikai_matrix_read_csv(test_filename, 0, error);

    // Load the model
    model = rikai_vector_read_csv(model_filename, 0, error);

    // Get the dimensions of the dataset
    uint32_t dataset_rows, dataset_cols;
    rikai_matrix_get_dimensions(dataset, &dataset_rows, &dataset_cols, error);

    // Split the dataset into input and output vector
    input_matrix = rikai_matrix_create_new(dataset_rows, dataset_cols, error);
    rikai_matrix_fill(input_matrix, 0, 0, dataset_rows - 1, 0, 1.0, error);
    rikai_matrix_copy(input_matrix, 0, 1, dataset, 0, 0, dataset_rows - 1, dataset_cols - 2, error);

    printf("Input Matrix Dimensions: %u x %u\n", dataset_rows, dataset_cols);
    printf("Model Size: %u\n", rikai_vector_get_size(model, error));

    y = rikai_matrix_get_vector(dataset, 0, dataset_cols - 1, dataset_rows - 1, dataset_cols - 1, error);

    // Run the model on the input data
    predicted_y = rikai_vector_create_new(dataset_rows, error);
    predicted_y = rikai_multiply_matrix_with_vector(input_matrix, model, predicted_y, error);

    FILE *output_file = fopen("predicted_y.csv", "w");
    rikai_vector_print(output_file, predicted_y, error);
    fclose(output_file);

    // Calculate the accuracy R2
    double r2 = rikai_r2_score(y, predicted_y, error);

    // Print the R2 score
    printf("R2 Score: %f\n", r2);

    return 0;
}