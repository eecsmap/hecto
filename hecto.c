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
}

int main()
{
    setup_raw_mode();
    char c;
    screen_t screen;
    do {
        update_screen_size(&screen);
        display(&screen);
        c = read_key();
    } while (c != '\0');
}
