#include <ndarray_store.h>

NDArray *NDArrayStore::get(vector<uint32_t> &v)
{
    auto iter = ndArrayPool.find(v);
    NDArray *ndArray;
    if (iter == ndArrayPool.end())
    {
        ndArray = new NDArray(v);
    }
    else
    {
        NDArrayStack *ndArrayStack = iter->second;
        if (ndArrayStack->is_empty())
            ndArray = new NDArray(v);
        else
            ndArray = ndArrayStack->pop();
    }
    return ndArray;
}

void NDArrayStore::put(NDArray *ndArray)
{
    vector v = ndArray->dimensions;
    auto iter = ndArrayPool.find(v);
    if (iter == ndArrayPool.end())
    {
        NDArrayStack *ndArrayStack = new NDArrayStack(1024);
        ndArrayStack->push(ndArray);
        ndArrayPool.insert({v, ndArrayStack});
    }
    else
    {
        NDArrayStack *ndArrayStack = iter->second;
        ndArrayStack->push(ndArray);
    }
}

NDArrayStore::~NDArrayStore()
{
    NDArrayStack *ndArrayStack;
    NDArray *ndArray;
    for (auto &pr : this->ndArrayPool)
    {
        ndArrayStack = pr.second;
        while (!(ndArrayStack->is_empty()))
        {
            ndArray = ndArrayStack->pop();
            delete ndArray;
        }
        delete ndArrayStack;
    }
}
