#include <rikai.h>
#include <stdio.h>
#include <stdlib.h>

// ----------------------------- Global Variables ------------------------------
FILE *error_plot = NULL;
char *filename = NULL;

uint64_t iterations[100000];
double errors[100000];
uint64_t error_count = 0;

// ----------------------------- Callback Function ------------------------------
uint8_t callback_on_iteration_complete(uint64_t iteration, double error_value, rikai_vector *model_vector)
{
    if (iteration % 1000 == 0)
    {
        printf("Iteration: %llu, Error Value: %12.12f\n", iteration, error_value);

        if (error_count < 100000)
        {
            iterations[error_count] = iteration;
            errors[error_count] = error_value;
            error_count++;
        }

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

// ----------------------------- Main Function ----------------------------------
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

    error_plot = popen("gnuplot -persist", "w");
    if (error_plot == NULL)
    {
        fprintf(stderr, "Error opening pipe to gnuplot\n");
        return 1;
    }

    fprintf(error_plot, "set term wxt size 1248,650 position 1248,1200\n");
    fprintf(error_plot, "set title 'Training Error'\n");
    fprintf(error_plot, "set xlabel 'Iteration'\n");
    fprintf(error_plot, "set ylabel 'Error'\n");
    fprintf(error_plot, "set grid\n");
    fprintf(error_plot, "set style data linespoints\n");

    // Load the dataset
    dataset = rikai_matrix_read_csv(filename, 1, error);
    rikai_on_error_throw(error, err);

    rikai_matrix_get_dimensions(dataset, &dataset_rows, &dataset_cols, error);

    // Split the dataset into input and output matrices
    input_matrix = rikai_matrix_create_new(dataset_rows, dataset_cols - 1, error);
    rikai_matrix_copy(input_matrix, 0, 0, dataset, 0, 0, dataset_rows - 1, dataset_cols - 2, error);

    y = rikai_matrix_get_vector(dataset, 0, dataset_cols - 1, dataset_rows - 1, dataset_cols - 1, error);
    rikai_on_error_throw(error, err);

    rikai_matrix_destroy(dataset, error);

    model = rikai_logistic_regression_fit(input_matrix, y, number_of_iterations, learning_rate, callback_on_iteration_complete, error);
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

    if (error_plot)
    {
        fprintf(error_plot, "exit\n");
        fflush(error_plot);
        pclose(error_plot);
        error_plot = NULL;
    }
    return 0;
}
