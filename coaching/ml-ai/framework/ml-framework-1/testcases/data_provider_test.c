#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <rikai.h>
#include <private_rikai.h>

typedef struct __rikai_data_provider rikai_data_provider;

typedef struct __rikai_data_provider
{
    char *filename;
    FILE *file;
    uint32_t number_of_columns;
    uint8_t number_of_lines_in_header;
    int (*get_next_batch)(rikai_data_provider *data_provider);
} rikai_data_provider;

rikai_data_provider *rikai_data_provider_create_new(
    const char *filename,
    uint8_t number_of_lines_in_header,
    int (*get_next_batch)(rikai_data_provider *self),
    rikai_err *error)
{
    rikai_data_provider *data_provider = NULL;
    if (error)
        rikai_clear_error(error);

    if (filename == NULL)
    {
        rikai_set_error(error, "Null pointer argument 1", RIKAI_NULL_POINTER);
        return NULL;
    }

    if (get_next_batch == NULL)
    {
        rikai_set_error(error, "Null pointer argument 4", RIKAI_NULL_POINTER);
        return NULL;
    }

    data_provider = (rikai_data_provider *)malloc(sizeof(rikai_data_provider));
    if (data_provider == NULL)
    {
        rikai_set_error(error, "Low memory", RIKAI_LOW_MEMORY);
        return NULL;
    }

    data_provider->number_of_lines_in_header = number_of_lines_in_header;
    data_provider->get_next_batch = get_next_batch;

    data_provider->filename = (char *)malloc(strlen(filename) + 1);
    if (data_provider->filename == NULL)
    {
        free(data_provider);
        rikai_set_error(error, "Low memory", RIKAI_LOW_MEMORY);
        return NULL;
    }

    strcpy(data_provider->filename, filename);
    data_provider->file = NULL;
    data_provider->number_of_columns = 0;

    return data_provider;
}

void rikai_data_provider_destroy(rikai_data_provider *data_provider, rikai_err *error)
{
    if (error)
        rikai_clear_error(error);
    if (data_provider == NULL)
    {
        rikai_set_error(error, "Null pointer", RIKAI_NULL_POINTER);
        return;
    }

    if (data_provider->filename)
        free(data_provider->filename);
    if (data_provider->file)
        fclose(data_provider->file);
    free(data_provider);
}

