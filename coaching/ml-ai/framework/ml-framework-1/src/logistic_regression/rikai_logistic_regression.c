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
    // Input matrix with bias term
    rikai_matrix *X_matrix = NULL;

    // Model parameters
    rikai_vector *theta_vector = NULL;

    // X * theta
    rikai_vector *X_theta_vector = NULL;

    // h(0) = sigmoid(X * theta) = sigmoid(X_theta)
    rikai_vector *h_vector = NULL;

    // difference between predicted and actual
    rikai_vector *diff_vector = NULL;

    // 1 - y_vector
    rikai_vector *one_minus_Y = NULL;

    // log(h(X_theta))
    rikai_vector *log_H = NULL;

    // 1 - h(X_theta)
    rikai_vector *one_minus_H = NULL;

    // log(1 - h(X_theta))
    rikai_vector *log_one_minus_H = NULL;

    // (y_vector * log(h(X_theta)) + ((1 - y_vector) * log(1 - h(X_theta)))
    // (y_vector * log_H) + (one_minus_Y * log_one_minus_H)
    rikai_vector *E_vector = NULL;
    rikai_vector *ET_vector = NULL;

    rikai_matrix *XT_matrix = NULL;
    rikai_vector *XTDiff_vector = NULL;

    char error_message[4096];
    double error_value = 0.0;
    uint64_t i;

    uint32_t x_matrix_rows, x_matrix_columns;
    uint32_t y_vector_size;

    uint32_t X_matrix_rows, X_matrix_columns;
    uint32_t theta_vector_size;

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

    // 1. Create X_matrix with bias term and copy x_matrix data
    X_matrix = rikai_matrix_create_new(X_matrix_rows, X_matrix_columns, error);
    if (X_matrix == NULL)
        goto catch_error;

    // Fill first column with ones for bias term
    rikai_matrix_fill(X_matrix, 0, 0, X_matrix_rows - 1, 0, 1.0, NULL);
    // Copy the input matrix to the rest of the columns
    rikai_matrix_copy(X_matrix, 0, 1, x_matrix, 0, 0, x_matrix_rows - 1, x_matrix_columns - 1, NULL);

    // 2. Initialize theta_vector
    theta_vector_size = X_matrix_columns;
    theta_vector = rikai_vector_create_new(theta_vector_size, error);
    if (theta_vector == NULL)
        goto catch_error;

    // rikai_vector_fill_random(theta_vector, NULL);
    for (i = 0; i < theta_vector_size; i++)
    {
        rikai_vector_set(theta_vector, i, 1000.0, NULL);
    }

    // 3. Prepare other vectors and matrices
    X_theta_vector = rikai_vector_create_new(X_matrix_rows, error);
    if (X_theta_vector == NULL)
        goto catch_error;

    h_vector = rikai_vector_create_new(X_matrix_rows, error);
    if (h_vector == NULL)
        goto catch_error;

    log_H = rikai_vector_create_new(X_matrix_rows, error);
    if (log_H == NULL)
        goto catch_error;

    one_minus_Y = rikai_vector_create_new(y_vector_size, error);
    if (one_minus_Y == NULL)
        goto catch_error;
    one_minus_Y = rikai_subtract_vector_from_scalar(1, y_vector, one_minus_Y, error);

    one_minus_H = rikai_vector_create_new(X_matrix_rows, error);
    if (one_minus_H == NULL)
        goto catch_error;

    log_one_minus_H = rikai_vector_create_new(X_matrix_rows, error);
    if (log_one_minus_H == NULL)
        goto catch_error;

    E_vector = rikai_vector_create_new(X_matrix_rows, error);
    if (E_vector == NULL)
        goto catch_error;

    ET_vector = rikai_vector_create_new(X_matrix_rows, error);
    if (ET_vector == NULL)
        goto catch_error;

    XT_matrix = rikai_matrix_transpose_create_new(X_matrix, error);
    if (XT_matrix == NULL)
        goto catch_error;

    diff_vector = rikai_vector_create_new(X_matrix_rows, error);
    if (diff_vector == NULL)
        goto catch_error;

    XTDiff_vector = rikai_vector_create_new(theta_vector_size, error);
    if (XTDiff_vector == NULL)
        goto catch_error;

    for (i = 0; i < number_of_iterations; i++)
    {
        // Calculate x_theta = X * theta
        X_theta_vector = rikai_multiply_matrix_with_vector(X_matrix, theta_vector, X_theta_vector, error);

        // Apply sigmoid function to x_theta
        h_vector = rikai_sigmoid_vector(X_theta_vector, h_vector, error);

        // Calculate log(h)
        log_H = rikai_log_vector(h_vector, log_H, error);

        // Calculate 1 - h
        one_minus_H = rikai_subtract_vector_from_scalar(1, h_vector, one_minus_H, error);

        // Calculate log(1 - h)
        log_one_minus_H = rikai_log_vector(one_minus_H, log_one_minus_H, error);

        // calculate E = (y_vector * log(h)) + ((1 - y_vector) * log(1 - h))
        // E = (y_vector * log_H) + (one_minus_Y * log_one_minus_H)
        // using log_H to store (y_vector * log_H)
        log_H = rikai_hadamard_product_vector(y_vector, log_H, log_H, error);
        // using log_one_minus_H to store (one_minus_Y * log_one_minus_H)
        log_one_minus_H = rikai_hadamard_product_vector(one_minus_Y, log_one_minus_H, log_one_minus_H, error);
        // using E_vector to store the final result
        E_vector = rikai_sum_vectors(log_H, log_one_minus_H, E_vector, error);

        // Calculate ET = E^T
        ET_vector = rikai_vector_transpose_create_new(E_vector, ET_vector, error);

        // error_value = ET * E
        error_value = rikai_multiple_vector_get_scalar(ET_vector, E_vector, error);

        // error_value = -1 / m * error_value
        error_value = error_value * -1.0 / (double)X_matrix_rows;

        // Callback on iteration complete
        if (on_iteration_complete)
        {
            if (!on_iteration_complete(i, error_value, theta_vector))
            {
                break;
            }
        }

        // Calculate difference vector = predicted_Y - y_vector
        diff_vector = rikai_subtract_vector(h_vector, y_vector, diff_vector, error);

        // Calculate XTdiff_vector = X^T * diff_vector
        XTDiff_vector = rikai_multiply_matrix_with_vector(XT_matrix, diff_vector, XTDiff_vector, error);
        if (XTDiff_vector == NULL)
            goto catch_error;

        // Calculate XTDiff_vector = XTDiff_vector * ((learning_rate * (1 / X_matrix_rows));
        XTDiff_vector = rikai_multiply_vector_with_scalar(XTDiff_vector, (1.0 / X_matrix_rows) * learning_rate, XTDiff_vector, error);

        // Update model_vector = model_vector - XTDiff_vector
        theta_vector = rikai_subtract_vector(theta_vector, XTDiff_vector, theta_vector, error);
    }

    goto prepare_for_exit;

catch_error:
    rikai_vector_destroy(theta_vector, NULL);

prepare_for_exit:
    rikai_matrix_destroy(X_matrix, NULL);
    rikai_matrix_destroy(XT_matrix, NULL);

    rikai_vector_destroy(X_theta_vector, NULL);
    rikai_vector_destroy(E_vector, NULL);
    rikai_vector_destroy(ET_vector, NULL);
    rikai_vector_destroy(XTDiff_vector, NULL);
    rikai_vector_destroy(h_vector, NULL);
    rikai_vector_destroy(diff_vector, NULL);
    rikai_vector_destroy(one_minus_Y, NULL);
    rikai_vector_destroy(log_H, NULL);
    rikai_vector_destroy(one_minus_H, NULL);
    rikai_vector_destroy(log_one_minus_H, NULL);

    return theta_vector;
}