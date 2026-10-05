#pragma once
#include <NDArray.h>
#include <NDArrayView.h>
#include <NDArrayTransposedView.h>
#include <OneParamFunc.h>
#include <math.h>

using namespace std;

double sigmoid(double x);
double der_sigmoid(double x);
double log_with_eps(double v);
