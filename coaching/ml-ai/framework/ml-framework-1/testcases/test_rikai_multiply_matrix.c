#include <stdio.h>
#include <stdlib.h>
#include <rikai.h>
#include <math.h>

#define TEST_EPSILON 1e-9
#define MAX_ERROR_MSG_SIZE 1024

// Test result counters
static int tests_passed = 0;
static int tests_failed = 0;
static int test_number = 0;

// Helper function to compare double values
int double_equals(double a, double b)
{
    return fabs(a - b) < TEST_EPSILON;
}

// Helper function to print test results
void print_test_result(const char *test_name, int passed)
{
    test_number++;
    if (passed)
    {
        tests_passed++;
        printf("Test %d: %s : Status(Passed)\n", test_number, test_name);
    }
    else
    {
        tests_failed++;
        printf("Test %d: %s : Status(Failed)\n", test_number, test_name);
    }

    printf("---------------------------------------------------------------------------------\n");
}

// Helper function to print error details if there's an error
void print_error_if_exists(rikai_err *error)
{
    if (rikai_has_error(error))
    {
        char error_message[MAX_ERROR_MSG_SIZE];
        rikai_get_error(error_message, MAX_ERROR_MSG_SIZE, error);
        printf("    ERROR: %s (Error Code: %u)\n", error_message, rikai_get_error_code(error));
    }
}

// Helper function to print a matrix
void print_matrix(const char *label, const rikai_matrix *matrix, rikai_err *error)
{
    if (matrix == NULL)
    {
        printf("%s: NULL\n", label);
        return;
    }

    uint32_t rows, cols;
    rikai_matrix_get_dimensions(matrix, &rows, &cols, error);
    printf("%s: [%ux%u]\n", label, rows, cols);

    for (uint32_t i = 0; i < rows; i++)
    {
        printf("  [");
        for (uint32_t j = 0; j < cols; j++)
        {
            double value = rikai_matrix_get(matrix, i, j, error);
            printf("%8.3f", value);
            if (j < cols - 1)
                printf(", ");
        }
        printf("]\n");
    }
}

// Helper function to print expected vs actual results
void print_expected_vs_actual(double expected_values[], const rikai_matrix *actual_matrix, uint32_t rows, uint32_t cols, rikai_err *error)
{
    printf("Expected Output:\n  [%ux%u]\n", rows, cols);
    for (uint32_t i = 0; i < rows; i++)
    {
        printf("  [");
        for (uint32_t j = 0; j < cols; j++)
        {
            printf("%8.3f", expected_values[i * cols + j]);
            if (j < cols - 1)
                printf(", ");
        }
        printf("]\n");
    }

    printf("\nActual Output:\n");
    if (actual_matrix != NULL)
    {
        print_matrix("", actual_matrix, error);
    }
    else
    {
        printf("  NULL (Error occurred)\n");
    }
}

// Helper function to verify matrix values
int verify_matrix_values(const rikai_matrix *matrix, double expected_values[], uint32_t rows, uint32_t cols, rikai_err *error)
{
    for (uint32_t i = 0; i < rows; i++)
    {
        for (uint32_t j = 0; j < cols; j++)
        {
            double actual = rikai_matrix_get(matrix, i, j, error);
            double expected = expected_values[i * cols + j];
            if (!double_equals(actual, expected))
            {
                printf("    Mismatch at [%u,%u]: expected %.6f, got %.6f\n", i, j, expected, actual);
                return 0;
            }
        }
    }
    return 1;
}

// Helper function to run standard matrix multiplication test
void run_matrix_test(const char *test_name,
                     const rikai_matrix *matrix_a, const rikai_matrix *matrix_b,
                     rikai_matrix *container, double expected_values[],
                     uint32_t expected_rows, uint32_t expected_cols, rikai_err *error)
{
    rikai_matrix *result = rikai_multiply_matrix(matrix_a, matrix_b, container, error);

    int passed = (result != NULL) &&
                 !rikai_has_error(error) &&
                 verify_matrix_values(result, expected_values, expected_rows, expected_cols, error);

    // Special case for container test
    if (container != NULL && passed)
    {
        passed = (result == container);
    }

    print_test_result(test_name, passed);

    printf("Inputs:\n");
    print_matrix("Matrix A", matrix_a, error);
    print_matrix("Matrix B", matrix_b, error);
    printf("Container: %s\n\n", container ? "Provided" : "NULL");

    print_expected_vs_actual(expected_values, result, expected_rows, expected_cols, error);

    if (!passed)
    {
        print_error_if_exists(error);
    }

    printf("------------------------------------\n\n");

    // Only destroy result if it's not the same as container
    if (result != container)
    {
        rikai_matrix_destroy(result, error);
    }
}

// Test 1: Basic 2x2 matrix multiplication
void test_basic_2x2_multiplication()
{
    rikai_err *error = rikai_error_create_new();
    rikai_matrix *matrix_a = rikai_matrix_create_new(2, 2, error);
    rikai_matrix *matrix_b = rikai_matrix_create_new(2, 2, error);

    // Set up matrix A = [[1, 2], [3, 4]]
    rikai_matrix_set(matrix_a, 0, 0, 1.0, error);
    rikai_matrix_set(matrix_a, 0, 1, 2.0, error);
    rikai_matrix_set(matrix_a, 1, 0, 3.0, error);
    rikai_matrix_set(matrix_a, 1, 1, 4.0, error);

    // Set up matrix B = [[5, 6], [7, 8]]
    rikai_matrix_set(matrix_b, 0, 0, 5.0, error);
    rikai_matrix_set(matrix_b, 0, 1, 6.0, error);
    rikai_matrix_set(matrix_b, 1, 0, 7.0, error);
    rikai_matrix_set(matrix_b, 1, 1, 8.0, error);

    // Expected result: [[19, 22], [43, 50]]
    double expected[] = {19.0, 22.0, 43.0, 50.0};

    rikai_matrix *result = rikai_multiply_matrix(matrix_a, matrix_b, NULL, error);

    int passed = (result != NULL) &&
                 !rikai_has_error(error) &&
                 verify_matrix_values(result, expected, 2, 2, error);

    print_test_result("Basic 2x2 matrix multiplication", passed);

    printf("Inputs:\n");
    print_matrix("Matrix A", matrix_a, error);
    print_matrix("Matrix B", matrix_b, error);
    printf("Container: NULL\n\n");

    print_expected_vs_actual(expected, result, 2, 2, error);

    if (!passed)
    {
        print_error_if_exists(error);
    }

    printf("------------------------------------\n\n");

    rikai_matrix_destroy(matrix_a, error);
    rikai_matrix_destroy(matrix_b, error);
    rikai_matrix_destroy(result, error);
    rikai_error_destroy(error);
}

// Test 2: Non-square matrix multiplication (2x3 × 3x2)
void test_non_square_multiplication()
{
    rikai_err *error = rikai_error_create_new();
    rikai_matrix *matrix_a = rikai_matrix_create_new(2, 3, error);
    rikai_matrix *matrix_b = rikai_matrix_create_new(3, 2, error);

    // Set up matrix A = [[1, 2, 3], [4, 5, 6]]
    rikai_matrix_set(matrix_a, 0, 0, 1.0, error);
    rikai_matrix_set(matrix_a, 0, 1, 2.0, error);
    rikai_matrix_set(matrix_a, 0, 2, 3.0, error);
    rikai_matrix_set(matrix_a, 1, 0, 4.0, error);
    rikai_matrix_set(matrix_a, 1, 1, 5.0, error);
    rikai_matrix_set(matrix_a, 1, 2, 6.0, error);

    // Set up matrix B = [[7, 8], [9, 10], [11, 12]]
    rikai_matrix_set(matrix_b, 0, 0, 7.0, error);
    rikai_matrix_set(matrix_b, 0, 1, 8.0, error);
    rikai_matrix_set(matrix_b, 1, 0, 9.0, error);
    rikai_matrix_set(matrix_b, 1, 1, 10.0, error);
    rikai_matrix_set(matrix_b, 2, 0, 11.0, error);
    rikai_matrix_set(matrix_b, 2, 1, 12.0, error);

    // Expected result: [[58, 64], [139, 154]]
    double expected[] = {58.0, 64.0, 139.0, 154.0};

    rikai_matrix *result = rikai_multiply_matrix(matrix_a, matrix_b, NULL, error);

    int passed = (result != NULL) &&
                 !rikai_has_error(error) &&
                 verify_matrix_values(result, expected, 2, 2, error);

    print_test_result("Non-square matrix multiplication (2x3 x 3x2)", passed);

    printf("Inputs:\n");
    print_matrix("Matrix A", matrix_a, error);
    print_matrix("Matrix B", matrix_b, error);
    printf("Container: NULL\n\n");

    print_expected_vs_actual(expected, result, 2, 2, error);

    if (!passed)
    {
        print_error_if_exists(error);
    }

    printf("------------------------------------\n\n");

    rikai_matrix_destroy(matrix_a, error);
    rikai_matrix_destroy(matrix_b, error);
    rikai_matrix_destroy(result, error);
    rikai_error_destroy(error);
}

// Test 3: Identity matrix multiplication
void test_identity_matrix_multiplication()
{
    rikai_err *error = rikai_error_create_new();
    rikai_matrix *matrix_a = rikai_matrix_create_new(3, 3, error);
    rikai_matrix *identity = rikai_matrix_create_new(3, 3, error);

    // Set up matrix A
    rikai_matrix_set(matrix_a, 0, 0, 1.0, error);
    rikai_matrix_set(matrix_a, 0, 1, 2.0, error);
    rikai_matrix_set(matrix_a, 0, 2, 3.0, error);
    rikai_matrix_set(matrix_a, 1, 0, 4.0, error);
    rikai_matrix_set(matrix_a, 1, 1, 5.0, error);
    rikai_matrix_set(matrix_a, 1, 2, 6.0, error);
    rikai_matrix_set(matrix_a, 2, 0, 7.0, error);
    rikai_matrix_set(matrix_a, 2, 1, 8.0, error);
    rikai_matrix_set(matrix_a, 2, 2, 9.0, error);

    // Set up identity matrix
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            rikai_matrix_set(identity, i, j, (i == j) ? 1.0 : 0.0, error);
        }
    }

    // Expected result should be the same as matrix A
    double expected[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};

    rikai_matrix *result = rikai_multiply_matrix(matrix_a, identity, NULL, error);

    int passed = (result != NULL) &&
                 !rikai_has_error(error) &&
                 verify_matrix_values(result, expected, 3, 3, error);

    if (!passed)
    {
        print_error_if_exists(error);
    }

    print_test_result("Identity matrix multiplication", passed);

    rikai_matrix_destroy(matrix_a, error);
    rikai_matrix_destroy(identity, error);
    rikai_matrix_destroy(result, error);
    rikai_error_destroy(error);
}

// Test 4: Zero matrix multiplication
void test_zero_matrix_multiplication()
{
    rikai_err *error = rikai_error_create_new();
    rikai_matrix *matrix_a = rikai_matrix_create_new(2, 3, error);
    rikai_matrix *zero_matrix = rikai_matrix_create_new(3, 2, error);

    // Set up matrix A with non-zero values
    rikai_matrix_set(matrix_a, 0, 0, 5.0, error);
    rikai_matrix_set(matrix_a, 0, 1, 10.0, error);
    rikai_matrix_set(matrix_a, 0, 2, 15.0, error);
    rikai_matrix_set(matrix_a, 1, 0, 20.0, error);
    rikai_matrix_set(matrix_a, 1, 1, 25.0, error);
    rikai_matrix_set(matrix_a, 1, 2, 30.0, error);

    // Zero matrix is already initialized to zeros

    // Expected result should be all zeros
    double expected[] = {0.0, 0.0, 0.0, 0.0};

    rikai_matrix *result = rikai_multiply_matrix(matrix_a, zero_matrix, NULL, error);

    int passed = (result != NULL) &&
                 !rikai_has_error(error) &&
                 verify_matrix_values(result, expected, 2, 2, error);

    if (!passed)
    {
        print_error_if_exists(error);
    }

    print_test_result("Zero matrix multiplication", passed);

    rikai_matrix_destroy(matrix_a, error);
    rikai_matrix_destroy(zero_matrix, error);
    rikai_matrix_destroy(result, error);
    rikai_error_destroy(error);
}

// Test 5: 1x1 matrix multiplication
void test_1x1_matrix_multiplication()
{
    rikai_err *error = rikai_error_create_new();
    rikai_matrix *matrix_a = rikai_matrix_create_new(1, 1, error);
    rikai_matrix *matrix_b = rikai_matrix_create_new(1, 1, error);

    rikai_matrix_set(matrix_a, 0, 0, 7.0, error);
    rikai_matrix_set(matrix_b, 0, 0, 3.0, error);

    double expected[] = {21.0};

    rikai_matrix *result = rikai_multiply_matrix(matrix_a, matrix_b, NULL, error);

    int passed = (result != NULL) &&
                 !rikai_has_error(error) &&
                 verify_matrix_values(result, expected, 1, 1, error);

    if (!passed)
    {
        print_error_if_exists(error);
    }

    print_test_result("1x1 matrix multiplication", passed);

    rikai_matrix_destroy(matrix_a, error);
    rikai_matrix_destroy(matrix_b, error);
    rikai_matrix_destroy(result, error);
    rikai_error_destroy(error);
}

// Test 6: Single row × single column
void test_row_column_multiplication()
{
    rikai_err *error = rikai_error_create_new();
    rikai_matrix *row_matrix = rikai_matrix_create_new(1, 4, error);
    rikai_matrix *col_matrix = rikai_matrix_create_new(4, 1, error);

    // Row matrix: [1, 2, 3, 4]
    rikai_matrix_set(row_matrix, 0, 0, 1.0, error);
    rikai_matrix_set(row_matrix, 0, 1, 2.0, error);
    rikai_matrix_set(row_matrix, 0, 2, 3.0, error);
    rikai_matrix_set(row_matrix, 0, 3, 4.0, error);

    // Column matrix: [[5], [6], [7], [8]]
    rikai_matrix_set(col_matrix, 0, 0, 5.0, error);
    rikai_matrix_set(col_matrix, 1, 0, 6.0, error);
    rikai_matrix_set(col_matrix, 2, 0, 7.0, error);
    rikai_matrix_set(col_matrix, 3, 0, 8.0, error);

    // Expected result: [70] (1*5 + 2*6 + 3*7 + 4*8 = 5 + 12 + 21 + 32 = 70)
    double expected[] = {70.0};

    rikai_matrix *result = rikai_multiply_matrix(row_matrix, col_matrix, NULL, error);

    int passed = (result != NULL) &&
                 !rikai_has_error(error) &&
                 verify_matrix_values(result, expected, 1, 1, error);

    if (!passed)
    {
        print_error_if_exists(error);
    }

    print_test_result("Row x Column matrix multiplication", passed);

    rikai_matrix_destroy(row_matrix, error);
    rikai_matrix_destroy(col_matrix, error);
    rikai_matrix_destroy(result, error);
    rikai_error_destroy(error);
}

// Test 7: Using pre-allocated container with correct dimensions
void test_with_valid_container()
{
    rikai_err *error = rikai_error_create_new();
    rikai_matrix *matrix_a = rikai_matrix_create_new(2, 2, error);
    rikai_matrix *matrix_b = rikai_matrix_create_new(2, 2, error);
    rikai_matrix *container = rikai_matrix_create_new(2, 2, error);

    // Set up matrices
    rikai_matrix_set(matrix_a, 0, 0, 1.0, error);
    rikai_matrix_set(matrix_a, 0, 1, 2.0, error);
    rikai_matrix_set(matrix_a, 1, 0, 3.0, error);
    rikai_matrix_set(matrix_a, 1, 1, 4.0, error);

    rikai_matrix_set(matrix_b, 0, 0, 5.0, error);
    rikai_matrix_set(matrix_b, 0, 1, 6.0, error);
    rikai_matrix_set(matrix_b, 1, 0, 7.0, error);
    rikai_matrix_set(matrix_b, 1, 1, 8.0, error);

    double expected[] = {19.0, 22.0, 43.0, 50.0};

    rikai_matrix *result = rikai_multiply_matrix(matrix_a, matrix_b, container, error);

    int passed = (result == container) &&
                 !rikai_has_error(error) &&
                 verify_matrix_values(result, expected, 2, 2, error);

    if (!passed)
    {
        print_error_if_exists(error);
    }

    print_test_result("Matrix multiplication with valid container", passed);

    rikai_matrix_destroy(matrix_a, error);
    rikai_matrix_destroy(matrix_b, error);
    rikai_matrix_destroy(container, error);
    rikai_error_destroy(error);
}

// Test 8: NULL left matrix (error case)
void test_null_left_matrix()
{
    rikai_err *error = rikai_error_create_new();
    rikai_matrix *matrix_b = rikai_matrix_create_new(2, 2, error);

    rikai_matrix *result = rikai_multiply_matrix(NULL, matrix_b, NULL, error);

    int passed = (result == NULL) &&
                 rikai_has_error(error) &&
                 (rikai_get_error_code(error) == RIKAI_NULL_POINTER);

    print_test_result("NULL left matrix error handling", passed);

    printf("Inputs:\n");
    printf("Matrix A: NULL\n");
    print_matrix("Matrix B", matrix_b, error);
    printf("Container: NULL\n\n");

    printf("Expected Output:\n");
    printf("  NULL result with RIKAI_NULL_POINTER error\n\n");

    printf("Actual Output:\n");
    printf("  Result: %s\n", (result == NULL) ? "NULL" : "NOT NULL");
    printf("  Has Error: %s\n", rikai_has_error(error) ? "Yes" : "No");
    if (rikai_has_error(error))
    {
        printf("  Error Code: %u\n", rikai_get_error_code(error));
        char error_message[MAX_ERROR_MSG_SIZE];
        rikai_get_error(error_message, MAX_ERROR_MSG_SIZE, error);
        printf("  Error Message: %s\n", error_message);
    }

    if (!passed)
    {
        printf("    Expected NULL result and RIKAI_NULL_POINTER error, but got: result=%p, has_error=%d, error_code=%u\n",
               result, rikai_has_error(error), rikai_get_error_code(error));
    }

    printf("------------------------------------\n\n");

    rikai_matrix_destroy(matrix_b, error);
    rikai_error_destroy(error);
}

// Test 9: NULL right matrix (error case)
void test_null_right_matrix()
{
    rikai_err *error = rikai_error_create_new();
    rikai_matrix *matrix_a = rikai_matrix_create_new(2, 2, error);

    rikai_matrix *result = rikai_multiply_matrix(matrix_a, NULL, NULL, error);

    int passed = (result == NULL) &&
                 rikai_has_error(error) &&
                 (rikai_get_error_code(error) == RIKAI_NULL_POINTER);

    if (!passed)
    {
        printf("    Expected NULL result and RIKAI_NULL_POINTER error, but got: result=%p, has_error=%d, error_code=%u\n",
               result, rikai_has_error(error), rikai_get_error_code(error));
    }
    print_error_if_exists(error);

    print_test_result("NULL right matrix error handling", passed);

    rikai_matrix_destroy(matrix_a, error);
    rikai_error_destroy(error);
}

// Test 10: Incompatible dimensions (error case)
void test_incompatible_dimensions()
{
    rikai_err *error = rikai_error_create_new();
    rikai_matrix *matrix_a = rikai_matrix_create_new(2, 3, error);
    rikai_matrix *matrix_b = rikai_matrix_create_new(2, 2, error); // Should be 3x2 for compatibility

    rikai_matrix *result = rikai_multiply_matrix(matrix_a, matrix_b, NULL, error);

    int passed = (result == NULL) &&
                 rikai_has_error(error) &&
                 (rikai_get_error_code(error) == RIKAI_INVALID_DIMENSIONS_FOR_MATRIX_MULTIPLICATION);

    if (!passed)
    {
        printf("    Expected NULL result and RIKAI_INVALID_DIMENSIONS_FOR_MATRIX_MULTIPLICATION error, but got: result=%p, has_error=%d, error_code=%u\n",
               result, rikai_has_error(error), rikai_get_error_code(error));
    }
    print_error_if_exists(error);

    print_test_result("Incompatible dimensions error handling", passed);

    rikai_matrix_destroy(matrix_a, error);
    rikai_matrix_destroy(matrix_b, error);
    rikai_error_destroy(error);
}

// Test 11: Invalid container dimensions (error case)
void test_invalid_container_dimensions()
{
    rikai_err *error = rikai_error_create_new();
    rikai_matrix *matrix_a = rikai_matrix_create_new(2, 3, error);
    rikai_matrix *matrix_b = rikai_matrix_create_new(3, 2, error);
    rikai_matrix *container = rikai_matrix_create_new(3, 3, error); // Wrong size container

    rikai_matrix *result = rikai_multiply_matrix(matrix_a, matrix_b, container, error);

    int passed = (result == NULL) &&
                 rikai_has_error(error) &&
                 (rikai_get_error_code(error) == RIKAI_INVALID_MATRIX_CONTAINER_DIMENSIONS);

    if (!passed)
    {
        printf("    Expected NULL result and RIKAI_INVALID_MATRIX_CONTAINER_DIMENSIONS error, but got: result=%p, has_error=%d, error_code=%u\n",
               result, rikai_has_error(error), rikai_get_error_code(error));
    }
    print_error_if_exists(error);

    print_test_result("Invalid container dimensions error handling", passed);

    rikai_matrix_destroy(matrix_a, error);
    rikai_matrix_destroy(matrix_b, error);
    rikai_matrix_destroy(container, error);
    rikai_error_destroy(error);
}

// Test 12: Large matrix multiplication (performance test)
void test_large_matrix_multiplication()
{
    rikai_err *error = rikai_error_create_new();
    const uint32_t size = 50; // 50x50 matrices

    rikai_matrix *matrix_a = rikai_matrix_create_new(size, size, error);
    rikai_matrix *matrix_b = rikai_matrix_create_new(size, size, error);

    // Fill matrices with simple values
    for (uint32_t i = 0; i < size; i++)
    {
        for (uint32_t j = 0; j < size; j++)
        {
            rikai_matrix_set(matrix_a, i, j, (double)(i + j), error);
            rikai_matrix_set(matrix_b, i, j, (double)(i * j + 1), error);
        }
    }

    rikai_matrix *result = rikai_multiply_matrix(matrix_a, matrix_b, NULL, error);

    // Verify dimensions and no errors
    uint32_t result_rows, result_cols;
    if (result != NULL)
    {
        rikai_matrix_get_dimensions(result, &result_rows, &result_cols, error);
    }

    int passed = (result != NULL) &&
                 !rikai_has_error(error) &&
                 (result_rows == size) &&
                 (result_cols == size);

    if (!passed)
    {
        print_error_if_exists(error);
    }

    print_test_result("Large matrix multiplication (50x50)", passed);

    rikai_matrix_destroy(matrix_a, error);
    rikai_matrix_destroy(matrix_b, error);
    rikai_matrix_destroy(result, error);
    rikai_error_destroy(error);
}

// Test 13: Column vector × row vector (produces matrix)
void test_column_row_multiplication()
{
    rikai_err *error = rikai_error_create_new();
    rikai_matrix *col_matrix = rikai_matrix_create_new(3, 1, error);
    rikai_matrix *row_matrix = rikai_matrix_create_new(1, 3, error);

    // Column matrix: [[2], [3], [4]]
    rikai_matrix_set(col_matrix, 0, 0, 2.0, error);
    rikai_matrix_set(col_matrix, 1, 0, 3.0, error);
    rikai_matrix_set(col_matrix, 2, 0, 4.0, error);

    // Row matrix: [5, 6, 7]
    rikai_matrix_set(row_matrix, 0, 0, 5.0, error);
    rikai_matrix_set(row_matrix, 0, 1, 6.0, error);
    rikai_matrix_set(row_matrix, 0, 2, 7.0, error);

    // Expected result: [[10, 12, 14], [15, 18, 21], [20, 24, 28]]
    double expected[] = {10.0, 12.0, 14.0, 15.0, 18.0, 21.0, 20.0, 24.0, 28.0};

    rikai_matrix *result = rikai_multiply_matrix(col_matrix, row_matrix, NULL, error);

    int passed = (result != NULL) &&
                 !rikai_has_error(error) &&
                 verify_matrix_values(result, expected, 3, 3, error);

    if (!passed)
    {
        print_error_if_exists(error);
    }

    print_test_result("Column x Row matrix multiplication", passed);

    rikai_matrix_destroy(col_matrix, error);
    rikai_matrix_destroy(row_matrix, error);
    rikai_matrix_destroy(result, error);
    rikai_error_destroy(error);
}

// Test 14: Multiplication with negative values
void test_negative_values_multiplication()
{
    rikai_err *error = rikai_error_create_new();
    rikai_matrix *matrix_a = rikai_matrix_create_new(2, 2, error);
    rikai_matrix *matrix_b = rikai_matrix_create_new(2, 2, error);

    // Matrix A: [[-1, 2], [3, -4]]
    rikai_matrix_set(matrix_a, 0, 0, -1.0, error);
    rikai_matrix_set(matrix_a, 0, 1, 2.0, error);
    rikai_matrix_set(matrix_a, 1, 0, 3.0, error);
    rikai_matrix_set(matrix_a, 1, 1, -4.0, error);

    // Matrix B: [[5, -6], [-7, 8]]
    rikai_matrix_set(matrix_b, 0, 0, 5.0, error);
    rikai_matrix_set(matrix_b, 0, 1, -6.0, error);
    rikai_matrix_set(matrix_b, 1, 0, -7.0, error);
    rikai_matrix_set(matrix_b, 1, 1, 8.0, error);

    // Expected result: [[-19, 22], [43, -50]]
    double expected[] = {-19.0, 22.0, 43.0, -50.0};

    rikai_matrix *result = rikai_multiply_matrix(matrix_a, matrix_b, NULL, error);

    int passed = (result != NULL) &&
                 !rikai_has_error(error) &&
                 verify_matrix_values(result, expected, 2, 2, error);

    if (!passed)
    {
        print_error_if_exists(error);
    }

    print_test_result("Matrix multiplication with negative values", passed);

    rikai_matrix_destroy(matrix_a, error);
    rikai_matrix_destroy(matrix_b, error);
    rikai_matrix_destroy(result, error);
    rikai_error_destroy(error);
}

// Test 15: Fractional values multiplication
void test_fractional_values_multiplication()
{
    rikai_err *error = rikai_error_create_new();
    rikai_matrix *matrix_a = rikai_matrix_create_new(2, 2, error);
    rikai_matrix *matrix_b = rikai_matrix_create_new(2, 2, error);

    // Matrix A: [[0.5, 1.5], [2.5, 3.5]]
    rikai_matrix_set(matrix_a, 0, 0, 0.5, error);
    rikai_matrix_set(matrix_a, 0, 1, 1.5, error);
    rikai_matrix_set(matrix_a, 1, 0, 2.5, error);
    rikai_matrix_set(matrix_a, 1, 1, 3.5, error);

    // Matrix B: [[0.25, 0.75], [1.25, 1.75]]
    rikai_matrix_set(matrix_b, 0, 0, 0.25, error);
    rikai_matrix_set(matrix_b, 0, 1, 0.75, error);
    rikai_matrix_set(matrix_b, 1, 0, 1.25, error);
    rikai_matrix_set(matrix_b, 1, 1, 1.75, error);

    // Expected result: [[2.0, 3.0], [5.0, 8.0]]
    double expected[] = {2.0, 3.0, 5.0, 8.0};

    rikai_matrix *result = rikai_multiply_matrix(matrix_a, matrix_b, NULL, error);

    int passed = (result != NULL) &&
                 !rikai_has_error(error) &&
                 verify_matrix_values(result, expected, 2, 2, error);

    if (!passed)
    {
        print_error_if_exists(error);
    }

    print_test_result("Matrix multiplication with fractional values", passed);

    rikai_matrix_destroy(matrix_a, error);
    rikai_matrix_destroy(matrix_b, error);
    rikai_matrix_destroy(result, error);
    rikai_error_destroy(error);
}

int main()
{
    printf("=== RIKAI MULTIPLY MATRIX TEST SUITE ===\n\n");

    // Run all tests
    test_basic_2x2_multiplication();
    test_non_square_multiplication();
    test_identity_matrix_multiplication();
    test_1x1_matrix_multiplication();

    test_row_column_multiplication();
    test_with_valid_container();
    test_null_left_matrix();
    test_null_right_matrix();

    test_incompatible_dimensions();

    test_invalid_container_dimensions();
    test_large_matrix_multiplication();

    test_column_row_multiplication();

    test_negative_values_multiplication();
    test_fractional_values_multiplication();

    // Print summary
    printf("\n=== TEST SUMMARY ===\n");
    printf("Total tests run: %d\n", test_number);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("Success rate: %.1f%%\n", (double)tests_passed / test_number * 100.0);

    if (tests_failed == 0)
    {
        printf("\n All tests passed! The rikai_multiply_matrix function is working correctly.\n");
        return 0;
    }
    else
    {
        printf("\n Some tests failed. Please check the implementation.\n");
        return 1;
    }
}
