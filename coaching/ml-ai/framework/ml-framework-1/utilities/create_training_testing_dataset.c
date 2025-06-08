#include <rikai.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    uint32_t r, c;
    uint32_t rows, columns;
    char *filename;
    double n, d;

    char *input_csv, *training_csv, *testing_csv;
    FILE *input_dataset_file, *training_csv_file, *testing_csv_file;
    int skip_lines, training_percentage;
    int input_csv_rows, i, upper_bound, random_index;
    uint32_t *array1, *array2;
    char m;
    int divider;
    double temp;

    if (argc != 6)
    {
        fprintf(stderr, "Usage: %s <input_dataset> <skip_lines> <training_dataset> <testing_dataset> <training_dataset_percentage>\n", argv[0]);
        return EXIT_FAILURE;
    }

    input_csv = argv[1];
    skip_lines = atoi(argv[2]);
    training_csv = argv[3];
    testing_csv = argv[4];
    training_percentage = atoi(argv[5]);

    input_dataset_file = fopen(input_csv, "r");
    if (input_dataset_file == NULL)
    {
        fprintf(stderr, "Error opening input dataset file: %s\n", input_csv);
        return EXIT_FAILURE;
    }

    input_csv_rows = 0;
    while (1)
    {
        m = fgetc(input_dataset_file);
        if (feof(input_dataset_file))
            break;
        if (m == '\n')
            input_csv_rows++;
    }

    input_csv_rows -= skip_lines; // Adjust for skipped lines
    rewind(input_dataset_file);

    array1 = (uint32_t *)malloc(input_csv_rows * sizeof(uint32_t));
    if (array1 == NULL)
    {
        fprintf(stderr, "Out of memory\n");
        fclose(input_dataset_file);
        return EXIT_FAILURE;
    }

    array2 = (uint32_t *)malloc(input_csv_rows * sizeof(uint32_t));
    if (array2 == NULL)
    {
        fprintf(stderr, "Out of memory\n");
        fclose(input_dataset_file);
        return EXIT_FAILURE;
    }

    // initialize the array1 with indices
    for (i = 0; i < input_csv_rows; i++)
        array1[i] = i;

    // Shuffle the array1
    srand(time(NULL));
    upper_bound = input_csv_rows - 1;
    while (upper_bound > 10)
    {
        random_index = rand() % upper_bound;
        temp = array1[upper_bound];
        array1[upper_bound] = array1[random_index];
        array1[random_index] = temp;
        upper_bound--;
    }

    for (i = 0; i < input_csv_rows; i++)
        array2[array1[i]] = i;

    while (skip_lines > 0)
    {
        m = fgetc(input_dataset_file);
        if (m == '\n')
            skip_lines--;
    }

    training_csv_file = fopen(training_csv, "w");
    if (training_csv_file == NULL)
    {
        fprintf(stderr, "Error opening training dataset file: %s\n", training_csv);
        fclose(input_dataset_file);
        free(array1);
        free(array2);
        return EXIT_FAILURE;
    }

    testing_csv_file = fopen(testing_csv, "w");
    if (testing_csv_file == NULL)
    {
        fprintf(stderr, "Error opening testing dataset file: %s\n", testing_csv);
        fclose(input_dataset_file);
        fclose(training_csv_file);
        free(array1);
        free(array2);
        return EXIT_FAILURE;
    }

    divider = (input_csv_rows * training_percentage) / 100;
    divider--;

    int index = 0;
    while (1)
    {
        m = fgetc(input_dataset_file);
        if (feof(input_dataset_file))
            break;

        if (m == '\n')
        {
            if (array2[index] <= divider)
                fputc('\n', training_csv_file);
            else
                fputc('\n', testing_csv_file);
            index++;
            continue;
        }

        if (array2[index] <= divider)
            fputc(m, training_csv_file);
        else
            fputc(m, testing_csv_file);
    }

    fclose(input_dataset_file);
    fclose(training_csv_file);
    fclose(testing_csv_file);
    free(array1);
    free(array2);

    printf("Training dataset created: %s\n", training_csv);
    printf("Testing dataset created: %s\n", testing_csv);

    printf("Total rows in input dataset: %d\n", input_csv_rows);
    printf("Training dataset rows: %d\n", (input_csv_rows * training_percentage) / 100);
    printf("Testing dataset rows: %d\n", input_csv_rows - (input_csv_rows * training_percentage) / 100);
    printf("Skip lines: %d\n", skip_lines);

    return 0;
}