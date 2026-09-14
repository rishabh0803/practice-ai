#pragma once
#include <cstdint>
#include <iostream>
#include <NDArray.h>
#include <NDArrayView.h>
#include <NDArrayTransposedView.h>
using namespace std;

typedef enum _expression_enums
{
    LEFT_IS_NDARRAY = 1,
    LEFT_IS_NDARRAY_VIEW = 2,
    LEFT_IS_NDARRAY_TRANSPOSED_VIEW = 4,
    LEFT_IS_DOUBLE = 8,
    LEFT_IS_FUNCTION = 16,
    LEFT_IS_EXPRESSION = 32,
    RIGHT_IS_NDARRAY = 64,
    RIGHT_IS_NDARRAY_VIEW = 128,
    RIGHT_IS_NDARRAY_TRANSPOSED_VIEW = 256,
    RIGHT_IS_DOUBLE = 512,
    RIGHT_IS_FUNCTION = 1024,
    RIGHT_IS_EXPRESSION = 2048,
    RESULT_IS_DOUBLE = 4096,
    RESULT_IS_NDARRAY = 8192

} expression_enum;

class expression;
typedef union _operand
{
    NDArray *ndArray;
    NDArrayView *ndArrayView;
    NDArrayTransposedView *ndArrayTransposedView;
    double value;
    void (*func)();
    expression *expr;
} operand;

typedef union _expression_result
{
    double value;
    NDArray *ndArray;
} expression_result;

class expression
{
public:
    operand left;
    operand right;
    char oper;
    uint64_t flags;
    expression_result result;
    expression *next;
    static expression *expressions;
    static uint32_t tail;
    static uint32_t sz;
    expression();
    static expression *get_expression();
};

expression *expression::expressions = NULL;
uint32_t expression::tail = 0;
uint32_t sz = 0;
