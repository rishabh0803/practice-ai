#define TRAINING_CYCLES 5
#define NUM_CLASSES 5
#define DEBUG_ROWS 6
#include <NDArray.h>
#include <NDArrayView.h>
#include <NDArrayTransposedView.h>
#include <ml_utils.h>
#include <csv_info.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ml_functions.h>

using namespace std;

// Debug helper: writes an NDArray (1D or 2D) to the trace file under a label,
// so every value can be recomputed by hand for validation.
static void trace_ndarray(ofstream &out, const string &label, NDArray *arr)
{
    auto dims = arr->get_dimensions();
    out << label << "  dims=[";
    for (size_t d = 0; d < dims.size(); ++d)
        out << dims[d] << (d + 1 < dims.size() ? "," : "");
    out << "]" << endl;
    if (dims.size() == 1)
    {
        for (uint32_t i = 0; i < dims[0]; ++i)
            out << "    [" << i << "] = " << (double)((*arr)[i]) << endl;
    }
    else if (dims.size() == 2)
    {
        for (uint32_t r = 0; r < dims[0]; ++r)
        {
            out << "   ";
            for (uint32_t c = 0; c < dims[1]; ++c)
                out << " [" << r << "][" << c << "]=" << (double)((*arr)[r][c]);
            out << endl;
        }
    }
}

// Overload for NDArrayTransposedView: it has no operator[], so read via get().
static void trace_ndarray(ofstream &out, const string &label, NDArrayTransposedView *arr)
{
    auto dims = arr->get_dimensions();
    out << label << "  dims=[";
    for (size_t d = 0; d < dims.size(); ++d)
        out << dims[d] << (d + 1 < dims.size() ? "," : "");
    out << "]" << endl;
    if (dims.size() == 1)
    {
        for (uint32_t i = 0; i < dims[0]; ++i)
            out << "    [" << i << "] = " << (double)(arr->get(i)) << endl;
    }
    else if (dims.size() == 2)
    {
        for (uint32_t r = 0; r < dims[0]; ++r)
        {
            out << "   ";
            for (uint32_t c = 0; c < dims[1]; ++c)
                out << " [" << r << "][" << c << "]=" << (double)(arr->get(r, c));
            out << endl;
        }
    }
}

int main()
{
    // load dataset
    csv_info csv = ml_utils::get_csv_info("synthetic.csv", 1);
    double regularization_parameter = 0.01; // to avoid overfitting
    double learning_rate = 0.01;

    NDArray x({csv.rows(), csv.columns()});
    NDArray target_class({csv.rows()});
    ml_utils::get_x_y(csv, x, target_class);

    // create 2D y from info in target_class
    NDArray y({csv.rows(), NUM_CLASSES});
    // populate y by using data from target_class
    NDArray::fill(y, {0, 0}, {csv.rows() - 1, NUM_CLASSES - 1}, 0);

    size_t idx;
    for (idx = 0; idx < csv.rows(); ++idx)
    {
        // each element of target class has a number between (1 to 10)
        y[idx][target_class[idx] - 1] = 1.0;
    }

    // create 2D y_hat with dimensions same as that of y
    NDArray y_hat({csv.rows(), NUM_CLASSES});
    vector<uint32_t> layer_wise_neurons_count = {csv.columns(), 4, 4, NUM_CLASSES};
    vector<NDArray *> layers(layer_wise_neurons_count.size());
    vector<NDArrayTransposedView *> layersTransposed(layer_wise_neurons_count.size() - 1);
    NDArray target_y({NUM_CLASSES});

    idx = 0;
    for (auto cnt : layer_wise_neurons_count)
    {
        layers[idx] = new NDArray({cnt});
        (*(layers[idx]))[0] = 1.0; // ideally not required for first and last layer, but wouldn't matter
        idx++;
    }

    for (idx = 0; idx < layer_wise_neurons_count.size() - 1; ++idx)
    {
        layersTransposed[idx] = new NDArrayTransposedView(layers[idx], true);
    }

    vector<NDArray *> z(layers.size() - 1);
    vector<NDArray *> lower_case_deltas(layers.size() - 1);
    for (idx = 1; idx < layer_wise_neurons_count.size() - 1; ++idx)
    {
        z[idx - 1] = new NDArray({layer_wise_neurons_count[idx] - 1});
        lower_case_deltas[idx - 1] = new NDArray({layer_wise_neurons_count[idx] - 1});
    }

    z[idx - 1] = new NDArray({layer_wise_neurons_count[idx]});
    lower_case_deltas[idx - 1] = new NDArray({layer_wise_neurons_count[idx]});
    vector<NDArray *> models(layers.size() - 1);
    vector<NDArray *> tmp_models(layers.size() - 1);
    vector<NDArrayView *> tmp_model_views(layers.size() - 1);

    vector<NDArray *> upper_case_deltas(layers.size() - 1);
    vector<NDArray *> tmp_upper_case_deltas(layers.size() - 1);
    for (idx = 0; idx < z.size() - 1; ++idx)
    {
        models[idx] = new NDArray({layer_wise_neurons_count[idx + 1] - 1, layer_wise_neurons_count[idx]});
        tmp_models[idx] = new NDArray({layer_wise_neurons_count[idx + 1] - 1, layer_wise_neurons_count[idx]});

        tmp_model_views[idx] = new NDArrayView(tmp_models[idx], {0, 1}, {layer_wise_neurons_count[idx + 1] - 1 - 1, layer_wise_neurons_count[idx] - 1}, true);
        upper_case_deltas[idx] = new NDArray({layer_wise_neurons_count[idx + 1] - 1, layer_wise_neurons_count[idx]});
        tmp_upper_case_deltas[idx] = new NDArray({layer_wise_neurons_count[idx + 1] - 1, layer_wise_neurons_count[idx]});
    }
    models[idx] = new NDArray({layer_wise_neurons_count[idx + 1], layer_wise_neurons_count[idx]});
    tmp_models[idx] = new NDArray({layer_wise_neurons_count[idx + 1], layer_wise_neurons_count[idx]});
    tmp_model_views[idx] = new NDArrayView(tmp_models[idx], {0, 1}, {layer_wise_neurons_count[idx + 1] - 1, layer_wise_neurons_count[idx] - 1}, true);
    upper_case_deltas[idx] = new NDArray({layer_wise_neurons_count[idx + 1], layer_wise_neurons_count[idx]});
    tmp_upper_case_deltas[idx] = new NDArray({layer_wise_neurons_count[idx + 1], layer_wise_neurons_count[idx]});
    vector<NDArrayView *> layers_views(layers.size() - 1);
    for (idx = 0; idx < layers_views.size() - 1; ++idx)
    {
        layers_views[idx] = new NDArrayView(layers[idx + 1], {1}, {layer_wise_neurons_count[idx + 1] - 1}, true);
    }
    layers_views[idx] = new NDArrayView(layers[idx + 1], {0}, {layer_wise_neurons_count[idx + 1] - 1}, true);

    // hard-coded initial model weights for deterministic manual validation
    // model[0] dims [3,5]
    (*(models[0]))[0][0] = 0.606412980196915; (*(models[0]))[0][1] = 0.992453107031227; (*(models[0]))[0][2] = 0.109787136998726; (*(models[0]))[0][3] = 0.412290362203887; (*(models[0]))[0][4] = 0.722321839175809;
    (*(models[0]))[1][0] = 0.494228642999667; (*(models[0]))[1][1] = 0.731318605408058; (*(models[0]))[1][2] = 0.719344802878138; (*(models[0]))[1][3] = 0.935568492697489; (*(models[0]))[1][4] = 0.341733793948539;
    (*(models[0]))[2][0] = 0.888797812321924; (*(models[0]))[2][1] = 0.376036492055393; (*(models[0]))[2][2] = 0.306521990587764; (*(models[0]))[2][3] = 0.929527518342779; (*(models[0]))[2][4] = 0.571114456254572;

    // model[1] dims [3,4]
    (*(models[1]))[0][0] = 0.984958821319939; (*(models[1]))[0][1] = 0.652620026209849; (*(models[1]))[0][2] = 0.373651903167804; (*(models[1]))[0][3] = 0.0763713983985561;
    (*(models[1]))[1][0] = 0.297004413291392; (*(models[1]))[1][1] = 0.725685024488137; (*(models[1]))[1][2] = 0.133214019685439; (*(models[1]))[1][3] = 0.0455611437390074;
    (*(models[1]))[2][0] = 0.382159148226285; (*(models[1]))[2][1] = 0.930599850429383; (*(models[1]))[2][2] = 0.245580169487924; (*(models[1]))[2][3] = 0.25380352396242;

    // model[2] dims [5,4]
    (*(models[2]))[0][0] = 0.77112070468893; (*(models[2]))[0][1] = 0.708369149485983; (*(models[2]))[0][2] = 0.991187379739956; (*(models[2]))[0][3] = 0.118948965352922;
    (*(models[2]))[1][0] = 0.986877727314285; (*(models[2]))[1][1] = 0.121819980447003; (*(models[2]))[1][2] = 0.586054347197509; (*(models[2]))[1][3] = 0.0228511834786266;
    (*(models[2]))[2][0] = 0.466125977018398; (*(models[2]))[2][1] = 0.30871369486532; (*(models[2]))[2][2] = 0.81602314507509; (*(models[2]))[2][3] = 0.673480443926495;
    (*(models[2]))[3][0] = 0.0233832294411814; (*(models[2]))[3][1] = 0.927130716720763; (*(models[2]))[3][2] = 0.47320919559489; (*(models[2]))[3][3] = 0.762412002698607;
    (*(models[2]))[4][0] = 0.244259662782993; (*(models[2]))[4][1] = 0.423909595466864; (*(models[2]))[4][2] = 0.301875955933872; (*(models[2]))[4][3] = 0.970110886125794;

    uint32_t x_row_index;
    auto x_rows = csv.rows();
    auto x_columns = csv.columns();
    uint64_t ii, jj;
    jj = layers.size() - 1;
    int iii;
    int previous_target_y = 0;
    int current_target_y;
    for (iii = 0; iii < NUM_CLASSES; ++iii)
        target_y[iii] = 0.0;
    vector<NDArrayTransposedView *> model_tvs(lower_case_deltas.size() - 1);
    vector<NDArray *> tmps(lower_case_deltas.size() - 1);
    vector<NDArray *> g_dash(lower_case_deltas.size() - 1);
    vector<NDArrayView *> tmp_views(lower_case_deltas.size() - 1);

    for (iii = 0; iii < lower_case_deltas.size() - 1; ++iii)
    {
        model_tvs[iii] = new NDArrayTransposedView(models[iii + 1], true);
        tmps[iii] = new NDArray({layer_wise_neurons_count[iii + 1]});
        g_dash[iii] = new NDArray({layer_wise_neurons_count[iii + 1] - 1});
        tmp_views[iii] = new NDArrayView(tmps[iii], {1}, {layer_wise_neurons_count[iii + 1] - 1}, true);
    }

    // ---- open trace file for manual validation ----
    ofstream trace("training_trace.txt");
    trace << setprecision(15);
    trace << "==================== INITIAL MODEL WEIGHTS ====================" << endl;
    for (idx = 0; idx < models.size(); ++idx)
        trace_ndarray(trace, "model[" + to_string(idx) + "]", models[idx]);

    uint64_t cycle = 1;
    while (cycle <= TRAINING_CYCLES)
    {
        trace << "\n==================== CYCLE " << cycle << " ====================" << endl;
        // assign 0.0 to upper case deltas before loading the first training example on input layer
        for (iii = 0; iii < upper_case_deltas.size() - 1; ++iii)
        {
            NDArray::fill(*(upper_case_deltas[iii]), {0, 0}, {layer_wise_neurons_count[iii + 1] - 2, layer_wise_neurons_count[iii] - 1}, 0.0);
        }
        NDArray::fill(*(upper_case_deltas[iii]), {0, 0}, {layer_wise_neurons_count[iii + 1] - 1, layer_wise_neurons_count[iii] - 1}, 0.0);

        // iterate on training examples (capped at DEBUG_ROWS for manual validation)
        for (x_row_index = 0; x_row_index < x_rows && x_row_index < DEBUG_ROWS; ++x_row_index)
        {
            // trace << "\n----- CYCLE " << cycle << " / ROW " << x_row_index << " -----" << endl;
            // pick x_row_index-th row and place it on first layer
            NDArray::copy(*(layers[0]), {0}, x, {x_row_index, 0}, {x_row_index, x_columns - 1});
            target_y[previous_target_y] = 0.0;
            current_target_y = (int)(target_class[x_row_index] - 1.0);
            target_y[current_target_y] = 1.0;
            previous_target_y = current_target_y;

            // -- forward/backward propagation verified; per-row tracing disabled --
            // trace_ndarray(trace, "input layer[0]", layers[0]);
            // trace_ndarray(trace, "target_y", &target_y);

            // trace << "  -- forward propagation --" << endl;
            for (ii = 0; ii < jj; ++ii)
            {
                *(z[ii]) = (*(models[ii])) * (*(layers[ii]));
                *(layers_views[ii]) = sigmoid(*(z[ii]));
                // trace_ndarray(trace, "z[" + to_string(ii) + "]", z[ii]);
                // trace_ndarray(trace, "activation layer[" + to_string(ii + 1) + "]", layers[ii + 1]);
            }
            // update y_hat with info on output_layer
            NDArray::copy(y_hat, {x_row_index, 0}, *(layers[ii]), {0}, {NUM_CLASSES - 1});

            // trace << "  -- backward propagation --" << endl;
            iii = layers.size() - 2;
            *lower_case_deltas[iii] = *layers[iii + 1] - target_y;
            // trace_ndarray(trace, "lower_case_deltas[" + to_string(iii) + "]", lower_case_deltas[iii]);
            // compute lower_case deltas
            for (--iii; iii >= 0; --iii)
            {
                *(tmps[iii]) = *(model_tvs[iii]) * (*(lower_case_deltas[iii + 1]));
                *(g_dash[iii]) = der_sigmoid(*(z[iii]));
                *(lower_case_deltas[iii]) = pointwise_product(*(tmp_views[iii]), *(g_dash[iii]));
                // trace_ndarray(trace, "lower_case_deltas[" + to_string(iii) + "]", lower_case_deltas[iii]);
            }

            // update upper_case_deltas (accumulated gradient used by model update)
            for (idx = 0; idx < upper_case_deltas.size(); ++idx)
            {
                trace_ndarray(trace, "lower_case_deltas[" + to_string(idx) + "]", lower_case_deltas[idx]);
                trace_ndarray(trace, "layers_transposed[" + to_string(idx) + "]", layersTransposed[idx]);
                trace_ndarray(trace, "tmp_upper_case_deltas[" + to_string(idx) + "]", tmp_upper_case_deltas[idx]);
                trace_ndarray(trace, "upper_case_deltas[" + to_string(idx) + "]", upper_case_deltas[idx]);

                *(tmp_upper_case_deltas[idx]) = *(lower_case_deltas[idx]) * (*(layersTransposed[idx]));
                *(upper_case_deltas[idx]) = *(upper_case_deltas[idx]) + (*(tmp_upper_case_deltas[idx]));
            }

        } // loop on x rows ends here

        // something to compute cost
        // 1/m summation summation (-y)*log(y_hat) - (1-y)*log(1-y_hat) + reg/2m summ summ summ theta^2
        NDArray minus_y({csv.rows(), NUM_CLASSES});
        NDArray log_y_hat({csv.rows(), NUM_CLASSES});
        NDArray one_minus_y({csv.rows(), NUM_CLASSES});
        NDArray one_minus_y_hat({csv.rows(), NUM_CLASSES});
        NDArray log_one_minus_y_hat({csv.rows(), NUM_CLASSES});
        NDArray left_product({csv.rows(), NUM_CLASSES});
        NDArray right_product({csv.rows(), NUM_CLASSES});
        NDArray difference({csv.rows(), NUM_CLASSES});

        minus_y = -y;
        log_y_hat = log(y_hat);
        one_minus_y = 1.0 - y;
        one_minus_y_hat = 1.0 - y_hat;
        log_one_minus_y_hat = log(one_minus_y_hat);
        left_product = pointwise_product(minus_y, log_y_hat);
        right_product = pointwise_product(one_minus_y, log_one_minus_y_hat);
        difference = left_product - right_product;

        trace << "\n  -- cost calculation for cycle " << cycle << " --" << endl;
        trace_ndarray(trace, "y (targets)", &y);
        trace_ndarray(trace, "y_hat (predictions)", &y_hat);
        trace_ndarray(trace, "minus_y (-y)", &minus_y);
        trace_ndarray(trace, "log_y_hat (log(y_hat))", &log_y_hat);
        trace_ndarray(trace, "one_minus_y (1-y)", &one_minus_y);
        trace_ndarray(trace, "one_minus_y_hat (1-y_hat)", &one_minus_y_hat);
        trace_ndarray(trace, "log_one_minus_y_hat (log(1-y_hat))", &log_one_minus_y_hat);
        trace_ndarray(trace, "left_product (-y .* log(y_hat))", &left_product);
        trace_ndarray(trace, "right_product ((1-y) .* log(1-y_hat))", &right_product);
        trace_ndarray(trace, "difference (left_product - right_product)", &difference);

        cout << "Cost before regularization : " << difference.sum() << endl;
        double cost = difference.sum() / (double)(csv.rows());
        trace << "    sum(difference)            = " << difference.sum() << endl;
        trace << "    m (csv.rows)               = " << csv.rows() << endl;
        trace << "    cost before regularization = " << cost << endl;

        // computing sum theta squared
        double models_sum = 0.0;
        for (idx = 0; idx < models.size(); ++idx)
        {
            *(tmp_models[idx]) = (*(models[idx])) ^ 2;
            trace << "    sum of squares model[" << idx << "]   = " << (*(tmp_models[idx])).sum() << endl;
            models_sum += (*(tmp_models[idx])).sum();
        }
        double reg_term = regularization_parameter / (((double)csv.rows()) * 2) * models_sum;
        cost += reg_term;
        printf("Cost %20.10lf\n", cost);
        trace << "    models_sum (sum theta^2)   = " << models_sum << endl;
        trace << "    lambda                     = " << regularization_parameter << endl;
        trace << "    regularization term        = " << reg_term << endl;
        trace << "    cost (with regularization) = " << cost << endl;

        // something to update model
        // the common part
        trace << "\n  -- model update for cycle " << cycle << " --" << endl;
        trace << "    m (x_rows) = " << x_rows << ", learning_rate = " << learning_rate
              << ", lambda = " << regularization_parameter << endl;
        for (idx = 0; idx < upper_case_deltas.size(); ++idx)
        {
            trace << "  model[" << idx << "]:" << endl;
            trace_ndarray(trace, "    accumulated upper_case_deltas (D)", upper_case_deltas[idx]);

            (*(upper_case_deltas[idx])) /= x_rows;

            trace_ndarray(trace, "    D / m", upper_case_deltas[idx]);


            *(tmp_models[idx]) = *(models[idx]);
            (*(tmp_model_views[idx])) *= (regularization_parameter / x_rows);
            // column 0 (bias) is NOT regularized: zero its reg term so
            //   col 0    -> W = W - alpha * (D/m)
            //   col >= 1 -> W = W - alpha * (D/m + (lambda/m) * W)
            auto model_dims = tmp_models[idx]->get_dimensions();
            NDArray::fill(*(tmp_models[idx]), {0, 0}, {model_dims[0] - 1, 0}, 0.0);

            trace_ndarray(trace, "    reg term (col 0 = 0, cols>=1 = lambda/m * theta)", tmp_models[idx]);

            (*(upper_case_deltas[idx])) += (*(tmp_models[idx]));
            trace_ndarray(trace, "    D/m + reg term", upper_case_deltas[idx]);

            (*(upper_case_deltas[idx])) *= learning_rate;
            trace_ndarray(trace, "    gradient step (alpha * (D/m + reg))", upper_case_deltas[idx]);

            (*(models[idx])) -= (*upper_case_deltas[idx]);
            trace_ndarray(trace, "    updated model[" + to_string(idx) + "]", models[idx]);
        }

        ++cycle;
    } // training cycles loop ends

    cout << "Done" << endl;
    trace.close();
    // code to clean up / free allocations

    return 0;
}
