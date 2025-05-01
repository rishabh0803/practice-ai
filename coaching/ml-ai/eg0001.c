#include <stdio.h>
#include <stdlib.h>

struct matrix
{
    int *x;
    int rows;
    int columns;
};

struct matrix *create_new(int rows, int columns)
{
    struct matrix *m;
    m = (struct matricx *)malloc(sizeof(struct matrix));
    m->x = (int *)malloc(rows * columns * sizeof(int));
    m->rows = rows;
    m->columns = columns;
    return m;
}

void _destroy(struct matrix **m)
{
    if (m == NULL)
        return;
    if (*m == NULL)
        return;
    free((*m)->x);
    free(*m);
    *m = NULL;
}
#define destroy(m) _destroy(&m)

void set(Struct matrix *m, int r, int c, int value)
{
    m->x[r * m->columns + c] = value;
}

int get(struct matrix *m, int r, int c)
{
    return m->x[r * m->columns + c];
}

void get_size(struct matrix *m, int *rows, int *columns)
{
    *rows = m->rows;
    *columns = m->columns;
}

// ==============================================================

int main()
{
    struct matrix *m = create_new(3, 4);
    set(m, 0, 0, 1);
    set(m, 0, 1, 2);
    set(m, 0, 2, 3);
    set(m, 0, 3, 4);
    set(m, 1, 0, 5);
    set(m, 1, 1, 6);
    set(m, 1, 2, 7);
    set(m, 1, 3, 8);
    set(m, 2, 0, 9);
    set(m, 2, 1, 10);
    set(m, 2, 2, 11);
    set(m, 2, 3, 12);

    int rows, columns;
    get_size(m, &rows, &columns);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            printf("%5d ", get(m, i, j));
            printf("\n");
        }
    }

    destroy(m);

    return 0;
}