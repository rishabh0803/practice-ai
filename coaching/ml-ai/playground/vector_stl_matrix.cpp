#include <iostream>
#include <vector>

using namespace std;

// Overloading operator* as a free function for vector<vector<int>>
vector<vector<int>> operator*(const vector<vector<int>> &a, const vector<vector<int>> &b)
{
    int n = a.size();
    int m = a[0].size();
    int p = b[0].size();
    vector<vector<int>> result(n, vector<int>(p, 0));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < p; ++j)
            for (int k = 0; k < m; ++k)
                result[i][j] += a[i][k] * b[k][j];
    return result;
}

void printMatrix(const vector<vector<int>> &matrix)
{
    for (const auto &row : matrix)
    {
        for (const auto &elem : row)
            cout << elem << " ";
        cout << endl;
    }
}

int main()
{
    int n = 3, m = 4, p = 3;
    vector<vector<int>> mat1(n, vector<int>(m));
    vector<vector<int>> mat2(m, vector<int>(p));

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            mat1[i][j] = i + j;

    for (int i = 0; i < m; ++i)
        for (int j = 0; j < p; ++j)
            mat2[i][j] = i + j;

    vector<vector<int>> result = mat1 * mat2;

    // printing mat1
    cout << "Matrix 1:" << endl;
    printMatrix(mat1);

    // printing mat2
    cout << "Matrix 2:" << endl;
    printMatrix(mat2);

    // printing result
    cout << "Resultant Matrix after multiplication:" << endl;
    printMatrix(result);

    return 0;
}