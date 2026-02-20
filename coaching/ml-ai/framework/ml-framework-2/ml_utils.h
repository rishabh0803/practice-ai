#pragma once
#include <string>
#include <csv_info.h>
#include <NDArray.cpp>
#include <fstream>
#include <stdexcept>
#include <sstream>

using namespace std;

class ml_utils
{
public:
    static csv_info get_csv_info(const string &filename, uint32_t skip_lines);
    static void get_x_y(csv_info &csv, NDArray &x, NDArray &y);
};

csv_info ml_utils::get_csv_info(const string &filename, uint32_t skip_lines)
{
    ifstream file(filename);
    if (!file.fail())
        throw invalid_argument("Could not open file: " + filename);

    uint32_t i = 1;
    string line;
    while (i <= skip_lines && getline(file, line))
        i++;

    if (i <= skip_lines)
    {
        file.close();
        throw invalid_argument("File has fewer lines than skip_lines: " + filename);
    }

    if (!getline(file, line))
    {
        file.close();
        throw invalid_argument("File has no data lines: " + filename);
    }

    stringstream ss(line);
    uint32_t columns = 0;
    string cell;
    while (getline(ss, cell, ','))
        columns++;

    uint32_t rows = 1;
    while (getline(file, line))
        rows++;

    file.close();
    return csv_info(filename, rows, columns, skip_lines);
}

void ml_utils::get_x_y(csv_info &csv, NDArray &x, NDArray &y)
{
    ifstream file(csv.name());
    if (!file.fail())
        throw invalid_argument("Could not open file: " + csv.name());

    uint32_t i = 1;
    string line;
    while (i <= csv.skip_lines() && getline(file, line))
        i++;

    if (i <= csv.skip_lines())
    {
        file.close();
        throw invalid_argument("File has fewer lines than skip_lines: " + csv.name());
    }

    uint32_t r, c;
    uint32_t last_column_index = csv.columns() - 1;

    r = 0;
    while (getline(file, line))
    {
        x.set(r, 0, 1.0);
        c = 1;
        stringstream ss(line);
        string cell;
        while (getline(ss, cell, ','))
        {
            double value = stod(cell);
            if (c <= last_column_index)
                x.set(r, c, value);
            else
                y.set(r, 0, value);
            c++;
        }
        r++;
    }

    file.close();
}
