#include "common.h"
#include "ascii.h"

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#define GO_HOME write(STDOUT_FILENO, "\x1b[H", 4)
#define BEGIN_LINE write(STDOUT_FILENO, "\r\x1b[K", 4)
#define REVERSE_VIDEO write(STDOUT_FILENO, "\r\x1b[7m", 5)
#define NO_REVERSE_VIDEO write(STDOUT_FILENO, "\r\x1b[27m", 6)
#define CURSOR_SHOW write(STDOUT_FILENO, "\x1b[25h", 6)
#define CURSOR_HIDE write(STDOUT_FILENO, "\x1b[25l", 6)

#define STATUS_MARGIN 2

// text file consists of multiple lines
typedef struct file_handle_s {
    char **lines;
    size_t line_count;
} file_handle_t;

typedef struct control_handle_s {
    enum {VIEW, EDIT} mode;
} control_handle_t;

void add_line(file_handle_t *file, char *line, ssize_t index)
{
    file->lines = realloc(file->lines, sizeof *file->lines * ++file->line_count);
    if (index == -1) index = file->line_count - 1;
    if (index < file->line_count - 1)
        memmove(file->lines + index + 1, file->lines + index, file->line_count - 1 - index);
    file->lines[index] = strdup(line);
}

file_handle_t load_file(char *file_path)
{
    FILE *fp = fopen(file_path, "r");
    if (!fp) {
        perror("fopen");
        exit(1);
    }

    char *line = NULL;
    size_t size = 0;
    ssize_t nread = 0;
    file_handle_t file = {0};
    while ((nread = getline(&line, &size, fp)) > 0) {
        char *end = line + nread - 1;
        if (*end == '\r' || *end == '\n') *end = '\0';
        add_line(&file, line, -1);
    }
    free(line);
    fclose(fp);
    return file;
}

void show_cursor(screen_t *screen)
{
    dprintf(STDOUT_FILENO, "\x1b[%d;%dH", screen->cursor.y + 1, screen->cursor.x + 1);
}

void handle_key(control_handle_t *control, screen_t *screen)
{
    cursor_t *cursor = &screen->cursor;
    char c = read_key();
    if (control->mode == VIEW)
        switch (c) {
        case '\0': exit(0);
        case 'i': control->mode = EDIT; break;
        case 'h': {
            cursor->x > 0 && --cursor->x;
        } break;
        case 'j': {
            cursor->y < screen->nrow && ++cursor->y;
        } break;
        case 'k': {
            cursor->y > 0 && --cursor->y;
        } break;
        case 'l': {
            cursor->x < screen->ncol && ++cursor->x;
        } break;
        }
    else if (control->mode == EDIT)
        switch (c) {
        case ESC: control->mode = VIEW; break;
        }
}

void display(control_handle_t *control, file_handle_t *file, screen_t *screen)
{
    GO_HOME;
    size_t screen_row_count = screen->nrow - STATUS_MARGIN;
    size_t row_count = file->line_count > screen_row_count ? screen_row_count : file->line_count;
    for (size_t i = 0; i < row_count; ++i) {
        BEGIN_LINE;
        dprintf(STDOUT_FILENO, "%s\r\n", file->lines[i]);
    }
    REVERSE_VIDEO;
    BEGIN_LINE;
    dprintf(STDOUT_FILENO, "%s | cursor <%d,%d>", control->mode ? "EDIT" : "VIEW", screen->cursor.y+1, screen->cursor.x+1);
    NO_REVERSE_VIDEO;
    show_cursor(screen);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        exit(1);
    }
    file_handle_t file = load_file(argv[1]);
    control_handle_t control = {0};
    setup_raw_mode();
    screen_t screen = {0};
    do {
        update_screen_size(&screen);
        display(&control, &file, &screen);
        handle_key(&control, &screen);
    } while (1);
}
