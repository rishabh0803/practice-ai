#include <iostream>

using namespace std;

class Number;
typedef union _element
{
    char _operator;
    const Number *_operand;
} Element;

// ----------------------------- ExpressionNode Class -----------------------------
class ExpressionNode
{
public:
    Element element;
    Number *accumulator;

    ExpressionNode *rootNode;

    const ExpressionNode *left;
    const ExpressionNode *right;
    ExpressionNode *parent;

    ExpressionNode();
    ~ExpressionNode();

    ExpressionNode &operator+(ExpressionNode &right_operand);
    ExpressionNode &operator+(Number &right_operand);

    ExpressionNode &operator*(ExpressionNode &right_operand);
    ExpressionNode &operator*(Number &right_operand);
};

// ----------------------------- Number Class -----------------------------
class Number
{
private:
    int value;
    ExpressionNode rootNode;
    ExpressionNode leftNode;
    ExpressionNode rightNode;

public:
    Number();
    Number(int value);

    ExpressionNode &operator+(const Number &right_operand);
    ExpressionNode &operator+(ExpressionNode &right_operand);
    ExpressionNode &operator*(const Number &right_operand);
    ExpressionNode &operator*(ExpressionNode &right_operand);

    Number &operator=(ExpressionNode &node);
    int operator()() const;

    void traversePostOrder(const ExpressionNode *node);

    friend class ExpressionNode;
};

// ----------------------------- ExpressionNode Class Implementation -----------------------------
ExpressionNode::ExpressionNode()
{
    this->left = nullptr;
    this->right = nullptr;
    this->parent = nullptr;

    this->rootNode = nullptr;

    this->accumulator = nullptr;
}

ExpressionNode::~ExpressionNode()
{
    delete this->rootNode;
    delete this->accumulator;
}

ExpressionNode &ExpressionNode::operator+(Number &right_operand)
{
    cout << "ExpressionNode::operator+(Number &right_operand)" << endl;

    right_operand.rootNode.element._operator = '+';
    right_operand.rootNode.left = this;
    right_operand.rootNode.right = &right_operand.rightNode;
    this->parent = &right_operand.rootNode;

    right_operand.rightNode.element._operand = &right_operand;
    right_operand.rightNode.left = nullptr;
    right_operand.rightNode.right = nullptr;
    right_operand.rightNode.parent = &right_operand.rootNode;

    return right_operand.rootNode;
}

ExpressionNode &ExpressionNode::operator+(ExpressionNode &right_operand)
{
    cout << "ExpressionNode::operator+(ExpressionNode &right_operand)" << endl;

    if (right_operand.rootNode)
    {
        right_operand.rootNode->element._operator = '+';
        right_operand.rootNode->left = this;
        right_operand.rootNode->right = &right_operand;
        right_operand.rootNode->parent = nullptr;

        this->parent = right_operand.rootNode;
        right_operand.parent = right_operand.rootNode;

        return *(right_operand.rootNode);
    }
    else
    {
        this->rootNode = new ExpressionNode();
        this->rootNode->element._operator = '+';
        this->rootNode->left = this;
        this->rootNode->right = &right_operand;
        this->rootNode->parent = nullptr;

        this->parent = this->rootNode;
        right_operand.parent = this->rootNode;

        return *(this->rootNode);
    }
}

ExpressionNode &ExpressionNode::operator*(Number &right_operand)
{
    cout << "ExpressionNode::operator*(Number &right_operand)" << endl;
    right_operand.rootNode.element._operator = '*';
    right_operand.rootNode.left = this;
    right_operand.rootNode.right = &right_operand.rightNode;
    this->parent = &right_operand.rootNode;

    right_operand.rightNode.element._operand = &right_operand;
    right_operand.rightNode.left = nullptr;
    right_operand.rightNode.right = nullptr;
    right_operand.rightNode.parent = &right_operand.rootNode;

    return right_operand.rootNode;
}

ExpressionNode &ExpressionNode::operator*(ExpressionNode &right_operand)
{
    cout << "ExpressionNode::operator*(ExpressionNode &right_operand)" << endl;
    if (right_operand.rootNode)
    {
        right_operand.rootNode->element._operator = '*';
        right_operand.rootNode->left = this;
        right_operand.rootNode->right = &right_operand;
        right_operand.rootNode->parent = nullptr;

        this->parent = right_operand.rootNode;
        right_operand.parent = right_operand.rootNode;

        return *(right_operand.rootNode);
    }
    else
    {
        this->rootNode = new ExpressionNode();
        this->rootNode->element._operator = '*';
        this->rootNode->left = this;
        this->rootNode->right = &right_operand;
        this->rootNode->parent = nullptr;

        this->parent = this->rootNode;
        right_operand.parent = this->rootNode;

        return *(this->rootNode);
    }
}

// ----------------------------- Number Class Implementation -----------------------------
Number::Number()
{
    this->value = 0;
}

Number::Number(int value)
{
    this->value = value;
}

ExpressionNode &Number::operator+(const Number &right_operand)
{
    cout << "Number::operator+(const Number &right_operand)" << endl;
    this->rootNode.element._operator = '+';
    this->leftNode.element._operand = this;
    this->rightNode.element._operand = &right_operand;

    this->rootNode.left = &this->leftNode;
    this->rootNode.right = &this->rightNode;

    this->leftNode.parent = &this->rootNode;
    this->rightNode.parent = &this->rootNode;

    this->leftNode.left = nullptr;
    this->leftNode.right = nullptr;
    this->rightNode.left = nullptr;
    this->rightNode.right = nullptr;
    this->rootNode.parent = nullptr;

    return this->rootNode;
}

ExpressionNode &Number::operator+(ExpressionNode &right_operand)
{
    cout << "Number::operator+(const ExpressionNode &right_operand)" << endl;
    this->rootNode.element._operator = '+';
    this->leftNode.element._operand = this;

    this->rootNode.left = &this->leftNode;
    this->rootNode.right = &right_operand;

    this->leftNode.parent = &this->rootNode;
    right_operand.parent = &this->rootNode;

    this->leftNode.left = nullptr;
    this->leftNode.right = nullptr;
    this->rightNode.left = nullptr;
    this->rightNode.right = nullptr;
    this->rootNode.parent = nullptr;

    return this->rootNode;
}

ExpressionNode &Number::operator*(const Number &right_operand)
{
    cout << "Number::operator*(const Number &right_operand)" << endl;
    this->rootNode.element._operator = '*';
    this->leftNode.element._operand = this;
    this->rightNode.element._operand = &right_operand;

    this->rootNode.left = &this->leftNode;
    this->rootNode.right = &this->rightNode;

    this->leftNode.parent = &this->rootNode;
    this->rightNode.parent = &this->rootNode;

    this->leftNode.left = nullptr;
    this->leftNode.right = nullptr;
    this->rightNode.left = nullptr;
    this->rightNode.right = nullptr;
    this->rootNode.parent = nullptr;

    return this->rootNode;
}

ExpressionNode &Number::operator*(ExpressionNode &right_operand)
{
    cout << "Number::operator*(ExpressionNode &right_operand)" << endl;
    this->rootNode.element._operator = '*';
    this->leftNode.element._operand = this;

    this->rootNode.left = &this->leftNode;
    this->rootNode.right = &right_operand;

    this->leftNode.parent = &this->rootNode;
    right_operand.parent = &this->rootNode;

    this->leftNode.left = nullptr;
    this->leftNode.right = nullptr;
    this->rightNode.left = nullptr;
    this->rightNode.right = nullptr;
    this->rootNode.parent = nullptr;

    return this->rootNode;
}

Number &Number::operator=(ExpressionNode &node)
{
    cout << "Number::operator=(ExpressionNode &node)" << endl;
    // traversePostOrder(&node);
    // return *this;

    char oper;

    ExpressionNode *parent;
    const ExpressionNode *left_sibling;
    const ExpressionNode *right_sibling;

    int left_num;
    int right_num;

    const ExpressionNode *curr = &node;
    while (!(node.left == nullptr && node.right == nullptr))
    {
        if (curr->left == nullptr && curr->right == nullptr)
        {
            // curr is pointing to a leaf node, so we can evaluate the parent node
            parent = curr->parent;
            left_sibling = parent->left;
            right_sibling = parent->right;

            if (left_sibling->left == nullptr && left_sibling->right == nullptr &&
                right_sibling->left == nullptr && right_sibling->right == nullptr)
            {
                // both left_sibling and right_sibling are leaf nodes, so we can evaluate the parent node

                left_num = left_sibling->element._operand->value;
                right_num = right_sibling->element._operand->value;
                oper = parent->element._operator;

                if (parent->accumulator == nullptr)
                    parent->accumulator = new Number();

                if (oper == '+')
                    parent->accumulator->value = left_num + right_num;
                else if (oper == '*')
                    parent->accumulator->value = left_num * right_num;
                else if (oper == '-')
                    parent->accumulator->value = left_num - right_num;
                else if (oper == '/')
                    parent->accumulator->value = left_num / right_num;
                else
                    parent->accumulator->value = 0;

                parent->element._operand = parent->accumulator;
                parent->left = nullptr;
                parent->right = nullptr;

                curr = parent;
            }
            else
            {
                // curr is pointing to a leaf node,
                // but either left_sibling or right_sibling is not a leaf node,
                // move to node that is not a leaf node to resolve it first
                curr = (parent->left == curr) ? right_sibling : left_sibling;
            }
        }
        else
        {
            // curr is not pointing to a leaf node, so we need to traverse down the tree
            if (curr->left == nullptr)
                curr = curr->right;
            else
                curr = curr->left;
        }
    }

    this->value = node.element._operand->value;
    return *this;
}

int Number::operator()() const
{
    return this->value;
}

void Number::traversePostOrder(const ExpressionNode *node)
{
    if (!node)
        return;

    traversePostOrder(node->left);
    traversePostOrder(node->right);

    if (node->left == nullptr && node->right == nullptr)
        cout << "Operand : " << node->element._operand->value << endl;
    else
        cout << "Operator : " << node->element._operator << endl;
}

// ----------------------------- main -----------------------------
int main()
{
    Number a(1), b(2), c(3), d(4), e(5), f(6), g(7), h(8), i;

    cout << "Expression: a + b" << endl;
    i = a + b;
    cout << "Result: " << i() << endl;
    cout << "Expected: " << (1 + 2) << endl;

    cout << "Expression: d + (e + f)" << endl;
    i = d + (e + f);
    cout << "Result: " << i() << endl;
    cout << "Expected: " << (4 + (5 + 6)) << endl;

    cout << "Expression: a + b * c" << endl;
    i = a + b * c;
    cout << "Result: " << i() << endl;
    cout << "Expected: " << (1 + 2 * 3) << endl;

    cout << "Expression: (a + b) * c" << endl;
    i = (a + b) * c;
    cout << "Result: " << i() << endl;
    cout << "Expected: " << ((1 + 2) * 3) << endl;

    cout << "Expression: (a + b) * (c + d)" << endl;
    i = (a + b) * (c + d);
    cout << "Result: " << i() << endl;
    cout << "Expected: " << ((1 + 2) * (3 + 4)) << endl;

    cout << "Expression: a + b * (c + d) * (e + f) * g + h" << endl;
    i = a + b * (c + d) * (e + f) * g + h;
    cout << "Result: " << i() << endl;
    cout << "Expected: " << (1 + 2 * (3 + 4) * (5 + 6) * 7 + 8) << endl;

    return 0;
}