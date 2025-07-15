#include <rikai.h>
#include <private_rikai.h>
#include <stdio.h>
#include <stdlib.h>

rikai_vector *rikai_linear_regression_fit(const rikai_matrix *x_matrix, const rikai_vector *y_vector, uint64_t number_of_iterations, double learning_rate, uint8_t (*on_iteration_complete)(uint64_t, double, rikai_vector *), rikai_err *error)
{
    // This function is a placeholder for the actual implementation of linear regression fitting.
    // It should return a model vector after training on the input matrix and output vector.

    rikai_matrix *X_matrix = NULL;
    rikai_matrix *XT_matrix = NULL;

    rikai_vector *Y_HAT_vector = NULL;
    rikai_vector *E_vector = NULL;
    rikai_vector *ET_vector = NULL;
    rikai_vector *XTE_vector = NULL;

    rikai_vector *model_vector = NULL;

    char error_message[4096];
    double error_value = 0.0;
    uint64_t i;

    uint32_t x_matrix_rows, x_matrix_columns;
    uint32_t y_vector_size;

    uint32_t X_matrix_rows, X_matrix_columns;
    uint32_t model_vector_size;

    if (error)
        rikai_clear_error(error);

    if (x_matrix == NULL)
    {
        rikai_set_error(error, "Null pointer argument (1)", RIKAI_NULL_POINTER);
        goto catch_error;
    }

    if (y_vector == NULL)
    {
        rikai_set_error(error, "Null pointer argument (2)", RIKAI_NULL_POINTER);
        goto catch_error;
    }

    if (number_of_iterations == 0)
    {
        rikai_set_error(error, "Cannot train with zero iterations", RIKAI_INVALID_NUMBER_OF_ITERATIONS);
        goto catch_error;
    }

    rikai_matrix_get_dimensions(x_matrix, &x_matrix_rows, &x_matrix_columns, error);
    y_vector_size = rikai_vector_get_size(y_vector, error);

    if (y_vector_size != x_matrix_rows)
    {
        sprintf(error_message, "Size mismatch: y_vector size (%u) does not match x_matrix rows (%u)", y_vector_size, x_matrix_rows);
        rikai_set_error(error, error_message, RIKAI_SIZE_MISMATCH);
        goto catch_error;
    }

    X_matrix_rows = x_matrix_rows;
    X_matrix_columns = x_matrix_columns + 1; // Adding bias term

    X_matrix = rikai_matrix_create_new(X_matrix_rows, X_matrix_columns, error);
    if (X_matrix == NULL)
        goto catch_error;

    // Fill first column with ones for bias term
    rikai_matrix_fill(X_matrix, 0, 0, X_matrix_rows - 1, 0, 1.0, NULL);

    // Copy the input matrix to the rest of the columns
    rikai_matrix_copy(X_matrix, 0, 1, x_matrix, 0, 0, x_matrix_rows - 1, x_matrix_columns - 1, NULL);

    XT_matrix = rikai_matrix_transpose_create_new(X_matrix, error);
    if (XT_matrix == NULL)
        goto catch_error;

    Y_HAT_vector = rikai_vector_create_new(X_matrix_rows, error);
    if (Y_HAT_vector == NULL)
        goto catch_error;

    E_vector = rikai_vector_create_new(X_matrix_rows, error);
    if (E_vector == NULL)
        goto catch_error;

    ET_vector = rikai_vector_create_new(X_matrix_rows, error);
    if (ET_vector == NULL)
        goto catch_error;

    model_vector_size = X_matrix_columns;
    model_vector = rikai_vector_create_new(model_vector_size, error);
    if (model_vector == NULL)
        goto catch_error;

    // Initialize model vector with random values
    rikai_vector_fill_random(model_vector, error);

    XTE_vector = rikai_vector_create_new(model_vector_size, error);
    if (XTE_vector == NULL)
        goto catch_error;

    // Training loop
    for (i = 0; i < number_of_iterations; i++)
    {
        // Calculate Y_HAT = X * model
        Y_HAT_vector = rikai_multiply_matrix_with_vector(X_matrix, model_vector, Y_HAT_vector, error);

        // Calculate E = Y - Y_HAT
        E_vector = rikai_substract_vector(Y_HAT_vector, y_vector, E_vector, error);

        // Calculate ET = E^T
        ET_vector = rikai_vector_transpose_create_new(E_vector, ET_vector, error);

        // error_value = ET * E
        error_value = rikai_multiple_vector_get_scalar(ET_vector, E_vector, error);

        // error_value  = error_value / 2 * X_matrix_rows;
        error_value = error_value / (2.0 * X_matrix_rows);

        if (on_iteration_complete)
        {
            if (!on_iteration_complete(i, error_value, model_vector))
            {
                break; // Stop training if the callback returns false
            }
        }

        // Calculate XTE = X^T * E
        XTE_vector = rikai_multiply_matrix_with_vector(XT_matrix, E_vector, XTE_vector, error);
        if (XTE_vector == NULL)
            goto catch_error;

        // Calculate XTE = XTE * ((learning_rate * (1 / X_matrix_rows));
        XTE_vector = rikai_multiply_vector_with_scalar(XTE_vector, (1.0 / X_matrix_rows) * learning_rate, XTE_vector, error);

        // Update model_vector = model_vector - XTE
        model_vector = rikai_substract_vector(model_vector, XTE_vector, model_vector, error);
    }

    goto prepare_for_exit;

catch_error:
    rikai_vector_destroy(model_vector, NULL);

prepare_for_exit:
    rikai_matrix_destroy(X_matrix, NULL);
    rikai_matrix_destroy(XT_matrix, NULL);

    rikai_vector_destroy(Y_HAT_vector, NULL);
    rikai_vector_destroy(E_vector, NULL);
    rikai_vector_destroy(ET_vector, NULL);
    rikai_vector_destroy(XTE_vector, NULL);

    return model_vector;
}

// --------------------------------------------------------------------------------------

uint8_t callback_on_iteration_complete(uint64_t iteration, double error_value, rikai_vector *model_vector)
{
    // This is a placeholder for the callback function that can be used to monitor training progress.
    // It can be modified to log the progress or perform other actions.
    if (iteration % 100 == 0)
        printf("Iteration: %llu, Error Value: %f\n", iteration, error_value);

    return 1; // Return 1 to continue training, return 0 to stop training
}

// Main function to demonstrate the usage of the linear regression fitting function

int main(int c, char *v[])
{
    if (c < 4)
    {
        fprintf(stderr, "Usage: %s <filename> <number_of_iterations> <learning_rate>\n", v[0]);
        return 1;
    }

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
    rikai_vector *model = NULL;

    uint64_t number_of_iterations, i;
    uint32_t dataset_rows, dataset_cols;
    uint32_t input_rows, input_cols;
    uint32_t model_size;
    double mean_squared_error;
    double learning_rate;

    filename = v[1];
    number_of_iterations = atol(v[2]);
    learning_rate = atof(v[3]);

    // Load the dataset
    dataset = rikai_matrix_read_csv(filename, 0, error);
    rikai_on_error_throw(error, err);

    rikai_matrix_get_dimensions(dataset, &dataset_rows, &dataset_cols, error);

    // Split the dataset into input and output matrices
    input_matrix = rikai_matrix_create_new(dataset_rows, dataset_cols - 1, error);
    rikai_matrix_copy(input_matrix, 0, 0, dataset, 0, 0, dataset_rows - 1, 0, error);

    y = rikai_matrix_get_vector(dataset, 0, dataset_cols - 1, dataset_rows - 1, dataset_cols - 1, error);
    rikai_on_error_throw(error, err);

    rikai_matrix_destroy(dataset, error);

    model = rikai_linear_regression_fit(input_matrix, y, number_of_iterations, learning_rate, callback_on_iteration_complete, error);
    rikai_on_error_throw(error, err);

    rikai_vector_write_csv(model, "model.csv", error);
    goto exit_program;

err:
    rikai_get_error(error_message, 4096, error);
    printf("Error: %s\n", error_message);

exit_program:

    rikai_matrix_destroy(dataset, error);
    rikai_matrix_destroy(input_matrix, error);
    rikai_vector_destroy(y, error);
    rikai_vector_destroy(model, error);
    rikai_error_destroy(error);

    return 0;
}
