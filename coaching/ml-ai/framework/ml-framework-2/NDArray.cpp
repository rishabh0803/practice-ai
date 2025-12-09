#include <vector>
#include <unordered_set>
#include <iostream>
#include <stdint.h>
#include <numeric>
#include <sstream>
#include <stdarg.h>
using namespace std;

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
    NDArray(const NDArray &);
    NDArray(NDArray &&);
    NDArray &operator=(const NDArray &);
    NDArray &operator=(NDArray &&);
    ~NDArray();

    template <typename... TT>
    void set(TT...);

    template <typename... TT>
    T1 get(TT...) const;

    NDArrayNode operator[](uint32_t);
    operator T1() const;

    friend class NDArrayNode;
};
// ------------------------------------------------------------------------------

class NDArrayNode
{
private:
    NDArray *ndArray;
    std::vector<uint32_t> indices;
    NDArrayNode(NDArray *, const std::vector<uint32_t> &);

public:
    NDArrayNode operator[](uint32_t);
    operator T1() const;
    T1 operator=(T1);
    ~NDArrayNode();

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

NDArray::NDArray(const NDArray &other)
{
    this->dimensions = other.dimensions;
    this->data = other.data;
}

NDArray::NDArray(NDArray &&other)
{
    this->dimensions = std::move(other.dimensions);
    this->data = std::move(other.data);

    for (NDArrayNode *node : other.nodes)
    {
        node->ndArray = NULL;
        node->indices.resize(0);
    }

    other.nodes.clear();
}

NDArray::~NDArray()
{
    for (NDArrayNode *node : nodes)
    {
        node->ndArray = NULL;
        node->indices.resize(0);
    }
}

NDArray &NDArray::operator=(const NDArray &other)
{
    for (NDArrayNode *node : nodes)
    {
        node->ndArray = NULL;
        node->indices.resize(0);
    }

    nodes.clear();
    this->dimensions = other.dimensions;
    this->data = other.data;
    return *this;
}

NDArray &NDArray::operator=(NDArray &&other)
{
    for (NDArrayNode *node : nodes)
    {
        node->ndArray = NULL;
        node->indices.resize(0);
    }

    nodes.clear();
    this->dimensions = std::move(other.dimensions);
    this->data = std::move(other.data);

    for (NDArrayNode *node : other.nodes)
    {
        node->ndArray = NULL;
        node->indices.resize(0);
    }

    other.nodes.clear();
    return *this;
}

void NDArray::_set(uint32_t flat_index, T1 value)
{
    data[flat_index] = value;
}

T1 NDArray::_get(uint32_t flat_index) const
{
    return data[flat_index];
}

template <typename... TT>
void NDArray::set(TT... arguments)
{
    vector<uint32_t> indices;
    double value;

    (indices.push_back((uint32_t)arguments), ...);
    ((value = arguments), ...);

    indices.pop_back();
    if (indices.size() != dimensions.size())
    {
        ostringstream oss;
        oss << "Invalid number of dimensions, expected " << dimensions.size() << ", got " << indices.size();
        throw NDArrayException(oss.str());
    }

    uint64_t flat_index = 0;
    uint32_t stride = 1;

    for (int i = dimensions.size() - 1; i >= 0; --i)
    {
        if (indices[i] >= dimensions[i])
        {
            ostringstream oss;
            oss << "Index out of bounds (";
            for (uint32_t index : indices)
                oss << index << ", ";
            oss << ") for dimensions (";
            for (uint32_t dim : dimensions)
                oss << dim << ", ";
            oss << ")";
            throw NDArrayException(oss.str());
        }

        flat_index += indices[i] * stride;
        stride *= dimensions[i];
    }

    this->data[flat_index] = value;
}

template <typename... TT>
T1 NDArray::get(TT... arguments) const
{
    vector<uint32_t> indices;
    (indices.push_back((uint32_t)arguments), ...);
    if (indices.size() != dimensions.size())
    {
        ostringstream oss;
        oss << "Invalid number of dimensions, expected " << dimensions.size() << ", got " << indices.size();
        throw NDArrayException(oss.str());
    }

    uint64_t flat_index = 0;
    uint32_t stride = 1;
    for (int i = dimensions.size() - 1; i >= 0; --i)
    {
        if (indices[i] >= dimensions[i])
        {
            ostringstream oss;
            oss << "Index out of bounds (";
            for (uint32_t index : indices)
                oss << index << ", ";
            oss << ") for dimensions (";
            for (uint32_t dim : dimensions)
                oss << dim << ", ";
            oss << ")";
            throw NDArrayException(oss.str());
        }

        flat_index += indices[i] * stride;
        stride *= dimensions[i];
    }

    return this->data[flat_index];
}

NDArrayNode NDArray::operator[](uint32_t index)
{
    if (index >= dimensions[0])
    {
        ostringstream oss;
        oss << "Index out of bounds (" << index << ") for dimension size (" << dimensions[0] << ")";
        throw NDArrayException(oss.str());
    }

    return NDArrayNode(this, {index});
}

NDArray::operator T1() const
{
    throw NDArrayException("Cannot convert NDArray to scalar, array has dimensions");
}
// ------------------------------------------------------------------------------
// NDArrayNode implementations

NDArrayNode::NDArrayNode(NDArray *array, const std::vector<uint32_t> &indices)
{
    this->ndArray = array;
    this->indices = indices;
    if (this->ndArray)
        this->ndArray->nodes.insert(this);
}

NDArrayNode::~NDArrayNode()
{
    if (this->ndArray)
        this->ndArray->nodes.erase(this);
}

NDArrayNode NDArrayNode::operator[](uint32_t index)
{
    if (this->ndArray == NULL)
    {
        throw NDArrayException("NDArrayNode is no longer valid");
    }

    if (indices.size() == this->ndArray->dimensions.size())
    {
        throw NDArrayException("Cannot index further, already at scalar level");
    }

    std::vector<uint32_t> new_indices = this->indices;
    new_indices.push_back(index);
    return NDArrayNode(this->ndArray, new_indices);
}

NDArrayNode::operator T1() const
{
    if (this->ndArray == NULL)
    {
        throw NDArrayException("NDArrayNode is no longer valid");
    }

    if (indices.size() != this->ndArray->dimensions.size())
    {
        throw NDArrayException("Cannot convert NDArrayNode to scalar, not all indices specified");
    }

    uint64_t flat_index = 0;
    uint32_t stride = 1;
    for (int i = this->ndArray->dimensions.size() - 1; i >= 0; --i)
    {
        flat_index += indices[i] * stride;
        stride *= this->ndArray->dimensions[i];
    }
    return this->ndArray->data[flat_index];
}

T1 NDArrayNode::operator=(T1 value)
{
    if (this->ndArray == NULL)
    {
        throw NDArrayException("NDArrayNode is no longer valid");
    }

    if (indices.size() != this->ndArray->dimensions.size())
    {
        throw NDArrayException("Cannot assign to NDArrayNode, not all indices specified");
    }

    uint64_t flat_index = 0;
    uint32_t stride = 1;
    for (int i = this->ndArray->dimensions.size() - 1; i >= 0; --i)
    {
        flat_index += indices[i] * stride;
        stride *= this->ndArray->dimensions[i];
    }

    this->ndArray->data[flat_index] = value;
    return value;
}

// ------------------------------------------------------------------------------

int main()
{
    try
    {
        NDArray a({10});
        a.set(0, 10.0);

        NDArrayNode b = a[0];
        double val = b;
        cout << "Value at a[0]: " << val << endl;

        NDArray c({3, 4});
        c.set(2, 3, 42.0);
        c[0][0] = 5;

        cout << "Value at c[2,3]: " << c.get(2, 3) << endl;
        cout << "Value at c[0,0]: " << c[0][0] << endl;

        c[1] = 10;
    }
    catch (const NDArrayException &e)
    {
        std::cerr << "NDArrayException: " << e.what() << std::endl;
    }

    return 0;
}