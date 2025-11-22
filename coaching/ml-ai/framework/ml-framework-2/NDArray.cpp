#include <vector>
#include <unordered_set>
#include <iostream>
#include <stdint.h>
#include <numeric>
#include <sstream>
#include <stdarg.h>

#define T1 double
// ------------------------------------------------------------------------------
// NDArrayException class
class NDArrayException : public std::exception
{
private:
    std::string message;

public:
    NDArrayException(const std::string &);
    const char *what() const noexcept override;
};

NDArrayException::NDArrayException(const std::string &_message) : message(_message) {}

const char *NDArrayException::what() const noexcept
{
    return message.c_str();
}

// ------------------------------------------------------------------------------
class NDArrayNode;
class NDArray
{
private:
    std::vector<T1> data;
    std::vector<uint32_t> dimensions;
    std::unordered_set<NDArrayNode *> nodes;
    void _set(uint32_t, T1);
    T1 _get(uint32_t) const;

public:
    NDArray(const std::vector<uint32_t> &);
    // compiler will introduce copy/move constructors and copy/move assignment operators

    template <typename TT>
    void set(TT ...);

    template <typename TT>
    T1 get(TT ...) const;

    NDArrayNode operator[](uint32_t);
    operator T1() const;

friend class NDArrayNode;
};
// ------------------------------------------------------------------------------

class NDArrayNode
{
private:
    NDArray *array;
    std::vector<uint32_t> indices;
    NDArrayNode(NDArray *, const std::vector<uint32_t> &);

public:
    NDArrayNode operator[](uint32_t);
    operator T1() const; 

friend class NDArray;
};
// ------------------------------------------------------------------------------
// NDArray implementations

NDArray::NDArray(const std::vector<uint32_t> &_dimensions)
{
    if (_dimensions.empty())
    {
        throw NDArrayException("Dimensions cannot be empty");
    }

    uint32_t product = std::accumulate(_dimensions.begin(), _dimensions.end(), 1, std::multiplies<uint32_t>());
    if (product == 0)
    {
        std::ostringstream oss;
        for (uint32_t index : _dimensions)
            oss << "[" << index << "]";

        throw NDArrayException("Dimensions cannot contain zero" + oss.str());
    }

    this->dimensions = _dimensions;
    this->data.resize(product);
}

void NDArray::_set(uint32_t flat_index, T1 value)
{
    data[flat_index] = value;
}

T1 NDArray::_get(uint32_t flat_index) const
{
    return data[flat_index];
}

template <typename TT>
void NDArray::set(TT ...)
{
}

template <typename TT>
T1 NDArray::get(TT ...) const
{
    return 0.0;
}

NDArrayNode NDArray::operator[](uint32_t index)
{
}

NDArray::operator T1() const
{
}
// ------------------------------------------------------------------------------
// NDArrayNode implementations

NDArrayNode::NDArrayNode(NDArray *_array, const std::vector<uint32_t> &_indices)
{
}

NDArrayNode NDArrayNode::operator[](uint32_t index)
{
}

NDArrayNode::operator T1() const
{
}

// ------------------------------------------------------------------------------

int main()
{
    try
    {
        NDArray array({3, 4, 0});
        array.set(0, 1.0);
        std::cout << "Value at index 0: " << array.get(0) << std::endl;
    }
    catch (const NDArrayException &e)
    {
        std::cerr << "NDArrayException: " << e.what() << std::endl;
    }

    return 0;
}