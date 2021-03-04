#include "common.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
    setup_raw_mode();
    char c;
    char buf[3];
    do {
        c = read_key();
        //write(STDOUT_FILENO, "\r\x1b[K", 4);
        snprintf(buf, 3, "%02X", c);
        write(STDOUT_FILENO, buf, 2);
    } while (c != '\0');
}
