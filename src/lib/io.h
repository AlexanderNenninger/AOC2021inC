
#ifndef IO_H_ /* Include guard */
#define IO_H_
#include "io.c"

// Read file into string.
char *readfile(char *filename);

// Count blocks in string separated by `sep`, e.g. '\n'
int count_items(char *str, char sep);

// Greadily parse numbers from string.
int parse_ints(char *str, int *ibuf, char sep);

#endif // IO_H_
