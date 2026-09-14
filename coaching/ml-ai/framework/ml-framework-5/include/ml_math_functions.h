#pragma once
#include <expression.h>
#include <NDArray.h>
#include <NDArrayView.h>
#include <NDArrayTransposedView.h>

double sigmoid(double value);

expression &sigmoid(NDArray &);
expression &sigmoid(NDArrayTransposedView &);
expression &sigmoid(NDArrayView &);

NDArray *sigmoid(NDArray *);
NDArray *sigmoid(NDArrayView *);
NDArray *sigmoid(NDArrayTransposedView *);
