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
    uint32_t computeFlatIndex(const std::vector<uint32_t> &) const;

    class InfixExpression
    {
    private:
        NDArray *left;
        NDArray *right;
        char op;
        InfixExpression(NDArray *left, char op, NDArray *right)
        {
            this->left = left;
            this->right = right;
            this->op = op;
        }
        friend class NDArray;
    };

    void just_do_it(vector<uint32_t> source_from, vector<uint32_t> source_to)
    {
        int source_index;

        int source_last_index = source_from.size() - 1;
        vector<uint32_t> source_tmp_indices = source_from;
        while (1)
        {
            while (source_tmp_indices[source_last_index] <= source_to[source_last_index])
            {
                for (auto x : source_tmp_indices)
                    cout << "[" << x << "]";
                cout << " = " << this->data[this->computeFlatIndex(source_tmp_indices)] << " ";

                cout << endl;
                source_tmp_indices[source_last_index]++;
            }

            source_index = source_last_index - 1;
            while (source_index >= 0 && source_tmp_indices[source_index] == source_to[source_index])
                source_index--;

            if (source_index < 0)
                break;

            source_tmp_indices[source_index]++;
            source_index++;

            while (source_index <= source_last_index)
            {
                source_tmp_indices[source_index] = source_from[source_index];
                source_index++;
            }
        }
    }

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

    void print()
    {
        vector<uint32_t> source_from(dimensions.size(), 0);
        vector<uint32_t> source_to = dimensions;
        for (auto &x : source_to)
            x--;

        just_do_it(source_from, source_to);
    }

    static void fill(NDArray &array, vector<uint32_t> from, vector<uint32_t> to, T1 value)
    {
        int index;

        int last_index = from.size() - 1;
        vector<uint32_t> source_tmp_indices = from;
        while (1)
        {
            while (source_tmp_indices[last_index] <= to[last_index])
            {
                array.data[array.computeFlatIndex(source_tmp_indices)] = value;
                source_tmp_indices[last_index]++;
            }

            index = last_index - 1;
            while (index >= 0 && source_tmp_indices[index] == to[index])
                index--;

            if (index < 0)
                break;

            source_tmp_indices[index]++;
            index++;

            while (index <= last_index)
            {
                source_tmp_indices[index] = from[index];
                index++;
            }
        }
    }

    static void copy(NDArray &target, vector<uint32_t> target_from,
                     const NDArray &source, vector<uint32_t> source_from, vector<uint32_t> source_to)
    {
        int i, k;
        T1 value;

        int last_index = source_from.size() - 1;
        int offset = target.dimensions.size() - source.dimensions.size();

        vector<uint32_t> source_tmp_indices = source_from;
        vector<uint32_t> target_tmp_indices = target_from;

        while (1)
        {
            while (source_tmp_indices[last_index] <= source_to[last_index])
            {
                value = source.data[source.computeFlatIndex(source_tmp_indices)];
                target.data[target.computeFlatIndex(target_tmp_indices)] = value;

                source_tmp_indices[last_index]++;
                target_tmp_indices[last_index + offset]++;
            }

            k = last_index - 1;
            while (k >= 0 && source_tmp_indices[k] == source_to[k])
                k--;
            if (k < 0)
                break;

            source_tmp_indices[k]++;
            target_tmp_indices[k + offset]++;
            k++;

            while (k <= last_index)
            {
                source_tmp_indices[k] = source_from[k];
                target_tmp_indices[k + offset] = target_from[k + offset];
                k++;
            }
        }
    }

    InfixExpression operator*(NDArray &right)
    {
        return InfixExpression(this, '*', &right);
    }

    NDArray operator=(InfixExpression expr)
    {
        if (expr.op == '*')
        {
            this->multiply(expr);
        }
        return *this;
    }

    void multiply(InfixExpression expr)
    {
        auto *left = expr.left;
        auto left_array = left->data.data();
        auto i = left->dimensions.size();
        auto left_matrix_rows = left->dimensions[i - 2];
        auto left_matrix_cols = left->dimensions[i - 1];
        auto left_matrix_len = left_matrix_rows * left_matrix_cols;

        auto *right = expr.right;
        auto right_array = right->data.data();
        i = right->dimensions.size();
        auto right_matrix_rows = right->dimensions[i - 2];
        auto right_matrix_cols = right->dimensions[i - 1];
        auto right_matrix_len = right_matrix_rows * right_matrix_cols;

        int number_of_matrices = 1;
        if (i > 2)
        {
            for (int j = 0; j < i - 2; j++)
                number_of_matrices *= left->dimensions[j];
        }

        // prefill the product array with 0.0
        for (auto &v : this->data)
            v = 0.0;

        auto product_array = this->data.data();
        auto product_matrix_len = left_matrix_rows * right_matrix_cols;

        auto product_matrix = product_array;
        auto left_matrix = left_array;
        auto right_matrix = right_array;

        int r, c, c1;
        for (int m = 0; m < number_of_matrices; m++)
        {
#pragma omp parallel for private(c, c1)
            for (r = 0; r < left_matrix_rows; r++)
            {
                for (c = 0; c < left_matrix_cols; c++)
                {
                    for (c1 = 0; c1 < right_matrix_cols; c1++)
                    {
                        product_matrix[r * right_matrix_cols + c1] += left_matrix[r * left_matrix_cols + c] * right_matrix[c * right_matrix_cols + c1];
                    }
                }
            }

            left_matrix += left_matrix_len;
            right_matrix += right_matrix_len;
            product_matrix += product_matrix_len;
        }
    }
};
// ------------------------------------------------------------------------------

class NDArrayNode
{
private:
    NDArray *ndArray;
    std::vector<uint32_t> indices;
    NDArrayNode(NDArray *, const std::vector<uint32_t> &);
    uint32_t computeFlatIndex() const;

public:
    NDArrayNode operator[](uint32_t);
    operator T1() const;
    T1 operator=(T1);

    NDArrayNode(const NDArrayNode &);
    NDArrayNode(NDArrayNode &&);
    NDArrayNode &operator=(const NDArrayNode &);
    NDArrayNode &operator=(NDArrayNode &&);
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
        oss << "Size of dimension cannot be zero: ";
        for (uint32_t index : _dimensions)
            oss << "[" << index << "]";

        throw NDArrayException(oss.str());
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
    this->data[flat_index] = value;
}

T1 NDArray::_get(uint32_t flat_index) const
{
    return this->data[flat_index];
}

uint32_t NDArray::computeFlatIndex(const std::vector<uint32_t> &indices) const
{
    if (indices.size() != this->dimensions.size())
    {
        ostringstream oss;
        oss << "Index out of bounds ";
        for (uint32_t index : indices)
            oss << "[" << index << "]";
        oss << ", Size of array is ";
        for (uint32_t dim : this->dimensions)
            oss << "[" << dim << "]";
        throw NDArrayException(oss.str());
    }

    uint64_t flat_index = 0;
    uint32_t stride = 1;

    for (int i = this->dimensions.size() - 1; i >= 0; --i)
    {
        if (indices[i] >= this->dimensions[i])
        {
            ostringstream oss;
            oss << "Index out of bounds";
            for (uint32_t index : indices)
                oss << "[" << index << "]";
            oss << ", Size of array is ";
            for (uint32_t dim : this->dimensions)
                oss << "[" << dim << "]";
            throw NDArrayException(oss.str());
        }

        flat_index += indices[i] * stride;
        stride *= this->dimensions[i];
    }

    return flat_index;
}

template <typename... TT>
void NDArray::set(TT... arguments)
{
    vector<uint32_t> indices;
    double value;

    (indices.push_back((uint32_t)arguments), ...);
    ((value = arguments), ...);

    indices.pop_back();

    this->data[this->computeFlatIndex(indices)] = value;
}

template <typename... TT>
T1 NDArray::get(TT... arguments) const
{
    vector<uint32_t> indices;
    (indices.push_back((uint32_t)arguments), ...);

    return this->data[this->computeFlatIndex(indices)];
}

NDArrayNode NDArray::operator[](uint32_t index)
{
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

NDArrayNode::NDArrayNode(const NDArrayNode &other)
{
    this->ndArray = other.ndArray;
    this->indices = other.indices;
    if (this->ndArray)
        this->ndArray->nodes.insert(this);
}

NDArrayNode::NDArrayNode(NDArrayNode &&other)
{
    this->ndArray = other.ndArray;
    this->indices = std::move(other.indices);

    if (this->ndArray)
    {
        this->ndArray->nodes.erase(&other);
        this->ndArray->nodes.insert(this);
    }
    // TODO::
    other.ndArray = NULL;
    other.indices.resize(0);
}

NDArrayNode &NDArrayNode::operator=(const NDArrayNode &other)
{
    if (this->ndArray == NULL)
    {
        throw NDArrayException("NDArrayNode is no longer valid");
    }

    if (other.ndArray == NULL)
    {
        throw NDArrayException("Assigned NDArrayNode is no longer valid");
    }

    this->ndArray->data[this->computeFlatIndex()] = other.ndArray->data[other.computeFlatIndex()];
    return *this;
}

NDArrayNode &NDArrayNode::operator=(NDArrayNode &&other)
{
    if (this->ndArray == NULL)
    {
        throw NDArrayException("NDArrayNode is no longer valid");
    }

    if (other.ndArray == NULL)
    {
        throw NDArrayException("Assigned NDArrayNode is no longer valid");
    }

    this->ndArray->data[this->computeFlatIndex()] = other.ndArray->data[other.computeFlatIndex()];
    return *this;
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
        throw NDArrayException("Invalid NDArrayNode, array does not exist");
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
        throw NDArrayException("Invalid NDArrayNode, array does not exist");
    }

    return this->ndArray->data[this->computeFlatIndex()];
}

T1 NDArrayNode::operator=(T1 value)
{
    if (this->ndArray == NULL)
    {
        throw NDArrayException("Invalid NDArrayNode, array does not exist");
    }

    this->ndArray->data[this->computeFlatIndex()] = value;
    return value;
}

uint32_t NDArrayNode::computeFlatIndex() const
{

    if (this->indices.size() != this->ndArray->dimensions.size())
    {
        ostringstream oss;
        oss << "Index out of bounds ";
        for (uint32_t index : this->indices)
            oss << "[" << index << "]";
        oss << ", Size of array is ";
        for (uint32_t dim : this->ndArray->dimensions)
            oss << "[" << dim << "]";
        throw NDArrayException(oss.str());
    }

    uint64_t flat_index = 0;
    uint32_t stride = 1;
    for (int i = this->ndArray->dimensions.size() - 1; i >= 0; --i)
    {
        if (this->indices[i] >= this->ndArray->dimensions[i])
        {
            ostringstream oss;
            oss << "Index out of bounds";
            for (uint32_t index : this->indices)
                oss << "[" << index << "]";
            oss << ", Size of array is ";
            for (uint32_t dim : this->ndArray->dimensions)
                oss << "[" << dim << "]";
            throw NDArrayException(oss.str());
        }

        flat_index += this->indices[i] * stride;
        stride *= this->ndArray->dimensions[i];
    }
    return flat_index;
}

// ------------------------------------------------------------------------------

int main()
{
    try
    {
        NDArray left_array({2, 4, 3});
        NDArray right_array({2, 3, 5});

        double value = 1.0;
        for (int i = 0; i < 2; i++)
        {
            value = 1.0;
            for (int j = 0; j < 4; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    left_array.set(i, j, k, value++);
                }
            }
        }

        for (int i = 0; i < 2; i++)
        {
            value = 10.0;
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 5; k++)
                {
                    right_array.set(i, j, k, value++);
                }
            }
        }

        NDArray product_array({2, 4, 5});
        product_array = left_array * right_array;

        product_array.print();
    }
    catch (const NDArrayException &e)
    {
        std::cerr << "NDArrayException: " << e.what() << std::endl;
    }

    return 0;
}