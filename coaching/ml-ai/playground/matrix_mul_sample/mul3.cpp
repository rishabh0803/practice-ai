#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
using namespace std;

struct wrapper
{
    vector<double> *matrix_1;
    vector<double> *matrix_2;
    vector<double> *result_matrix;

    int matrix_1_rows;
    int matrix_1_cols;
    int matrix_2_rows;
    int matrix_2_cols;
    int start_at;
    int end_at;
};

void multiply_partially(struct wrapper *w)
{
    vector<double> *matrix_1 = w->matrix_1;
    vector<double> *matrix_2 = w->matrix_2;
    vector<double> *result_matrix = w->result_matrix;

    int matrix_1_rows = w->matrix_1_rows;
    int matrix_1_cols = w->matrix_1_cols;

    int matrix_2_rows = w->matrix_2_rows;
    int matrix_2_cols = w->matrix_2_cols;

    int start_at = w->start_at;
    int end_at = w->end_at;

    int r, c, c1;
    double sum;
    for (r = start_at; r < end_at; r++)
    {
        for (c = 0; c < matrix_1_cols; c++)
        {
            sum = 0.0;
            for (c1 = 0; c1 < matrix_2_cols; c1++)
            {
                sum += (*matrix_1)[r * matrix_1_cols + c] * (*matrix_2)[c * matrix_2_cols + c1];
            }
            (*result_matrix)[r * matrix_2_cols + c1] = sum;
        }
    }
}

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

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < result_matrix.size(); i++)
        result_matrix[i] = 0.0;

    auto cores = std::thread::hardware_concurrency();
    int number_of_threads = 16;

    int batch_size = matrix_1_rows / number_of_threads;
    if (matrix_1_rows % number_of_threads != 0)
        batch_size++;

    vector<thread> threads;
    vector<struct wrapper> wrappers(number_of_threads);

    int start_at = 0;
    int end_at = 0;

    for (int i = 0; i < number_of_threads; i++)
    {
        end_at = start_at + batch_size - 1;
        if (end_at > matrix_1_rows)
            end_at = matrix_1_rows - 1;

        wrappers[i].matrix_1 = &matrix_1;
        wrappers[i].matrix_2 = &matrix_2;
        wrappers[i].result_matrix = &result_matrix;

        wrappers[i].matrix_1_rows = matrix_1_rows;
        wrappers[i].matrix_1_cols = matrix_1_cols;
        wrappers[i].matrix_2_rows = matrix_2_rows;
        wrappers[i].matrix_2_cols = matrix_2_cols;

        wrappers[i].start_at = start_at;
        wrappers[i].end_at = end_at;

        start_at = end_at + 1;

        threads.push_back(thread(multiply_partially, &wrappers[i]));
    }

    for (auto &th : threads)
        th.join();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time to multiply: " << diff.count() << " s\n";

    return 0;
}