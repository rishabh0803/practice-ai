#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_SIZE 256

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <a> <b>\n", argv[0]);
        return 1;
    }

    char command[CMD_SIZE];
    snprintf(command, sizeof(command), "divide %s %s", argv[1], argv[2]);

    FILE *fp = popen(command, "r");
    if (fp == NULL)
    {
        perror("popen failed");
        return 2;
    }

    char buffer[128];
    if (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        printf("Result from divide: %s", buffer);
    }

    int status = pclose(fp);
    printf("pclose returned status: %d\n", status);

    // if (WIFEXITED(status))
    // {
    //     int exit_code = WEXITSTATUS(status);
    //     if (exit_code == 12)
    //     {
    //         fprintf(stderr, "Handled Error: Division by zero (exit code 12)\n");
    //         return 12;
    //     }
    //     else if (exit_code != 0)
    //     {
    //         fprintf(stderr, "divide exited with code %d\n", exit_code);
    //         return exit_code;
    //     }
    // }

    return 0;
}
