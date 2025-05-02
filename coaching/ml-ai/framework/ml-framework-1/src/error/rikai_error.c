#include <rikai.h>
#include <stdlib.h>
#include <string.h>

typedef struct __rikai_err
{
    char *error_str;
} rikai_err;

rikai_err *rikai_error_create_new()
{
    rikai_err *error;
    error = (rikai_err *)malloc(sizeof(rikai_err));
    if (error == NULL)
        return NULL;
    error->error_str = NULL;
    return error;
}

void __rikai_error_destroy(rikai_err **err)
{
    if (err == NULL || *err == NULL)
        return;
    if ((*err)->error_str)
        free((*err)->error_str);
    free(*err);
    *err = NULL;
}

unsigned char rikai_has_error(rikai_err *err)
{
    if (!err)
        return 0;
    return (err->error_str != NULL);
}

void rikai_get_error(char *str, unsigned int size, rikai_err *err)
{
    unsigned int i;
    unsigned int end_point;
    if (str && size > 0)
        *str = '\0';
    if (!err || !str || size == 0)
        return;
    if (err->error_str == NULL)
        return;
    end_point = size - 1;

    for (i = 0; i < end_point && err->error_str[i] != '\0'; ++i)
    {
        str[i] = err->error_str[i];
    }
    str[i] = '\0';
}

void rikai_clear_error(rikai_err *err)
{
    if (!err)
        return;
    if (err->error_str)
    {
        free(err->error_str);
        err->error_str = NULL;
    }
}

void rikai_set_error(rikai_err *err, const char *str)
{
    if (!err)
        return;
    if (rikai_has_error(err))
        rikai_clear_error(err);

    err->error_str = (char *)malloc(strlen(str) + 1);
    if (err->error_str == NULL)
        return;
    strcpy(err->error_str, str);
}

int main()
{
    return 0;
}