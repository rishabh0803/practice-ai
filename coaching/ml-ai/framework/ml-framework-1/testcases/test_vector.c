#include <stdio.h>
#include <rikai.h>

int main()
{
    rikai_err *error = NULL;
    rikai_vector *vector = NULL;
    char error_message[501];
    double value;
    uint32_t size;

    error = rikai_error_create_new();
    if (error == NULL)
    {
        printf("Error: Out of memory\n");
        return -1;
    }

    vector = rikai_vector_create_new(5, error);
    if (rikai_has_error(error))
    {
        rikai_get_error(error_message, 501, error);
        printf("%s\n", error_message);
        rikai_error_destroy(error);
        return -1;
    }

    rikai_vector_set(vector, 0, 1.0, error);
    rikai_vector_set(vector, 1, 2.0, error);
    rikai_vector_set(vector, 2, 3.0, error);
    rikai_vector_set(vector, 3, 4.0, error);
    rikai_vector_set(vector, 4, 5.0, error);

    size = rikai_vector_get_size(vector, error);
    printf("Size of vector: %u\n", size);

    for (uint32_t i = 0; i < size; i++)
    {
        value = rikai_vector_get(vector, i, error);
        printf("%lf  ", value);
    }

    rikai_vector_destory(vector, error);
    rikai_error_destroy(error);

    return 0;
}