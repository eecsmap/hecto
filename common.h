#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

void setup_raw_mode();
char read_key();
int get_screen_size(size_t *row, size_t *col);

#endif
