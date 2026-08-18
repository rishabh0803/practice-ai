#include <iostream>
#include <cstdint>

using namespace std;

class Number;
class Expression;
enum ExpressionType
{
    LEFT_IS_NUMBER = 1,
    RIGHT_IS_NUMBER = 2,
    LEFT_IS_EXPRESSION = 4,
    RIGHT_IS_EXPRESSION = 8,
};

union _operand
{
    const Number *ptr2Number;
    const Expression *ptr2Expression;
};

class ExpressionVector
{
public:
    Expression *expressions;
    uint32_t expressionCount;
    uint32_t head;
    uint32_t tail;
    ExpressionVector();
    ~ExpressionVector();
};

class Expression
{
public:
    _operand left;
    _operand right;
    char oper;

    Number *result;
    uint32_t flags;

    thread_local static ExpressionVector expressionVector;

    Expression();
    ~Expression();

    Expression &operator+(const Number &right_operand);
    Expression &operator+(const Expression &right_operand);
    Expression &operator*(const Number &right_operand);
    Expression &operator*(const Expression &right_operand);
};
thread_local ExpressionVector Expression::expressionVector;

class Number
{
private:
    int value;

public:
    Number();
    Number(int value);

    int operator()() const;

    Expression &operator+(const Number &right_operand);
    Expression &operator+(const Expression &right_operand);
    Expression &operator*(const Number &right_operand);
    Expression &operator*(const Expression &right_operand);

    Number &operator=(Expression &list);

    friend class Expression;
};

//----------------------------- ExpressionVector Class Implementation -----------------------------
ExpressionVector::ExpressionVector()
{
    this->expressions = new Expression[1024];
    this->expressionCount = 1024;
    this->head = 0;
    this->tail = 0;
}

ExpressionVector::~ExpressionVector()
{
    delete[] this->expressions;
}

// ----------------------------- Expression Class Implementation -----------------------------
Expression::Expression() : result(nullptr), flags(0)
{
    this->left.ptr2Number = nullptr;
    this->right.ptr2Number = nullptr;
}

Expression::~Expression()
{
    delete this->result;
    delete this->left.ptr2Expression;
    delete this->right.ptr2Expression;
}

Expression &Expression::operator+(const Number &right_operand)
{
    cout << "Expression::operator+(const Number &right_operand)" << endl;
    if (Expression::expressionVector.tail == Expression::expressionVector.expressionCount)
    {
        // TODO :: code to resize the expression vector if needed
        throw std::runtime_error("Expression vector is full");
    }

    Expression *expr;
    expr = Expression::expressionVector.expressions + Expression::expressionVector.tail;

    expr->left.ptr2Expression = this;
    expr->right.ptr2Number = &right_operand;
    expr->oper = '+';
    expr->flags = LEFT_IS_EXPRESSION | RIGHT_IS_NUMBER;

    Expression::expressionVector.tail++;
    return *expr;
}

Expression &Expression::operator*(const Expression &right_operand)
{
    cout << "Expression::operator*(const Expression &right_operand)" << endl;
    if (Expression::expressionVector.tail == Expression::expressionVector.expressionCount)
    {
        // TODO :: code to resize the expression vector if needed
        throw std::runtime_error("Expression vector is full");
    }

    Expression *expr;
    expr = Expression::expressionVector.expressions + Expression::expressionVector.tail;

    expr->left.ptr2Expression = this;
    expr->right.ptr2Expression = &right_operand;
    expr->oper = '*';
    expr->flags = LEFT_IS_EXPRESSION | RIGHT_IS_EXPRESSION;

    Expression::expressionVector.tail++;
    return *expr;
}

Expression &Expression::operator*(const Number &right_operand)
{
    cout << "Expression::operator*(const Number &right_operand)" << endl;
    if (Expression::expressionVector.tail == Expression::expressionVector.expressionCount)
    {
        // TODO :: code to resize the expression vector if needed
        throw std::runtime_error("Expression vector is full");
    }

    Expression *expr;
    expr = Expression::expressionVector.expressions + Expression::expressionVector.tail;

    expr->left.ptr2Expression = this;
    expr->right.ptr2Number = &right_operand;
    expr->oper = '*';
    expr->flags = LEFT_IS_EXPRESSION | RIGHT_IS_NUMBER;

    Expression::expressionVector.tail++;
    return *expr;
}

Expression &Expression::operator+(const Expression &right_operand)
{
    cout << "Expression::operator+(const Expression &right_operand)" << endl;
    if (Expression::expressionVector.tail == Expression::expressionVector.expressionCount)
    {
        // TODO :: code to resize the expression vector if needed
        throw std::runtime_error("Expression vector is full");
    }

    Expression *expr;
    expr = Expression::expressionVector.expressions + Expression::expressionVector.tail;

    expr->left.ptr2Expression = this;
    expr->right.ptr2Expression = &right_operand;
    expr->oper = '+';
    expr->flags = LEFT_IS_EXPRESSION | RIGHT_IS_EXPRESSION;

    Expression::expressionVector.tail++;
    return *expr;
}

// ---------------------------- Number Class Implementation -----------------------------
Number::Number() : value(0) {}

Number::Number(int value) : value(value) {}

int Number::operator()() const
{
    return value;
}

Expression &Number::operator+(const Number &right_operand)
{
    cout << "Number::operator+(const Number &right_operand)" << endl;
    if (Expression::expressionVector.tail == Expression::expressionVector.expressionCount)
    {
        // TODO :: code to resize the expression vector if needed
        throw std::runtime_error("Expression vector is full");
    }

    Expression *expr;
    expr = Expression::expressionVector.expressions + Expression::expressionVector.tail;

    expr->left.ptr2Number = this;
    expr->right.ptr2Number = &right_operand;
    expr->oper = '+';
    expr->flags = LEFT_IS_NUMBER | RIGHT_IS_NUMBER;

    Expression::expressionVector.tail++;
    return *expr;
}

Expression &Number::operator+(const Expression &right_operand)
{
    cout << "Number::operator+(const Expression &right_operand)" << endl;
    if (Expression::expressionVector.tail == Expression::expressionVector.expressionCount)
    {
        // TODO :: code to resize the expression vector if needed
        throw std::runtime_error("Expression vector is full");
    }

    Expression *expr;
    expr = Expression::expressionVector.expressions + Expression::expressionVector.tail;

    expr->left.ptr2Number = this;
    expr->right.ptr2Expression = &right_operand;
    expr->oper = '+';
    expr->flags = LEFT_IS_NUMBER | RIGHT_IS_EXPRESSION;

    Expression::expressionVector.tail++;
    return *expr;
}

Expression &Number::operator*(const Number &right_operand)
{
    cout << "Number::operator*(const Number &right_operand)" << endl;
    if (Expression::expressionVector.tail == Expression::expressionVector.expressionCount)
    {
        // TODO :: code to resize the expression vector if needed
        throw std::runtime_error("Expression vector is full");
    }

    Expression *expr;
    expr = Expression::expressionVector.expressions + Expression::expressionVector.tail;

    expr->left.ptr2Number = this;
    expr->right.ptr2Number = &right_operand;
    expr->oper = '*';
    expr->flags = LEFT_IS_NUMBER | RIGHT_IS_NUMBER;

    Expression::expressionVector.tail++;
    return *expr;
}

Expression &Number::operator*(const Expression &right_operand)
{
    cout << "Number::operator*(const Expression &right_operand)" << endl;
    if (Expression::expressionVector.tail == Expression::expressionVector.expressionCount)
    {
        // TODO :: code to resize the expression vector if needed
        throw std::runtime_error("Expression vector is full");
    }

    Expression *expr;
    expr = Expression::expressionVector.expressions + Expression::expressionVector.tail;

    expr->left.ptr2Number = this;
    expr->right.ptr2Expression = &right_operand;
    expr->oper = '*';
    expr->flags = LEFT_IS_NUMBER | RIGHT_IS_EXPRESSION;

    Expression::expressionVector.tail++;
    return *expr;
}

Number &Number::operator=(Expression &list)
{
    if (Expression::expressionVector.head == Expression::expressionVector.tail)
    {
        return *this;
    }

    uint32_t index;
    int left_num;
    int right_num;
    Expression *t;

    index = Expression::expressionVector.head;
    while (index != Expression::expressionVector.tail)
    {
        t = Expression::expressionVector.expressions + index;

        if (t->result == nullptr)
            t->result = new Number();

        if (t->flags & LEFT_IS_NUMBER && t->flags & RIGHT_IS_NUMBER)
        {
            left_num = t->left.ptr2Number->value;
            right_num = t->right.ptr2Number->value;
        }
        else if (t->flags & LEFT_IS_NUMBER && t->flags & RIGHT_IS_EXPRESSION)
        {
            left_num = t->left.ptr2Number->value;
            right_num = t->right.ptr2Expression->result->value;
        }
        else if (t->flags & LEFT_IS_EXPRESSION && t->flags & RIGHT_IS_NUMBER)
        {
            left_num = t->left.ptr2Expression->result->value;
            right_num = t->right.ptr2Number->value;
        }
        else if (t->flags & LEFT_IS_EXPRESSION && t->flags & RIGHT_IS_EXPRESSION)
        {
            left_num = t->left.ptr2Expression->result->value;
            right_num = t->right.ptr2Expression->result->value;
        }

        switch (t->oper)
        {
        case '+':
            t->result->value = left_num + right_num;
            break;
        case '-':
            t->result->value = left_num - right_num;
            break;
        case '*':
            t->result->value = left_num * right_num;
            break;
        case '/':
            t->result->value = left_num / right_num;
            break;
        default:
            t->result->value = 0;
        }

        t->left.ptr2Number = nullptr;
        t->right.ptr2Number = nullptr;

        index++;
    }

    this->value = t->result->value;
    Expression::expressionVector.head = 0;
    Expression::expressionVector.tail = 0;

    return *this;
}

// ---------------------------- main function -----------------------------
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

    return EXIT_SUCCESS;
}