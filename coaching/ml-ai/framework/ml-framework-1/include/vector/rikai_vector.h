#ifndef __RIKAI__VECTOR__H__
#define __RIKAI__VECTOR__H__

typedef struct __rikai_vector rikai_vector;

rikai_vector *rikai_vector_create_new(uint32_t size, rikai_err *error);

void __rikai_vector_destroy(rikai_vector **vector, rikai_err *error);
#define rikai_vector_destroy(vector, error) __rikai_vector_destroy(&vector, error)

void rikai_vector_set(rikai_vector *vector, uint32_t index, double value, rikai_err *error);

double rikai_vector_get(const rikai_vector *vector, uint32_t index, rikai_err *error);

uint32_t rikai_vector_get_size(const rikai_vector *vector, rikai_err *error);

void rikai_vector_write(const rikai_vector *vector, const char *filename, rikai_err *error);

rikai_vector *rikai_vector_read(const char *filename, rikai_err *error);

rikai_vector *rikai_vector_read_csv(const char *filename, int skip_lines, rikai_err *error);

void rikai_vector_print(FILE *file, const rikai_vector *vector, rikai_err *error);

#endif
