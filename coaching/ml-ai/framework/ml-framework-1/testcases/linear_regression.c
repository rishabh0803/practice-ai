#include <rikai.h>
#include <stdio.h>
#include <stdlib.h>

int main(int c, char *v[])
{
    rikai_err *error = rikai_error_create_new();
    if (error == NULL)
    {
        fprintf(stderr, "Out Of memory\n");
        return 1;
    }

    char *filename;
    char error_message[4096];
    rikai_matrix *dataset = NULL;
    rikai_matrix *input_matrix = NULL;
    rikai_vector *y = NULL;
    rikai_vector *y_hat = NULL;
    rikai_vector *model = NULL;

    uint64_t number_of_iterations, i;
    uint32_t dataset_rows, dataset_cols;
    uint32_t input_rows, input_cols;
    uint32_t model_size;
    double mean_aquared_error;

    filename = v[1];
    number_of_iterations = atol(v[2]);

    // Load the dataset
    dataset = rikai_matrix_read_csv(filename, 0, error);
    rikai_on_error_throw(error, err);

    rikai_matrix_get_dimensions(dataset, &dataset_rows, &dataset_cols, error);

    // Split the dataset into input and output matrices
    input_matrix = rikai_matrix_create_new(dataset_rows, dataset_cols, error);
    rikai_on_error_throw(error, err);

    rikai_matrix_get_dimensions(input_matrix, &input_rows, &input_cols, error);

    rikai_matrix_fill(input_matrix, 0, 0, input_rows - 1, 0, 1.0, error);
    rikai_matrix_copy(input_matrix, 0, 1, dataset, 0, 0, dataset_rows - 1, dataset_cols - 2, error);

    y = rikai_matrix_get_vector(dataset, 0, dataset_cols - 1, dataset_rows - 1, dataset_cols - 1, error);
    rikai_matrix_destroy(dataset, error);

    // Initialize the model with random values
    model_size = input_cols;
    model = rikai_vector_create_new(model_size, error);
    rikai_on_error_throw(error, err);

    rikai_vector_fill_random(model, error);

    // Train the model using gradient descent
    for (int i = 1; i < number_of_iterations; i++)
    {
        y_hat = rikai_multiply_matrix_with_vector(input_matrix, model, error);
        mean_aquared_error = rikai_mean_squared_error(y, y_hat, error);

        printf("Iteration %d: Mean Squared Error = %20.12lf\n", i, mean_aquared_error);
        rikai_vector_destroy(y_hat, error);
    }

    goto exit_program;

err:
    rikai_get_error(error_message, 4096, error);
    printf("Error: %s\n", error_message);

exit_program:

    rikai_matrix_destroy(dataset, error);
    rikai_matrix_destroy(input_matrix, error);
    rikai_vector_destroy(y, error);
    rikai_vector_destroy(y_hat, error);
    rikai_vector_destroy(model, error);
    rikai_error_destroy(error);

    return 0;
}