#include "buttons.h"


KEYBOARD_STATUS keyboard_status(void)
{
    rPCONG = 0x0;       //IN7~0
    rPUPG  = 0x0;       //pull up enable
    return ~((rPDATG & 0xf0) >> 4) & 0xF;

} // keyboard_status
