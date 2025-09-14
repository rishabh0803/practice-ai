#include <rikai.h>
#include <private_rikai.h>
#include <stdio.h>
#include <stdlib.h>
#include "data_provider_test.c"

rikai_vector *rikai_linear_regression_fit(
    rikai_data_provider *data_provider,
    uint64_t number_of_iterations,
    double learning_rate,
    uint8_t (*on_iteration_complete)(uint64_t, double, rikai_vector *),
    rikai_err *error)
{
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

    rikai_matrix *dataset = NULL;
    uint32_t dataset_rows, dataset_columns;
    uint32_t rows_provided = 0;

    rikai_vector *y_vector = NULL;
    uint32_t y_vector_size;

    uint32_t X_matrix_rows, X_matrix_columns;
    uint32_t model_vector_size;

    if (error)
        rikai_clear_error(error);

    if (number_of_iterations == 0)
    {
        rikai_set_error(error, "Cannot train with zero iterations", RIKAI_INVALID_NUMBER_OF_ITERATIONS);
        goto catch_error;
    }

    dataset_columns = rikai_data_provider_get_column_count(data_provider, error);
    dataset_rows = 90;

    dataset = rikai_matrix_create_new(dataset_rows, dataset_columns, error);
    if (dataset == NULL)
        goto catch_error;

    y_vector = rikai_vector_create_new(dataset_rows, error);
    if (y_vector == NULL)
        goto catch_error;

    X_matrix_rows = dataset_rows;
    X_matrix_columns = dataset_columns;

    X_matrix = rikai_matrix_create_new(X_matrix_rows, X_matrix_columns, error);
    if (X_matrix == NULL)
        goto catch_error;

    // XT_matrix = rikai_matrix_transpose_create_new(X_matrix, error);
    // if (XT_matrix == NULL)
    //     goto catch_error;

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

    // Fill first column with ones for bias term
    rikai_matrix_fill(X_matrix, 0, 0, X_matrix_rows - 1, 0, 1.0, NULL);

    // Training loop
    for (i = 0; i < number_of_iterations; i++)
    {
        while (1)
        {
            data_provider->get_next_batch(data_provider, dataset, &rows_provided, error);
            if (rikai_has_error(error))
                goto catch_error;

            if (rows_provided == 0 || rows_provided < dataset_rows)
                break;

            // Copy the input matrix to the rest of the columns
            rikai_matrix_copy(X_matrix, 0, 1, dataset, 0, 0, dataset_rows - 1, dataset_columns - 1, NULL);
 
            XT_matrix = rikai_matrix_transpose_create_new(X_matrix, error);
            if (XT_matrix == NULL)
                goto catch_error;

            // Populate y_vector
            y_vector = rikai_matrix_get_vector(dataset, 0, dataset_columns - 1, dataset_rows - 1, dataset_columns - 1, error);
            // rikai_vector_print(stdout, y_vector, NULL);

            // Calculate Y_HAT = X * model
            Y_HAT_vector = rikai_multiply_matrix_with_vector(X_matrix, model_vector, Y_HAT_vector, error);

            // Calculate E = Y - Y_HAT
            E_vector = rikai_subtract_vector(Y_HAT_vector, y_vector, E_vector, error);

            // Calculate ET = E^T
            ET_vector = rikai_vector_transpose_create_new(E_vector, ET_vector, error);

            // error_value = ET * E
            error_value = rikai_multiple_vector_get_scalar(ET_vector, E_vector, error);

            // error_value  = error_value / 2 * X_matrix_rows;
            error_value = error_value / (2.0 * X_matrix_rows);

            // Calculate XTE = X^T * E
            XTE_vector = rikai_multiply_matrix_with_vector(XT_matrix, E_vector, XTE_vector, error);
            if (XTE_vector == NULL)
                goto catch_error;

            // Calculate XTE = XTE * ((learning_rate * (1 / X_matrix_rows));
            XTE_vector = rikai_multiply_vector_with_scalar(XTE_vector, (1.0 / X_matrix_rows) * learning_rate, XTE_vector, error);

            // Update model_vector = model_vector - XTE
            model_vector = rikai_subtract_vector(model_vector, XTE_vector, model_vector, error);

            if (on_iteration_complete)
            {
                if (!on_iteration_complete(i, error_value, model_vector))
                {
                    break; // Stop training if the callback returns false
                }
            }
        }
    }

    goto prepare_for_exit;

catch_error:
    rikai_vector_destroy(model_vector, NULL);

prepare_for_exit:
    rikai_matrix_destroy(dataset, NULL);
    rikai_matrix_destroy(X_matrix, NULL);
    rikai_matrix_destroy(XT_matrix, NULL);

    rikai_vector_destroy(Y_HAT_vector, NULL);
    rikai_vector_destroy(E_vector, NULL);
    rikai_vector_destroy(ET_vector, NULL);
    rikai_vector_destroy(XTE_vector, NULL);

    return model_vector;
}

// --------------------------------------------------------------------------------------
FILE *fit_plot = NULL;
FILE *error_plot = NULL;
char *filename = NULL;

uint64_t iterations[1000000];
double errors[1000000];
uint64_t error_count = 0;

uint8_t callback_on_iteration_complete(uint64_t iteration, double error_value, rikai_vector *model_vector)
{
    // This is a placeholder for the callback function that can be used to monitor training progress.
    // It can be modified to log the progress or perform other actions.

    if (iteration % 1000 == 0)
    {
        printf("Iteration: %llu, Error Value: %12.12f\n", iteration, error_value);

        if (error_count < 1000000)
        {
            iterations[error_count] = (iteration / 1000) + error_count;
            errors[error_count] = error_value;
            error_count++;
        }

        fprintf(fit_plot, "plot '%s' using 1:2 with points pt 7 title 'Data', %f*x+%f title 'Line' lw 2\n",
                filename,
                rikai_vector_get(model_vector, 1, NULL),
                rikai_vector_get(model_vector, 0, NULL));
        fflush(fit_plot);

        fprintf(error_plot, "plot '-' with linespoints\n");
        for (uint64_t i = 0; i < error_count; i++)
        {
            fprintf(error_plot, "%llu %f\n", iterations[i], errors[i]);
        }
        fprintf(error_plot, "e\n");
        fflush(error_plot);
    }

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

    fit_plot = popen("gnuplot -persist", "w");
    error_plot = popen("gnuplot -persist", "w");
    if (fit_plot == NULL || error_plot == NULL)
    {
        fprintf(stderr, "Error opening pipe to gnuplot\n");
        return 1;
    }

    fprintf(fit_plot, "set term wxt size 1248,650 position 1248,0\n");
    fprintf(fit_plot, "set title 'Linear Regression Fit'\n");
    fprintf(fit_plot, "set xlabel 'X'\n");
    fprintf(fit_plot, "set ylabel 'Y'\n");
    fprintf(fit_plot, "set datafile separator \",\"\n");

    fprintf(error_plot, "set term wxt size 1248,650 position 1248,800\n");
    fprintf(error_plot, "set title 'Training Error'\n");
    fprintf(error_plot, "set xlabel 'Iteration / 100'\n");
    fprintf(error_plot, "set ylabel 'Error'\n");
    fprintf(error_plot, "set xrange [0:%llu]\n", number_of_iterations / 100);
    fprintf(error_plot, "set yrange [:100000000]\n");
    fprintf(error_plot, "set logscale y\n");
    fprintf(error_plot, "set grid\n");
    fprintf(error_plot, "set style data linespoints\n");

    // Setup Data
    rikai_data_provider *data_provider = NULL;
    data_provider = rikai_data_provider_create_new(filename, 0, kalu, error);
    if (rikai_has_error(error))
    {
        printf("Error \n");
        return 0;
    }

    // Start Training
    model = rikai_linear_regression_fit(data_provider, number_of_iterations, learning_rate, callback_on_iteration_complete, error);
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

    if (fit_plot)
    {
        fprintf(fit_plot, "exit\n");
        fflush(fit_plot);
        pclose(fit_plot);
        fit_plot = NULL;
    }

    if (error_plot)
    {
        fprintf(error_plot, "exit\n");
        fflush(error_plot);
        pclose(error_plot);
        error_plot = NULL;
    }
    return 0;
}
