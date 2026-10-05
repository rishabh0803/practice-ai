#pragma once
#include <expression.h>
#include <NDArray.h>
#include <NDArrayView.h>
#include <NDArrayTransposedView.h>

class ml_math
{
private:
    ml_math();

public:
    static double sigmoid(double value);
    static NDArray *sigmoid(NDArray *array);
    static NDArray *sigmoid(NDArrayView *array);
    static NDArray *sigmoid(NDArrayTransposedView *array);
};

expression &_sigmoid(double value);
expression &sigmoid(NDArray &);
expression &sigmoid(NDArrayTransposedView &);
expression &sigmoid(NDArrayView &);
