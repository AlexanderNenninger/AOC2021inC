#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/io.h"

#define SEP '\n'

// Simulate 0-padding on an array.
int get(int i, int *array, int length)
{
    if (i < 0 || i >= length)
    {
        return 0;
    }
    return array[i];
}

// solve part 2 of day 1
int sliding_window(int *nums, int len)
{
    int increases = 0;
    int curr = get(0, nums, len) + get(1, nums, len) + get(2, nums, len);
    for (int i = 2; i < len; i++)
    {
        int val = get(i - 2, nums, len) + get(i - 1, nums, len) + get(i, nums, len);

        if (val > curr)
        {
            increases++;
        }
        curr = val;
    }
    return increases;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("[ERROR] Supply the input for day01.\n");
        exit(1);
    }

    char *data = readfile(argv[1]);
    if (!data)
    {
        exit(1);
    }

    int len = count_items(data, SEP);
    int *numbers = (int *)malloc(len * sizeof(int));
    int n_numbers = parse_ints(data, numbers, SEP);

    int ns[10];
    for (int i = 0; i < 10; i++)
    {
        ns[i] = numbers[i];
    }

    int increases = 0;
    int curr = numbers[0];
    for (int i = 0; i < n_numbers; i++)
    {

        if (numbers[i] > curr)
        {
            increases++;
        }
        curr = numbers[i];
    }

    printf("Number of increases: %d\n", increases);

    // Part 2
    int inc2 = sliding_window(numbers, len);

    printf("Sliding Window: %d\n", inc2);
}
