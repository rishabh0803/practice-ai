#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <a> <b>\n", argv[0]);
        return 1;
    }

    int a = atoi(argv[1]);
    int b = atoi(argv[2]);

    if (b == 0) {
        fprintf(stderr, "Error: Division by zero\n");
        return 12;
    }

    double result = (double)a / b;
    printf("%f\n", result);
    return 0;
}
