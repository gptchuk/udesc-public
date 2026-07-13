start:
    LOAD 1
    XTO y
    LOAD 7
loop:
    XTO r4
    BC
    XTO r4
    BS
    GOTO loop