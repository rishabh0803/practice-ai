#include <stdio.h>

// Write program to generate a csv file with 100 rows and 2 columns
// first column increases from 1 to 100
// second column is 1 if first column is > 50 else 0
// save the file as test-log.csv
int main()
{
    FILE *file = fopen("test-log.csv", "w");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    for (int i = 1; i <= 100; i++)
    {
        fprintf(file, "%d,%d\n", i, (i > 50) ? 1 : 0);
    }

    fclose(file);
    return 0;
}