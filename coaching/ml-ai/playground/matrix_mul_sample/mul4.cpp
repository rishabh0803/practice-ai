#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
int main()
{
    int matrix_1_rows = 4096;
    int matrix_1_cols = 4096;
    int matrix_2_rows = 4096;
    int matrix_2_cols = 4500;

    vector<double> matrix_1(matrix_1_rows * matrix_1_cols);
    vector<double> matrix_2(matrix_2_rows * matrix_2_cols);

    vector<double> result_matrix(matrix_1_rows * matrix_2_cols);

    double value = 1.0;
    for (int i = 0; i < matrix_1.size(); i++)
        matrix_1[i] = value++;
    value = 10.0;
    for (int i = 0; i < matrix_2.size(); i++)
        matrix_2[i] = value++;

    int r, c, c1;
    double sum;
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < result_matrix.size(); i++)
        result_matrix[i] = 0.0;

#pragma omp parallel for private(c, c1, sum)
    for (r = 0; r < matrix_1_rows; r++)
    {
        for (c = 0; c < matrix_1_cols; c++)
        {
            for (c1 = 0; c1 < matrix_2_cols; c1++)
            {
                result_matrix[r * matrix_2_cols + c1] += matrix_1[r * matrix_1_cols + c] * matrix_2[c * matrix_2_cols + c1];
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time to multiply: " << diff.count() << " s\n";

    return 0;
}