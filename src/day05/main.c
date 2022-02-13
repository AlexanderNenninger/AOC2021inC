#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include "../lib/io.h"
#include "../lib/utils.h"
#include "../lib/hashmap/hashmap.h"

extern int errno;

// We need to store points in a hashmap to add up lines.
typedef struct Point
{
    int x;
    int y;
    int *count;
} Point_t;

int point_compare(const void *a, const void *b, void *pdata)
{
    const Point_t *pa = a;
    const Point_t *pb = b;
    return pa->x == pb->x && pa->y == pb->y && pa->count == pb->count - 1;
}

bool point_iter(const void *item, void *pdata)
{
    Point_t *point = item;
    printf("(%d, %d, %d)\n", point->x, point->y, *(point->count));
    return true;
}

uint64_t point_hash(const void *item, uint64_t seed0, uint64_t seed1)
{
    const Point_t *point = item;
    return hashmap_sip(point, sizeof(Point_t), seed0, seed1);
}

typedef struct Line
{
    Point_t a;
    Point_t b;
} Line_t;

bool is_vertical(const Line_t *line)
{
    return line->a.x == line->b.x;
}

bool is_horizontal(const Line_t *line)
{
    return line->a.y == line->b.y;
}

// Parse Line_t from string.
// The parse_line function converts the initial part of teh string
// in str to a Line_t. The since strtol is used, the string can begin with an
// arbitrary amount of whitespace.
// If  endptr  is not NULL, strtol() stores the address of the first invalid
// character in *endptr.  If there were no digits at all, strtol() stores the
// original value of nptr in *endptr (and returns 0).
// In particular, if *nptr is not '\0' but **endptr is '\0' on return, the
// entire string is valid.
Line_t parse_line(const char *str, char **endptr)
{
    Line_t l;
    Point_t a;
    Point_t b;

    a.x = strtol(str, endptr, 10);
    str = *endptr + 1;

    a.y = strtol(str, endptr, 10);
    str = *endptr + 4;

    a.count = NULL;

    b.x = strtol(str, endptr, 10);
    str = *endptr + 1;

    b.y = strtol(str, endptr, 10);

    b.count = NULL;

    l.a = a;
    l.b = b;
    return l;
}

// Parse multiple lines from string
int parse_lines(const char *lptr, Line_t *lbuf, char sep)
{
    int len = count_items(lptr, sep);

    char *endptr;
    int i;
    for (i = 0; i < len; i++)
    {
        errno = 0;
        Line_t result = parse_line(lptr, &endptr);
        // Oh no, something went wrong.
        if (errno != 0)
        {
            break;
        }
        lbuf[i] = result;
        // We finished parsing
        if (*lptr != '\0' && *endptr == '\0')
        {
            break;
        }
        // New Line starts at the end of old Line + 1
        lptr = endptr + 1;
    }

    return i + 1;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("[ERROR] Supply the input for day05.\n");
        exit(1);
    }

    char *data = readfile(argv[1]);
    if (!data)
    {
        printf("[ERROR] Could not read data from file.\n");
        exit(1);
    }

    int len = count_items(data, '\n');
    Line_t *lines = (Line_t *)calloc(len, sizeof(Line_t));

    if (lines == NULL)
    {
        fprintf(stderr, "Could not allocate array of %d Lines.\n", len);
        exit(1);
    }

    int n_lines = parse_lines(data, lines, '\n');
    free(data);

    if (n_lines != len)
    {
        fprintf(stderr, "[ERROR] Unexpected number of lines. Received %d, expected %d.\n", n_lines, len);
        exit(1);
    }

    printf("Loaded data!\n");

    struct hashmap *map = hashmap_new(sizeof(Point_t), 0, 0, 0, point_hash, point_compare, NULL, NULL);

    printf("Initialized Hashmap!\n");

    int counts[1] = {0};
    hashmap_set(map, &(Point_t){.x = 0, .y = 1, .count = counts[0]});
    Point_t *point = hashmap_get(map, &(Point_t){.x = 0, .y = 1, .count = counts[0]});

    printf("(%d, %d, %d)\n", point->x, point->y, *(point->count));

    printf("\n-- iterate over all points --\n");
    hashmap_scan(map, point_iter, NULL);

    hashmap_free(map);
    return 0;
}
