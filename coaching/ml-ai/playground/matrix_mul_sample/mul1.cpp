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

    vector<double> matrix_(matrix_1_rows * matrix_1_cols);
    vector<double> matrix_2(matrix_2_rows * matrix_2_cols);

    vector<double> result_matrix(matrix_1_rows * matrix_2_cols);

    double value = 1.0;
    for (int i = 0; i < matrix_.size(); i++)
        matrix_[i] = value++;
    value = 10.0;
    for (int i = 0; i < matrix_2.size(); i++)
        matrix_2[i] = value++;

    int r, c, j;
    double sum;
    auto start = std::chrono::high_resolution_clock::now();

    for (r = 0; r < matrix_1_rows; r++)
    {
        for (c = 0; c < matrix_2_cols; c++)
        {
            sum = 0.0;
            for (j = 0; j < matrix_1_cols; j++)
            {
                sum += matrix_[r * matrix_1_cols + j] * matrix_2[j * matrix_2_cols + c];
            }
            result_matrix[r * matrix_2_cols + c] = sum;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time to multiply: " << diff.count() << " s\n";

    return 0;
}