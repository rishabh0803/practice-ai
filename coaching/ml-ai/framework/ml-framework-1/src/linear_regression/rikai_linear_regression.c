#include <rikai.h>
#include <private_rikai.h>

rikai_vector *rikai_linear_regression_batch_fit(const rikai_matrix *x_matrix, const rikai_vector *y_vector, uint64_t number_of_iterations, double learning_rate, uint8_t(*on_iteration_complete)(uint64_t, double, rikai_vector *), rikai_err *error)
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
		E_vector = rikai_subtract_vector(Y_HAT_vector, y_vector, E_vector, error);

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
		model_vector = rikai_subtract_vector(model_vector, XTE_vector, model_vector, error);
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

rikai_vector *rikai_linear_regression_minibatch_fit(
	rikai_data_provider *data_provider,
	uint64_t number_of_iterations,
	double learning_rate,
	uint8_t(*on_iteration_complete)(uint64_t, double, rikai_vector *),
	rikai_err *error)
{
	rikai_matrix *X_matrix = NULL;
	rikai_matrix *XT_matrix = NULL;

	rikai_vector *Y_HAT_vector = NULL;
	rikai_vector *E_vector = NULL;
	rikai_vector *ET_vector = NULL;
	rikai_vector *XTE_vector = NULL;

	rikai_vector *model_vector = NULL;

	// char error_message[4096];
	double error_value = 0.0;
	uint64_t i;

	rikai_matrix *dataset = NULL;
	uint32_t dataset_rows, dataset_columns;
	uint32_t rows_provided = 0;

	rikai_vector *y_vector = NULL;
	// uint32_t y_vector_size;

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