#include "common.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>

// ===================================================
// raw mode control
// ===================================================
static struct termios orig_termios;

static void reset_termios()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

static void enable_raw_mode()
{
    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN |  ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) {
        perror("tcsetattr");
        exit(1);
    }
}

void setup_raw_mode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) < 0) {
        perror("tcgetattr");
        exit(1);
    }
    if (atexit(reset_termios) < 0) {
        perror("atexit");
        exit(1);
    }
    enable_raw_mode();
}

// ===================================================
// read key
// ===================================================
char read_key()
{
    ssize_t nread;
    char c;
    while (!(nread = read(STDIN_FILENO, &c, 1)));
    if (nread < 0) {
        perror("read");
        exit(1);
    }
    return c;
}

// ===================================================
// get screen size
// ===================================================
int get_screen_size(size_t *row, size_t *col)
{
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) < 0) {
        perror("ioctl");
        return -1;
    }
    *row = w.ws_row;
    *col = w.ws_col;
    return 0;
}
