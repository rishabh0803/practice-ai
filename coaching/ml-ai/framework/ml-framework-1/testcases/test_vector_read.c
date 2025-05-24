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

    vector = rikai_vector_read_csv("vector.csv", 0, error);

    if (rikai_has_error(error))
    {
        rikai_get_error(error_message, 501, error);
        printf("%s\n", error_message);
        rikai_vector_destroy(vector, error);
        rikai_error_destroy(error);
        return -1;
    }

    rikai_vector_print(stdout, vector, error);

    rikai_vector_destroy(vector, error);
    rikai_error_destroy(error);

    return 0;
}