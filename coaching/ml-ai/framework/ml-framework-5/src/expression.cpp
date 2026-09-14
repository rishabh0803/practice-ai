#include <string.h>
#include <expression.h>

expression::expression()
{
    this->left.ndArray = nullptr;
    this->right.ndArray = nullptr;
    this->oper = (char)0;
    this->flags = 0;
    this->result.value = 0;
    this->next = NULL;
}

expression *expression::get_expression()
{
    if (expressions == NULL)
    {
        expressions = new expression[1024];
        sz = 1024;
        tail = 0;
    }
    if (tail == sz) // no more to provide
    {
        expression *tmp = new expression[sz + 1024];
        memcpy(tmp, expressions, sizeof(expression) * sz);
        free(expressions);
        expressions = tmp;
        sz += 1024;
    }
    return &(expressions[tail++]);
}
