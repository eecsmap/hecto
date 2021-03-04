#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

void setup_raw_mode();
char read_key();
int get_screen_size(size_t *row, size_t *col);

typedef struct {
    size_t nrow;
    size_t ncol;
} screen_t;

void update_screen_size(screen_t *screen);

#endif
