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
    ExpressionNode *rootNode;
    ExpressionNode *rightNode;

    const ExpressionNode *left;
    const ExpressionNode *right;
    const ExpressionNode *parent;

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
    this->rightNode = nullptr;
}

ExpressionNode::~ExpressionNode()
{
    delete this->rootNode;
    delete this->rightNode;
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
    traversePostOrder(&node);
    return *this;
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
    Number a(1), b(2), c;
    c = a + b;

    Number d(1), e(2), f(3), g;
    g = d + (e + f);

    Number h(1), i(2), j(3), k;
    k = h + i * j;

    Number l(1), m(2), n(3), o;
    o = (l + m) * n;

    Number p(1), q(2), r(3), s(4), t;
    t = (p + q) * (r + s);

    Number a1(1), b1(2), c1(3), d1(4), e1(5), f1(6), g1(7), h1(8), i1;
    i1 = a1 + b1 * (c1 + d1) * (e1 + f1) * g1 + h1;

    return 0;
}