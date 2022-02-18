#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include "../lib/io.h"
#include "../lib/utils.h"
#include "../lib/hashmap/hashmap.h"

#define UNUSED(x) (void)(x)

extern int errno;

// We need to store points in a hashmap to add up lines.
typedef struct Point
{
    int x;
    int y;
    int count;
} Point_t;

int point_compare(const void *a, const void *b, void *pdata)
{
    UNUSED(pdata);
    const Point_t *pa = a;
    const Point_t *pb = b;
    return (pa->x == pb->x && pa->y == pb->y) - 1;
}

bool point_iter(const void *item, void *pdata)
{
    UNUSED(pdata);
    Point_t *point = (Point_t *)item;
    printf("(%d, %d):  %d\n", point->x, point->y, point->count);
    return true;
}

struct coords
{
    int x;
    int y;
};

uint64_t
point_hash(const void *item, uint64_t seed0, uint64_t seed1)
{
    const Point_t *point = item;

    struct coords c =
        {
            .x = point->x,
            .y = point->x,
        };

    return hashmap_sip(&c, sizeof(struct coords), seed0, seed1);
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

    a.count = 1;

    b.x = strtol(str, endptr, 10);
    str = *endptr + 1;

    b.y = strtol(str, endptr, 10);

    b.count = 1;

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

void add_or_inc(struct hashmap *map, Point_t *point)
{
    Point_t *entry = hashmap_get(map, point);
    if (entry == NULL)
    {
        hashmap_set(map, point);
    }
    else
    {
        entry->count += 1;
    }
}

void add_line(struct hashmap *map, Line_t *line)
{
    Point_t p = line->a;

    if (is_horizontal(line))
    {
        add_or_inc(map, &p);
        int inc = sign(line->b.x - line->a.x);
        while (p.x != line->b.x)
        {
            p.x += inc;
            add_or_inc(map, &p);
        }
    }
    else if (is_vertical(line))
    {
        add_or_inc(map, &p);
        int inc = sign(line->b.y - line->a.y);
        while (p.y != line->b.y)
        {
            p.y += inc;
            add_or_inc(map, &p);
        }
    }
}

void add_lines(struct hashmap *map, Line_t *lines, int n_lines)
{
    for (int i = 0; i < n_lines; i++)
    {
        add_line(map, lines);
        lines += 1;
    }
}

void add_line_diagonal(struct hashmap *map, Line_t *line)
{
    Point_t p = line->a;
    add_or_inc(map, &p);

    int incx = sign(line->b.x - line->a.x);
    int incy = sign(line->b.y - line->a.y);
    while (p.x != line->b.x || p.y != line->b.y)
    {
        p.x += incx;
        p.y += incy;
        add_or_inc(map, &p);
    }
}

void add_lines_diagonal(struct hashmap *map, Line_t *lines, int n_lines)
{
    for (int i = 0; i < n_lines; i++)
    {
        add_line_diagonal(map, lines);
        lines += 1;
    }
}

bool count_cover(const void *item, void *pdata)
{
    int *count = (int *)pdata;
    Point_t *point = (Point_t *)item;
    if (point->count >= 2)
    {
        *count += 1;
    }
    return true;
}

bool idx_iter(const void *item, void *pdata)
{
    int *maxidx = (int *)pdata;
    Point_t *point = (Point_t *)item;

    *maxidx = point->x > *maxidx ? point->x : *maxidx;
    *maxidx = point->y > *maxidx ? point->y : *maxidx;
    return true;
}

int max_idx(struct hashmap *map)
{
    int maxidx = 0;
    hashmap_scan(map, idx_iter, (void *)&maxidx);
    return maxidx;
}

void print_grid(struct hashmap *map)
{
    int maxidx = max_idx(map);
    for (int i = 0; i <= maxidx; i++)
    {
        for (int j = 0; j <= maxidx; j++)
        {
            Point_t q = {
                .x = j,
                .y = i,
                .count = 0,
            };

            Point_t *p = hashmap_get(map, &q);
            if (p == NULL)
            {
                printf(".");
            }
            else
            {
                printf("%d", p->count);
            }
        }
        printf("\n");
    }
    printf("\n");
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

    printf("--- Part One ---\n");

    struct hashmap *map = hashmap_new(sizeof(Point_t), 0, 0, 0, point_hash, point_compare, NULL, NULL);

    add_lines(map, lines, n_lines);

    // print_grid(map);

    int count = 0;
    hashmap_scan(map, count_cover, &count);
    printf("Number of covers >= 2: %d\n", count);

    hashmap_free(map);

    printf("--- Part Tow ---\n");

    struct hashmap *map_diag = hashmap_new(sizeof(Point_t), 0, 0, 0, point_hash, point_compare, NULL, NULL);

    add_lines_diagonal(map_diag, lines, n_lines);

    // print_grid(map_diag);

    int count_diag = 0;
    hashmap_scan(map_diag, count_cover, &count_diag);
    printf("Number of covers >= 2: %d\n", count_diag);

    hashmap_free(map_diag);
    free(lines);
    return 0;
}
