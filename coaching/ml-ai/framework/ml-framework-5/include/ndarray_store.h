#pragma once
#include <cstdint>
#include <unordered_map>
#include <dimension_hash.h>
#include <ndarray_stack.h>

using namespace std;

class NDArrayStore
{
private:
    unordered_map<vector<uint32_t>, NDArrayStack *, DimensionHash> ndArrayPool;

public:
    NDArray *get(vector<uint32_t> &v);
    void put(NDArray *ndArray);
    ~NDArrayStore();
};
