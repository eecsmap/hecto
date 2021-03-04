#include "common.h"

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#define GO_HOME write(STDOUT_FILENO, "\x1b[H", 4)
#define BEGIN_LINE write(STDOUT_FILENO, "\r\x1b[K", 4)
#define REVERSE_VIDEO write(STDOUT_FILENO, "\r\x1b[7m", 5)
#define NO_REVERSE_VIDEO write(STDOUT_FILENO, "\r\x1b[27m", 6)
#define STATUS_MARGIN 2

void show_cursor(screen_t *screen)
{
    dprintf(STDOUT_FILENO, "\x1b[%d;%dH", screen->cur_row, screen->cur_col);
}

void handle_key()
{
    char c = read_key();
    switch (c) {
    case '\0': exit(0);
    }
}

void display(screen_t *screen)
{
    GO_HOME;
    for (size_t i = 0; i < screen->nrow - STATUS_MARGIN; ++i) {
        BEGIN_LINE;
        dprintf(STDOUT_FILENO, "%03zu\r\n", i);
    }
    REVERSE_VIDEO;
    BEGIN_LINE;
    dprintf(STDOUT_FILENO, "%s", "hello world");
    NO_REVERSE_VIDEO;
    show_cursor(screen);
}

int main()
{
    setup_raw_mode();
    screen_t screen = {0};
    do {
        update_screen_size(&screen);
        display(&screen);
        handle_key();
    } while (1);
}
