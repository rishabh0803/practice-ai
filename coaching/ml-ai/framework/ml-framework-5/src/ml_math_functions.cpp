#include <ml_math_functions.h>
#include <ndarray_store.h>

double sigmoid(double value)
{
    return 1 / (1 + exp(-value));
}

expression &sigmoid(NDArray &ndArray)
{
    NDArray *(*func)(NDArray *);
    expression *t;
    t = expression::get_expression();
    func = sigmoid;
    t->left.func = (void (*)())func;
    t->oper = (char)1;
    t->right.ndArray = &ndArray;
    t->result.value = 0;
    t->flags = LEFT_IS_FUNCTION | RIGHT_IS_NDARRAY;
    t->next = NULL;
    return *t;
}

expression &sigmoid(NDArrayTransposedView &ndArrayTransposedView)
{
    NDArray *(*func)(NDArrayTransposedView *);
    expression *t;
    t = expression::get_expression();
    func = sigmoid;
    t->left.func = (void (*)())func;
    t->oper = (char)1;
    t->right.ndArrayTransposedView = &ndArrayTransposedView;
    t->result.value = 0;
    t->flags = LEFT_IS_FUNCTION | RIGHT_IS_NDARRAY_TRANSPOSED_VIEW;
    t->next = NULL;
    return *t;
}

expression &sigmoid(NDArrayView &ndArrayView)
{
    NDArray *(*func)(NDArrayView *);
    expression *t;
    t = expression::get_expression();
    func = sigmoid;
    t->left.func = (void (*)())func;
    t->oper = (char)1;
    t->right.ndArrayView = &ndArrayView;
    t->result.value = 0;
    t->flags = LEFT_IS_FUNCTION | RIGHT_IS_NDARRAY_VIEW;
    t->next = NULL;
    return *t;
}

NDArray *sigmoid(NDArray *ndArray)
{
    double *source_data = ndArray->data();
    NDArray *result = NDArrayStore::get(ndArray->get_dimensions());
    double *target_data = result->data();
    uint32_t data_size = ndArray->data_size();
    for (uint32_t i = 0; i < data_size; ++i)
        target_data[i] = sigmoid(source_data[i]);
    return result;
}

NDArray *sigmoid(NDArrayView *ndArrayView)
{
    return NULL;
}

NDArray *sigmoid(NDArrayTransposedView *ndArrayTransposedView)
{
    return NULL;
}
