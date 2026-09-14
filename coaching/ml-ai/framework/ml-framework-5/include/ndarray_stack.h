#pragma once
#include <iostream>
#include <vector>
#include <NDArray.h>
#include <cstdint>
using namespace std;

class NDArrayStack
{
private:
    vector<NDArray *> stack;
    uint64_t initial_capacity;
    int64_t top;

public:
    NDArrayStack(uint64_t initial_capacity)
    {
        this->initial_capacity = initial_capacity;
        this->stack.resize(this->initial_capacity);
        this->top = -1;
    }
    int is_empty()
    {
        return this->top == -1;
    }
    void push(NDArray *ndArray)
    {
        if (this->top == this->stack.size() - 1)
        {
            this->stack.resize(this->stack.size() + this->initial_capacity);
        }
        this->top++;
        this->stack[this->top] = ndArray;
    }
    NDArray *pop()
    {
        if (this->top == -1)
            return NULL;
        NDArray *ndArray = this->stack[this->top];
        this->top--;
        return ndArray;
    }
};
