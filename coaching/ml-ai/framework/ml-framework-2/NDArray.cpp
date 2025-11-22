#include <vector>
#include <iostream>
#include <stdint.h>
#include <numeric>
#include <sstream>
#include <stdarg.h>

#define T1 double

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

class NDArray
{
private:
    std::vector<T1> data;
    std::vector<uint32_t> dimensions;

public:
    NDArray(const std::vector<uint32_t> &);
    // compiler will introduce copy/move constructors and copy/move assignment operators

    void set(uint32_t, ...);
    T1 get(uint32_t, ...) const;
};

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

void NDArray::set(uint32_t index1, ...)
{
    va_list args;
    va_start(args, index1);

    uint32_t flat_index = index1;
    for (size_t dim = 1; dim < dimensions.size(); ++dim)
    {
        uint32_t next_index = va_arg(args, uint32_t);
        flat_index = flat_index * dimensions[dim] + next_index;
    }

    data[flat_index] = va_arg(args, T1);
    va_end(args);
}

T1 NDArray::get(uint32_t index1, ...) const
{
    return 0.0;
}

int main()
{
}

T1 NDArray::get(uint32_t index1, ...) const
{
    return 0.0;
}

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