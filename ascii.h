#ifndef KEY_H
#define KEY_H

/*
 * https://en.wikipedia.org/wiki/ASCII
 */
#define NUL   0
#define SOH   1 // start of heading
#define STX   2 // start of text
#define ETX   3 // end of text
#define EOT   4 // end of transmission
#define ENQ   5 // enquiry
#define ACK   6 // acknowledgement
#define BEL   7 // bell
#define BS    8 // backspace
#define HT    9 // horizontal tab
#define LF   10 // line feed
#define VT   11 // vertical tab
#define FF   12 // form feed
#define CR   13 // carriage return
#define SO   14 // shift out
#define SI   15 // shift in
#define DLE  16 // data link escape
#define DC1  17 // device control 1, XON
#define DC2  18 // device control 2
#define DC3  19 // device control 3, XOFF
#define DC4  20 // device control 4
#define NAK  21 // negative acknowledgement
#define SYN  22 // synchronous idle
#define ETB  23 // end of transmission block
#define CAN  24 // cancel
#define EM   25 // end of medium
#define SUB  26 // substitute
#define ESC  27 // escape
#define FS   28 // file separator
#define GS   29 // group separator
#define RS   30 // record separator
#define US   31 // unit separator
#define DEL 127 // delete

#endif
