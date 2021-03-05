#ifndef KEY_H
#define KEY_H

/*
 * https://en.wikipedia.org/wiki/ASCII
 */
enum key {
    NUL = 0x00;
    SOH, // start of heading
    STX, // start of text
    ETX, // end of text
    EOT, // end of transmission
    ENQ, // enquiry
    ACK, // acknowledgement
    BEL, // bell
    BS,  // backspace
    HT,  // horizontal tab
    LF,  // line feed
    VT,  // vertical tab
    FF,  // form feed
    CR,  // carriage return
    SO,  // shift out
    SI,  // shift in
    DLE, // data link escape
    DC1, // device control 1, XON
    DC2, // device control 2
    DC3, // device control 3, XOFF
    DC4, // device control 4
    NAK, // negative acknowledgement
    SYN, // synchronous idle
    ETB, // end of transmission block
    CAN, // cancel
    EM,  // end of medium
    SUB, // substitute
    ESC, // escape
    FS,  // file separator
    GS,  // group separator
    RS,  // record separator
    US,  // unit separator

    DEL = 0x7F, // delete
};

#endif
