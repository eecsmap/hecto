#include "common.h"
#include "ascii.h"

#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define GO_HOME write(STDOUT_FILENO, "\x1b[H", 4)
#define BEGIN_LINE write(STDOUT_FILENO, "\r\x1b[K", 4)
#define REVERSE_VIDEO write(STDOUT_FILENO, "\r\x1b[7m", 5)
#define NO_REVERSE_VIDEO write(STDOUT_FILENO, "\r\x1b[27m", 6)
#define CURSOR_SHOW write(STDOUT_FILENO, "\x1b[25h", 6)
#define CURSOR_HIDE write(STDOUT_FILENO, "\x1b[25l", 6)

#define STATUS_MARGIN 1

typedef struct pair_s { int x; int y} pair_t;

// ===================================================
// get screen size
// ===================================================
void update_screen_size(pair_t *screen_size)
{
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) < 0) {
        perror("ioctl");
        exit(1);
    }
    screen_size->y = w.ws_row - STATUS_MARGIN;
    screen_size->x = w.ws_col;
}

// text file consists of multiple lines
typedef struct file_s {
    char **lines;
    size_t line_count;
} file_t;

typedef struct control_s {
    enum {VIEW, EDIT} mode;
    pair_t screen_size, screen_offset, screen_cursor, file_cursor;
    file_t file;
} control_t;

void add_line(file_t *file, char *line, ssize_t index)
{
    file->lines = realloc(file->lines, sizeof *file->lines * ++file->line_count);
    if (index == -1) index = file->line_count - 1;
    if (index < file->line_count - 1)
        memmove(file->lines + index + 1, file->lines + index, file->line_count - 1 - index);
    file->lines[index] = strdup(line);
}

void load_file(char *file_path, file_t *file)
{
    FILE *fp = fopen(file_path, "r");
    if (!fp) {
        perror("fopen");
        exit(1);
    }

    char *line = NULL;
    size_t size = 0;
    ssize_t nread = 0;
    while ((nread = getline(&line, &size, fp)) > 0) {
        char *end = line + nread - 1;
        if (*end == '\r' || *end == '\n') *end = '\0';
        add_line(file, line, -1);
    }
    free(line);
    fclose(fp);
}

void show_cursor(pair_t *screen_cursor)
{
    dprintf(STDOUT_FILENO, "\x1b[%d;%dH", screen_cursor->y + 1, screen_cursor->x + 1);
}

void handle_key(control_t *control)
{
    pair_t *screen_size = &control->screen_size;
    pair_t *cursor = &control->screen_cursor;
    char c = read_key();
    if (control->mode == VIEW)
        switch (c) {
        case '\0': exit(0);
        case 'i': control->mode = EDIT; break;
        case 'h': {
            if (control->file_cursor.x > 0)
            {
                control->file_cursor.x--;
                if (control->screen_cursor.x == 0) control->screen_offset.x--;
                else control->screen_cursor.x--;
            }
        } break;
        case 'j': {
            if (control->file_cursor.y < control->file.line_count - 1)
            {
                control->file_cursor.y++;
                if (control->screen_cursor.y == control->screen_size.y - 1) control->screen_offset.y++;
                else control->screen_cursor.y++;
                size_t current_line_length = strlen(control->file.lines[control->file_cursor.y]);
                if (control->file_cursor.x > current_line_length) {
                    control->file_cursor.x = current_line_length;
                    control->screen_cursor.x = control->file_cursor.x - control->screen_offset.x;
                    if (control->screen_cursor.x < 0) {
                        control->screen_offset.x += control->screen_cursor.x;
                        control->screen_cursor.x = 0;
                    }
                }
            }
        } break;
        case 'k': {
            if (control->file_cursor.y > 0)
            {
                control->file_cursor.y--;
                if (control->screen_cursor.y == 0) control->screen_offset.y--;
                else control->screen_cursor.y--;
                size_t current_line_length = strlen(control->file.lines[control->file_cursor.y]);
                if (control->file_cursor.x > current_line_length) {
                    control->file_cursor.x = current_line_length;
                    control->screen_cursor.x = control->file_cursor.x - control->screen_offset.x;
                    if (control->screen_cursor.x < 0) {
                        control->screen_offset.x += control->screen_cursor.x;
                        control->screen_cursor.x = 0;
                    }
                }
            }
        } break;
        case 'l': {
            if (control->file_cursor.x < strlen(control->file.lines[control->file_cursor.y]))
            {
                control->file_cursor.x++;
                if (control->screen_cursor.x == control->screen_size.x - 1) control->screen_offset.x++;
                else control->screen_cursor.x++;
            }
        } break;
        }
    else if (control->mode == EDIT)
        switch (c) {
        case ESC: control->mode = VIEW; break;
        case BS:
        case DEL:
            {
                if (control->file_cursor.x > 0) {
                    memmove(control->file.lines[control->file_cursor.y] + control->file_cursor.x - 1,
                            control->file.lines[control->file_cursor.y] + control->file_cursor.x,
                            strlen(control->file.lines[control->file_cursor.y]) - control->file_cursor.x + 1);
                    control->file_cursor.x--;
                    if (control->screen_cursor.x == 0) control->screen_offset.x--;
                    else control->screen_cursor.x--;
                } else {
                    if (control->file_cursor.y == 0) break;
                    // move the current one up to the previous one if there is one.
                    char **previous_line = control->file.lines + control->file_cursor.y - 1;
                    char *cur_line = control->file.lines[control->file_cursor.y];
                    control->file_cursor.x = strlen(*previous_line);
                    *previous_line = realloc(*previous_line, strlen(*previous_line) + strlen(cur_line) + 1);
                    strcat(*previous_line, cur_line);
                    if (control->file_cursor.y < control->file.line_count - 1) {
                        memmove(control->file.lines + control->file_cursor.y,
                                control->file.lines + control->file_cursor.y + 1,
                                sizeof control->file.lines[0] * (control->file.line_count - control->file_cursor.y - 1));
                    }
                    control->file.line_count--;
                    free(cur_line);
                    control->file_cursor.y--;
                    control->screen_cursor.x = control->file_cursor.x;
                    if (control->file_cursor.x > control->screen_size.x - 1) {
                        control->screen_offset.x = control->file_cursor.x - control->screen_size.x + 1;
                        control->screen_cursor.x = control->screen_size.x - 1;
                    }
                    if (control->screen_cursor.y == 0) control->screen_offset.y--;
                    else control->screen_cursor.y--;
                }
            };
            break;
        case '\r':
        case '\n': {
            control->file.lines = realloc(control->file.lines, sizeof control->file.lines[0] * (++control->file.line_count));
            if (control->file_cursor.y + 2 < control->file.line_count)
                memmove(control->file.lines + control->file_cursor.y + 2, control->file.lines + control->file_cursor.y + 1, sizeof control->file.lines[0] * (control->file.line_count - control->file_cursor.y - 2));
            if (control->file_cursor.x < strlen(control->file.lines[control->file_cursor.y])) {
                control->file.lines[control->file_cursor.y + 1] = strdup(control->file.lines[control->file_cursor.y] + control->file_cursor.x);
                control->file.lines[control->file_cursor.y][control->file_cursor.x] = '\0';
            } else {
                control->file.lines[control->file_cursor.y + 1] = malloc(1);
                control->file.lines[control->file_cursor.y + 1][0] = '\0';
            }
            control->file_cursor.y++;
            if (control->screen_cursor.y == control->screen_size.y - 1) control->screen_offset.y++;
            else control->screen_cursor.y++;
            control->file_cursor.x = 0;
            control->screen_cursor.x = 0;
            control->screen_offset.x = 0;
            }
        break;
        default: {
            if (!isprint(c)) break;
            char **current_line = &control->file.lines[control->file_cursor.y];
            size_t len = strlen(*current_line);
            *current_line = realloc(*current_line, len + 2);
            if (control->file_cursor.x < len)
                memmove(*current_line + control->file_cursor.x + 1, *current_line + control->file_cursor.x, len - control->file_cursor.x + 1);
            *(*current_line + control->file_cursor.x) = c;
            control->file_cursor.x++;
            if (control->screen_cursor.x == control->screen_size.x - 1) control->screen_offset.x++;
            else control->screen_cursor.x++;
        }}
}

void display(control_t *control)
{
    GO_HOME;
    file_t *file = &control->file;
    pair_t *screen_size = &control->screen_size;
    size_t screen_row_count = screen_size->y;
    //size_t row_count = file->line_count > screen_row_count ? screen_row_count : file->line_count;
    for (size_t i = 0; i < screen_row_count; ++i) {
        BEGIN_LINE;
        size_t index = i + control->screen_offset.y;
        if (index < file->line_count) {
            if (control->screen_offset.x >= strlen(file->lines[index]))
                dprintf(STDOUT_FILENO, "\r\n");
            else {
                char *temp = strndup(file->lines[index] + control->screen_offset.x, control->screen_size.x);
                dprintf(STDOUT_FILENO, "%s\r\n", temp);
                free(temp);
            }
        }
        else
            dprintf(STDOUT_FILENO, "~\r\n");
    }
    REVERSE_VIDEO;
    BEGIN_LINE;
    dprintf(STDOUT_FILENO, "%s | cursor <%d,%d>", control->mode ? "EDIT" : "VIEW", control->screen_cursor.y+1, control->screen_cursor.x+1);
    NO_REVERSE_VIDEO;
    show_cursor(&control->screen_cursor);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        exit(1);
    }
    control_t control = {0};
    load_file(argv[1], &control.file);
    setup_raw_mode();
    do {
        update_screen_size(&control.screen_size);
        handle_key(&control);
        display(&control);
    } while (1);
}
