#include <string.h>
#include <expression.h>

expression_pool expression::pool;

expression_pool::expression_pool()
{
    this->expressions = NULL;
    this->sz = 0;
    this->tail = 0;
}

expression_pool::~expression_pool()
{
    delete[] this->expressions;
}

expression::expression()
{
    this->left.ndArray = nullptr;
    this->right.ndArray = nullptr;
    this->oper = (char)0;
    this->flags = 0;
    this->result.value = 0;
}

expression &expression::operator+(NDArray &right)
{
    expression *expr = expression::get_expression();
    expr->left.expr = this;
    expr->right.ndArray = &right;
    expr->oper = '+';
    expr->flags = LEFT_IS_EXPRESSION | RIGHT_IS_NDARRAY;
    return *expr;
}

expression &expression::operator+(expression &right)
{
    expression *expr = expression::get_expression();
    expr->left.expr = this;
    expr->right.expr = &right;
    expr->oper = '+';
    expr->flags = LEFT_IS_EXPRESSION | RIGHT_IS_EXPRESSION;
    return *expr;
}

expression &expression::operator*(NDArray &right)
{
    expression *expr = expression::get_expression();
    expr->left.expr = this;
    expr->right.ndArray = &right;
    expr->oper = '*';
    expr->flags = LEFT_IS_EXPRESSION | RIGHT_IS_NDARRAY;
    return *expr;
}

expression &expression::operator*(expression &right)
{
    expression *expr = expression::get_expression();
    expr->left.expr = this;
    expr->right.expr = &right;
    expr->oper = '*';
    expr->flags = LEFT_IS_EXPRESSION | RIGHT_IS_EXPRESSION;
    return *expr;
}

expression *expression::get_expression()
{
    if (pool.expressions == NULL)
    {
        pool.expressions = new expression[1024];
        pool.sz = 1024;
        pool.tail = 0;
    }
    if (pool.tail == pool.sz) // no more to provide
    {
        expression *tmp = new expression[pool.sz + 1024];
        memcpy(tmp, pool.expressions, sizeof(expression) * pool.sz);
        free(pool.expressions);
        pool.expressions = tmp;
        pool.sz += 1024;
    }
    return &(pool.expressions[pool.tail++]);
}

void expression::reset()
{
    pool.tail = 0;
}
