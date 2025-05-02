#include <rikai.h>
#include <stdlib.h>
#include <string.h>

typedef struct __rikai_err
{
    uint16_t fallback_error_code;
    char *error_str;
} rikai_err;

rikai_err *rikai_error_create_new()
{
    rikai_err *error;
    error = (rikai_err *)malloc(sizeof(rikai_err));
    if (error == NULL)
        return NULL;
    error->error_str = NULL;
    error->fallback_error_code = 0;
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

uint8_t rikai_has_error(const rikai_err *err)
{
    if (!err)
        return 0;
    if (err->error_str == NULL && err->fallback_error_code == 0)
        return 0;
    return 1;
}

uint32_t rikai_get_error_len(const rikai_err *err)
{
    if (!err)
        return 0;
    if (err->error_str == NULL && err->fallback_error_code == 0)
        return 0;
    if (err->error_str)
        return strlen(err->error_str);
    return 5;
}

void rikai_get_error(char *str, uint32_t size, const rikai_err *err)
{
    unsigned int i;
    unsigned int end_point;
    if (str && size > 0)
        *str = '\0';
    if (!err || !str || size == 0)
        return;
    if (err->error_str == NULL)
        return;

    if (err->error_str != NULL)
    {
        end_point = size - 1;

        for (i = 0; i < end_point && err->error_str[i] != '\0'; ++i)
        {
            str[i] = err->error_str[i];
        }
        str[i] = '\0';
    }
    else
    {
        sprintf(str, "%u", err->fallback_error_code);
    }
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
    err->fallback_error_code = 0;
}

void rikai_set_error(rikai_err *err, const char *str, uint16_t fallback_error_code)
{
    if (!err)
        return;
    if (rikai_has_error(err))
        rikai_clear_error(err);

    err->fallback_error_code = fallback_error_code;
    err->error_str = (char *)malloc(strlen(str) + 1);
    if (err->error_str == NULL)
        return;
    strcpy(err->error_str, str);
}

int main()
{
    return 0;
}