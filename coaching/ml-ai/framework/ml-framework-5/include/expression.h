#pragma once
#include <cstdint>
#include <iostream>
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

class NDArray;
class NDArrayTransposedView;
class NDArrayView;
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

class expression_pool
{
public:
    expression *expressions;
    uint32_t tail;
    uint32_t sz;
    expression_pool();
    ~expression_pool();
};

class expression
{
public:
    operand left;
    operand right;
    char oper;
    uint64_t flags;
    expression_result result;

    static expression *get_expression();
    static expression_pool pool;
    static void reset();

    expression();
    expression &operator+(NDArray &right);
    expression &operator+(expression &right);
    expression &operator*(NDArray &right);
    expression &operator*(expression &right);
};
