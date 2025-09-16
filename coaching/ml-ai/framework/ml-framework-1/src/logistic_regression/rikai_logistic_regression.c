#include <rikai.h>
#include <private_rikai.h>

rikai_vector *rikai_logistic_regression_fit(
    const rikai_matrix *x_matrix,
    const rikai_vector *y_vector,
    uint64_t number_of_iterations,
    double learning_rate,
    uint8_t (*on_iteration_complete)(uint64_t, double, rikai_vector *),
    rikai_err *error)
{
    rikai_matrix *X_matrix = NULL;
    rikai_matrix *XT_matrix = NULL;
    rikai_vector *XTDiff_vector = NULL;

    rikai_vector *Y_HAT_vector = NULL;
    rikai_vector *predicted_Y = NULL;
    rikai_vector *log_predicted_Y = NULL;

    rikai_vector *one_minus_Y = NULL;
    rikai_vector *one_minus_predicted_Y = NULL;
    rikai_vector *log_one_minus_predicted_Y = NULL;

    rikai_vector *E_vector = NULL;
    rikai_vector *ET_vector = NULL;

    rikai_vector *diff_vector = NULL;

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

    // Calculate 1 - y_vector
    one_minus_Y = rikai_vector_create_new(y_vector_size, error);
    one_minus_Y = rikai_subtract_vector_from_scalar(1, y_vector, one_minus_Y, error);
    if (one_minus_Y == NULL)
        goto catch_error;

    predicted_Y = rikai_vector_create_new(X_matrix_rows, error);
    if (predicted_Y == NULL)
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

    log_predicted_Y = rikai_vector_create_new(X_matrix_rows, error);
    if (log_predicted_Y == NULL)

        goto catch_error;

    one_minus_predicted_Y = rikai_vector_create_new(X_matrix_rows, error);
    if (one_minus_predicted_Y == NULL)
        goto catch_error;

    log_one_minus_predicted_Y = rikai_vector_create_new(X_matrix_rows, error);
    if (log_one_minus_predicted_Y == NULL)
        goto catch_error;

    diff_vector = rikai_vector_create_new(X_matrix_rows, error);
    if (diff_vector == NULL)
        goto catch_error;

    model_vector_size = X_matrix_columns;
    model_vector = rikai_vector_create_new(model_vector_size, error);
    if (model_vector == NULL)
        goto catch_error;

    // Initialize model vector with zero
    for (i = 0; i < model_vector_size; i++)
    {
        rikai_vector_set(model_vector, i, 0.01, error);
    }

    XTDiff_vector = rikai_vector_create_new(model_vector_size, error);
    if (XTDiff_vector == NULL)
        goto catch_error;

    // Training loop
    for (i = 0; i < number_of_iterations; i++)
    {
        // Calculate Y_HAT_vector = X * model_vector
        Y_HAT_vector = rikai_multiply_matrix_with_vector(X_matrix, model_vector, Y_HAT_vector, error);

        // Apply sigmoid function to Y_HAT_vector
        predicted_Y = rikai_sigmoid_vector(Y_HAT_vector, predicted_Y, error);

        // Calculate difference vector = predicted_Y - y_vector
        diff_vector = rikai_subtract_vector(predicted_Y, y_vector, diff_vector, error);

        // Calculate log(predicted_Y)
        log_predicted_Y = rikai_log_vector(predicted_Y, log_predicted_Y, error);

        // Calculate 1 - predicted_Y
        one_minus_predicted_Y = rikai_subtract_vector_from_scalar(1, predicted_Y, one_minus_predicted_Y, error);

        // Calculate log(1 - predicted_Y)
        log_one_minus_predicted_Y = rikai_log_vector(one_minus_predicted_Y, log_one_minus_predicted_Y, error);

        // calculate E = (y_vector * log(predicted_Y)) + ((1 - y_vector) * log(1 - predicted_Y))
        log_predicted_Y = rikai_hadamard_product_vector(y_vector, log_predicted_Y, log_predicted_Y, error);
        log_one_minus_predicted_Y = rikai_hadamard_product_vector(one_minus_Y, log_one_minus_predicted_Y, log_one_minus_predicted_Y, error);
        E_vector = rikai_sum_vectors(log_predicted_Y, log_one_minus_predicted_Y, E_vector, error);

        // Calculate ET = E^T
        ET_vector = rikai_vector_transpose_create_new(E_vector, ET_vector, error);

        // error_value = ET * E
        error_value = rikai_multiple_vector_get_scalar(ET_vector, E_vector, error);

        // error_value = -1 / m * error_value
        error_value = -1.0 / X_matrix_rows * error_value;

        // Callback on iteration complete
        if (on_iteration_complete)
        {
            if (!on_iteration_complete(i, error_value, model_vector))
            {
                break; // Stop training if the callback returns false
            }
        }

        // Calculate XTE = X^T * E
        XTDiff_vector = rikai_multiply_matrix_with_vector(XT_matrix, diff_vector, XTDiff_vector, error);
        if (XTDiff_vector == NULL)
            goto catch_error;

        // Calculate XTE = XTE * ((learning_rate * (1 / X_matrix_rows));
        XTDiff_vector = rikai_multiply_vector_with_scalar(XTDiff_vector, (1.0 / X_matrix_rows) * learning_rate, XTDiff_vector, error);

        // Update model_vector = model_vector - XTE
        model_vector = rikai_subtract_vector(model_vector, XTDiff_vector, model_vector, error);
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
    rikai_vector_destroy(XTDiff_vector, NULL);

    return model_vector;
}