#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_POINTS 10000    // Number of data points
#define SLOPE 45          // m in y = mx + c
#define INTERCEPT 0       // c in y = mx + c
#define NOISE_LEVEL 50    // Adjust this to control noise

int main() {
    FILE *fp;
    int i;
    double x, y, noise;

    // Seed the random number generator
    srand(time(NULL));

    // Open file to write
    fp = fopen("linear_data.csv", "w");
    if (fp == NULL) {
        perror("Unable to open file");
        return 1;
    }

    // Generate and write points
    for (i = 0; i < NUM_POINTS; i++) {
        x = i * 0.1;  // or use rand() for more randomness
        noise = ((double)rand() / RAND_MAX) * NOISE_LEVEL - NOISE_LEVEL / 2.0; // noise in range [-NOISE_LEVEL/2, +NOISE_LEVEL/2]
        y = SLOPE * x + INTERCEPT + noise;
        fprintf(fp, "%.4f,%.4f\n", x, y);
    }

    fclose(fp);
    printf("Data written to 'linear_data.csv'\n");

    return 0;
}
