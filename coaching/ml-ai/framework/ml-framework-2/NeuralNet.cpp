#pragma once
#include <NDArray.cpp>
#include <ml_utils.h>
#include <csv_info.h>
#include <iostream>

using namespace std;

int main()
{
    csv_info csv = ml_utils::get_csv_info("data.csv", 0);

    NDArray x({csv.rows(), csv.columns()});
    NDArray y({csv.rows(), 1});

    auto x_dimensions = x.get_dimensions();
    auto x_rows = x_dimensions[0];
    auto x_columns = x_dimensions[1];

    auto y_dimensions = y.get_dimensions();
    auto y_rows = y_dimensions[0];
    auto y_columns = y_dimensions[1];

    ml_utils::get_x_y(csv, x, y);

    uint32_t number_of_layers = 5;
    uint32_t number_of_classes = 10;

    vector<NDArray *> nu(number_of_layers);
    vector<NDArray *> v(number_of_layers - 1);
    vector<NDArray *> model(number_of_layers - 1);

    vector<uint32_t> nu_dimensions = {x_columns, 4, 6, 3, number_of_classes};
    vector<uint32_t> v_dimensions(number_of_layers - 1);
    for (uint32_t i = 0; i < number_of_layers - 1; i++)
        v_dimensions[i] = nu_dimensions[i + 1];

    for (uint32_t i = 0; i < number_of_layers; i++)
        nu[i] = new NDArray({nu_dimensions[i]});

    for (auto &x : nu)
        delete x;
    for (auto &x : v)
        delete x;
    for (auto &x : model)
        delete x;
}
