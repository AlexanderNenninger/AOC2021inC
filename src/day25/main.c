#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../lib/io.h"

typedef enum state
{
    SERROR = -1,
    NONE = 0,
    EAST,
    SOUTH,
} state_t;

state_t state_from_str(const char *c)
{
    if (*c == '.')
    {
        return NONE;
    }
    else if (*c == '>')
    {
        return EAST;
    }
    else if (*c == 'v')
    {
        return SOUTH;
    }
    return SERROR;
}

char state_to_str(const state_t *s)
{
    if (*s == NONE)
    {
        return '.';
    }
    else if (*s == EAST)
    {
        return '>';
    }
    else if (*s == SOUTH)
    {
        return 'v';
    }
    else
    {
        return 'E';
    }
}

typedef struct board
{
    size_t width;
    size_t height;
    state_t *data;
} board_t;

bool board_equals(const board_t *b0, const board_t *b1)
{
    if (b0->width != b1->width)
    {
        return false;
    }
    if (b0->height != b1->height)
    {
        return false;
    }
    for (size_t i = 0; i < b0->height; i++)
    {
        for (size_t j = 0; j < b0->width; j++)
        {
            if (b0->data[i * b0->width + j] != b1->data[i * b1->width + j])
            {
                return false;
            }
        }
    }
    return true;
}

void board_zero(const board_t *b)
{
    for (size_t i = 0; i < b->height; i++)
    {
        for (size_t j = 0; j < b->width; j++)
        {
            b->data[i * b->width + j] = NONE;
        }
    }
}

board_t board_like(const board_t *b)
{
    board_t out;
    out.width = b->width;
    out.height = b->height;
    out.data = calloc(out.width * out.height, sizeof(state_t));
    return out;
}

board_t board_from_str(char *c)
{
    state_t *buf = calloc(strlen(c), sizeof(state_t));
    int offset = 0;
    int i;
    int width = 0;
    for (i = 0; c[i] != '\0'; i++)
    {
        if (c[i + 1] == '\0' && c[i] == '\n')
        {
            break;
        }

        state_t s = state_from_str(&c[i]);
        if (s == SERROR)
        {
            offset++;
            width = 0;
        }
        else
        {
            buf[i - offset] = s;
            width++;
        }
    }
    board_t b = {width, offset + 1, buf};
    return b;
}

void board_print(const board_t *b)
{
    for (size_t i = 0; i < b->height; i++)
    {
        for (size_t j = 0; j < b->width; j++)
        {
            state_t s = b->data[i * b->width + j];
            char c = state_to_str(&s);
            printf("%s", &c);
        }
        printf("\n");
    }
}

void board_destroy(board_t board)
{
    free(board.data);
}

bool east_empty(const board_t *b, size_t i, size_t j)
{
    return b->data[i * b->width + (j + 1) % b->width] == NONE;
}

void step_east(const board_t *b, state_t *buf)
{
    for (size_t i = 0; i < b->height; i++)
    {
        for (size_t j = 0; j < b->width; j++)
        {
            if (b->data[i * b->width + j] == EAST)
            {
                if (east_empty(b, i, j))
                {
                    buf[i * b->width + j] = NONE;
                    buf[i * b->width + (j + 1) % b->width] = EAST;
                }
                else
                {
                    buf[i * b->width + j] = EAST;
                }
            }
            else if (buf[i * b->width + j] != EAST)
            {
                buf[i * b->width + j] = b->data[i * b->width + j];
            }
        }
    }
}

bool south_empty(const board_t *b, size_t i, size_t j)
{
    return b->data[((i + 1) % b->height) * b->width + j] == NONE;
}

void step_south(const board_t *b, state_t *buf)
{
    for (size_t i = 0; i < b->height; i++)
    {
        for (size_t j = 0; j < b->width; j++)
        {
            if (b->data[i * b->width + j] == SOUTH)
            {
                if (south_empty(b, i, j))
                {
                    buf[i * b->width + j] = NONE;
                    buf[((i + 1) % b->height) * b->width + j] = SOUTH;
                }
                else
                {
                    buf[i * b->width + j] = SOUTH;
                }
            }
            else if (buf[i * b->width + j] != SOUTH)
            {
                buf[i * b->width + j] = b->data[i * b->width + j];
            }
        }
    }
}

bool board_step(board_t *b0, board_t *b1)
{
    board_zero(b1);
    step_east(b0, b1->data);

    state_t *temp0 = b0->data;
    b0->data = b1->data;
    b1->data = temp0;

    board_zero(b1);
    step_south(b0, b1->data);

    state_t *temp1 = b0->data;
    b0->data = b1->data;
    b1->data = temp1;

    if (board_equals(b0, b1))
    {
        return false;
    }
    return true;
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
    // Parse all lines
    board_t b0 = board_from_str(data);
    free(data);

    printf("Initial state:\n");
    // board_print(&b0);

    board_t b1 = board_like(&b0);

    board_t *bptr0 = &b0;
    board_t *bptr1 = &b1;

    bool moving = true;
    int nsteps = 0;
    do
    {
        moving = board_step(bptr0, bptr1);
        nsteps++;
    } while (moving);

    board_print(bptr0);
    printf("\nSteps: %d\n", nsteps);
    board_destroy(b0);
    board_destroy(b1);
    return 0;
}
