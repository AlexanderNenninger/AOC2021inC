#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "io.h"

// Read file into string.
char *readfile(char *filename)
{
    char *buffer = NULL;
    int string_size, read_size;
    FILE *handler = fopen(filename, "r");
    if (handler)
    {
        fseek(handler, 0, SEEK_END);
        string_size = ftell(handler);
        rewind(handler);

        buffer = (char *)malloc(sizeof(char) * (string_size + 1));

        read_size = fread(buffer, sizeof(char), string_size, handler);

        buffer[string_size] = '\0';

        if (string_size != read_size)
        {
            free(buffer);
            buffer = NULL;
        }

        fclose(handler);
    }
    return buffer;
}

// Count blocks in string separated by `sep`, e.g. '\n'
int count_items(char *str, char sep)
{
    int lines = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == sep)
        {
            lines++;
        }
    }
    return lines + 1;
}

// Greadily parse numbers from string.
int parse_ints(char *str, int *ibuf, char sep)
{
    int len = count_items(str, sep);

    char *endptr;
    int i;
    for (i = 0; i < len; i++)
    {
        errno = 0;
        int result = strtol(str, &endptr, 10);
        if (errno != 0)
        {
            break;
        }

        ibuf[i] = result;
        str = endptr + 1;
    }

    return i;
}