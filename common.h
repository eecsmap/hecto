#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

void setup_raw_mode();
char read_key();
int get_screen_size(size_t *row, size_t *col);

typedef struct cursor_s { int x; int y} cursor_t;

typedef struct {
    size_t nrow;
    size_t ncol;
    cursor_t cursor;
} screen_t;

void update_screen_size(screen_t *screen);

#endif
