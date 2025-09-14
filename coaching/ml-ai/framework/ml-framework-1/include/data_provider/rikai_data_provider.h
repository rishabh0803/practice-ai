#ifndef __RIKAI__DATA_PROVIDER__H__
#define __RIKAI__DATA_PROVIDER__H__

#include <rikai_error.h>
#include <rikai_matrix.h>

typedef struct __rikai_data_provider rikai_data_provider;

typedef struct __rikai_data_provider
{
    char *filename;
    FILE *file;
    uint32_t number_of_columns;
    uint8_t number_of_lines_in_header;
    void (*get_next_batch)(rikai_data_provider *data_provider, rikai_matrix *matrix, uint32_t *rows_provided, rikai_err *error);
} rikai_data_provider;

rikai_data_provider *rikai_data_provider_create_new(
    const char *filename,
    uint8_t number_of_lines_in_header,
    void (*get_next_batch)(rikai_data_provider *self, rikai_matrix *matrix, uint32_t *rows_provided, rikai_err *error),
    rikai_err *error);

void rikai_data_provider_destroy(rikai_data_provider *data_provider, rikai_err *error);

uint32_t rikai_data_provider_get_column_count(rikai_data_provider *data_provider, rikai_err *error);

void default_get_next_batch(rikai_data_provider *data_provider, rikai_matrix *matrix, uint32_t *rows_provided, rikai_err *error);

#endif