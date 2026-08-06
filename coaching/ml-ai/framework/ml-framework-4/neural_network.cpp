#include <NDArray.h>
#include <NDArrayTransposedView.h>
#include <NDArrayView.h>
#include <csv_info.h>
#include <ml_utils.h>
#include <ml_functions.h>
#include <unistd.h>

using namespace std;

int main()
{
    csv_info csv = ml_utils::get_csv_info("tmdigits.csv", 1);
    NDArray x({csv.rows(), csv.columns()});
    NDArray y({csv.rows()});

    ml_utils::get_x_y(csv, x, y);

    uint32_t number_of_layers = 6;
    uint32_t number_of_classes = 10;
    size_t idx = 0;

    vector<uint32_t> layer_wise_neurons_count = {csv.columns(), 4, 5, 4, 9, number_of_classes};

    // Create neurons for each layer
    vector<NDArray *> layers(number_of_layers);
    for (auto cnt : layer_wise_neurons_count)
    {
        layers[idx] = new NDArray({cnt});
        *(layers[idx])[0] = 1.0; // ideally not required for first and last layer, but wouldn't matter
        idx++;
    }

    // Create deltas for each layer, for output layer we will not be using bias term,
    // so we will not be creating delta for bias term
    vector<NDArray *> deltas(number_of_layers - 1);
    for (idx = 1; idx < layer_wise_neurons_count.size() - 1; ++idx)
        deltas[idx - 1] = new NDArray({layer_wise_neurons_count[idx] - 1});

    deltas[idx - 1] = new NDArray({layer_wise_neurons_count[idx]});

    // Create model for each layer, for output layer we will not be using bias term
    // so we will not be creating model for bias term
    vector<NDArray *> models(number_of_layers - 1);
    for (idx = 0; idx < deltas.size() - 1; ++idx)
    {
        models[idx] = new NDArray({layer_wise_neurons_count[idx + 1] - 1, layer_wise_neurons_count[idx]});
        NDArray::fill_random(*models[idx], {0, 0}, {layer_wise_neurons_count[idx + 1] - 2, layer_wise_neurons_count[idx] - 1}, 0, 1);
    }

    models[idx] = new NDArray({layer_wise_neurons_count[idx + 1], layer_wise_neurons_count[idx]});
    NDArray::fill_random(*models[idx], {0, 0}, {layer_wise_neurons_count[idx + 1] - 1, layer_wise_neurons_count[idx] - 1}, 0, 1);

    // Create view for each layer, for output layer we will not be using bias term
    // so we will not be creating view for bias term
    vector<NDArrayView *> layers_view(number_of_layers - 1);
    for (idx = 0; idx < layers_view.size() - 1; ++idx)
        layers_view[idx] = new NDArrayView(layers[idx + 1], {1}, {layer_wise_neurons_count[idx + 1] - 1}, true);

    layers_view[idx] = new NDArrayView(layers[idx + 1], {0}, {layer_wise_neurons_count[idx + 1] - 1}, true);

    // For debugging lets print the dimensions of all the layers, deltas and model
    // cout << "====== Dimensions of layers ======" << endl;
    // print_dimensions(layers);
    // cout << "====== Dimensions of deltas ======" << endl;
    // print_dimensions(deltas);
    // cout << "====== Dimensions of models ======" << endl;
    // print_dimensions(models);
    // cout << "====== Dimensions of layers view ======" << endl;
    // print_dimensions(layers_view);

    uint32_t x_row_index;
    auto x_rows = csv.rows();
    auto x_columns = csv.columns();
    uint64_t ii, jj;
    jj = layers.size() - 1;

    cout << csv.name() << " has " << x_rows << " rows and " << x_columns << " columns." << endl;
    for (x_row_index = 0; x_row_index < x_rows; ++x_row_index)
    {
        // copy input to first layer
        NDArray::copy(*layers[0], {0}, x, {x_row_index, 0}, {x_row_index, x_columns - 1});

        for (ii = 0; ii < jj; ++ii)
        {
            *(deltas[ii]) = (*models[ii]) * (*layers[ii]);
            *(layers_view[ii]) = sigmoid(*(deltas[ii]));
        }
        cout << "Processed row " << x_row_index + 1 << " out of " << x_rows << endl;
        // print layers data for debugging
        for (idx = 0; idx < layers.size(); ++idx)
        {
            cout << "Layer " << idx << " data: ";
            layers[idx]->print();
        }
    }

    // release all
    for (auto &t : layers)
        delete t;
    for (auto &t : deltas)
        delete t;
    for (auto &t : models)
        delete t;
    for (auto &t : layers_view)
        delete t;

    return 0;
}

void print_dimensions(const vector<NDArray *> &arrays)
{
    for (size_t idx = 0; idx < arrays.size(); ++idx)
    {
        auto dimensions = arrays[idx]->get_dimensions();
        cout << "Layer " << idx << " dimensions: { ";
        for (auto &d : dimensions)
            cout << d << " ";
        cout << "}" << endl;
    }
}

void print_dimensions(const vector<NDArrayView *> &arrays)
{
    for (size_t idx = 0; idx < arrays.size(); ++idx)
    {
        auto dimensions = arrays[idx]->get_dimensions();
        cout << "Layer view " << idx << " dimensions: { ";
        for (auto &d : dimensions)
            cout << d << " ";
        cout << "}" << endl;
    }
}
