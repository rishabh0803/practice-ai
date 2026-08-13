/*
 * Code discussed in the next lecture.
 * This code is a more advanced version of the previous one.
 * We added the expression tree to evaluate the expression and return the result.
 * Currently we are using stack/recursion to evaluate the expression tree.
 * Plan is to avoid additional data structures.
 *
 * Problems in current implementation:
 * 1. Unable to handle operation like a = c * b + d; because we need to avoid new operator
 * to add operator overloadding in ExpresisonNode class we need to either its own object
 * (not possible will create cyclic dependency) or
 * we need to use pointer to expressionNode and use new to allocate memory
 *
 * 2. We are using recursion to evaluate the expression tree,
 * which is not a good idea because it will create stack overflow for large expression trees.
 */
#include <iostream>

using namespace std;

// ----------------------------- Expression node class -----------------------------
class ExpressionNode
{
public:
    void *element;
    ExpressionNode *left;
    ExpressionNode *right;

    ExpressionNode()
    {
        this->element = nullptr;
        this->left = nullptr;
        this->right = nullptr;
    }

    void reset()
    {
        this->element = nullptr;
        this->left = nullptr;
        this->right = nullptr;
    }
};

// ----------------------------- Number Class -----------------------------
class Number
{
private:
    int num;
    ExpressionNode root_node;
    ExpressionNode left_node;
    ExpressionNode right_node;

    int resolve(ExpressionNode &node)
    {
        if (node.left == nullptr && node.right == nullptr)
        {
            Number *number = static_cast<Number *>(node.element);
            return number->num;
        }

        char operation = *static_cast<const char *>(node.element);
        int left_value = resolve(*node.left);
        int right_value = resolve(*node.right);
        int result;

        switch (operation)
        {
        case '+':
            result = left_value + right_value;
            break;
        case '*':
            result = left_value * right_value;
            break;
        case '-':
            result = left_value - right_value;
            break;
        case '/':
            if (right_value == 0)
            {
                throw invalid_argument("Division by zero");
            }
            result = left_value / right_value;
            break;
        default:
            throw invalid_argument("Unsupported operator");
        }

        node.reset();
        this->num = result;
        node.element = static_cast<void *>(this);

        return result;
    }

public:
    Number()
    {
        this->num = 0;
    }

    Number(int num)
    {
        this->num = num;
    }

    Number(Number &other)
    {
        this->num = other.num;
    }

    Number(Number &&other)
    {
        this->num = other.num;
        other.num = 0;
    }

    Number &operator=(Number &other)
    {
        this->num = other.num;
        return *this;
    }

    Number &operator=(Number &&other)
    {
        this->num = other.num;
        other.num = 0;
        return *this;
    }

    ~Number()
    {
    }

    ExpressionNode &operator*(Number &right_operand)
    {
        this->root_node.reset();
        this->left_node.reset();
        this->right_node.reset();

        this->root_node.element = (void *)"*";
        this->left_node.element = (void *)this;
        this->right_node.element = (void *)&right_operand;

        this->root_node.left = &this->left_node;
        this->root_node.right = &this->right_node;

        return this->root_node;
    }

    ExpressionNode &operator*(ExpressionNode &right_operand)
    {
        this->root_node.reset();
        this->left_node.reset();

        this->root_node.element = (void *)"*";
        this->left_node.element = (void *)this;

        this->root_node.left = &this->left_node;
        this->root_node.right = &right_operand;

        return this->root_node;
    }

    ExpressionNode &operator+(Number &right_operand)
    {
        this->root_node.reset();
        this->left_node.reset();
        this->right_node.reset();

        this->root_node.element = (void *)"+";
        this->left_node.element = (void *)this;
        this->right_node.element = (void *)&right_operand;

        this->root_node.left = &this->left_node;
        this->root_node.right = &this->right_node;

        return this->root_node;
    }

    ExpressionNode &operator+(ExpressionNode &right_operand)
    {
        this->root_node.reset();
        this->left_node.reset();

        this->root_node.element = (void *)"+";
        this->left_node.element = (void *)this;

        this->root_node.left = &this->left_node;
        this->root_node.right = &right_operand;

        return this->root_node;
    }

    Number &operator=(ExpressionNode &node)
    {
        this->num = resolve(node);

        node.reset();
        this->root_node.reset();
        this->left_node.reset();
        this->right_node.reset();

        return *this;
    }

    int operator()() const
    {
        return this->num;
    }
};

int main()
{
    Number a;
    Number b(10);
    Number c(30);
    Number d(40);

    a = b + c * d;
    cout << a() << endl;

    a = c + b * d;
    cout << a() << endl;

    // Won't compile
    // a = c * b + d;
    // cout << a() << endl;
    return 0;
}