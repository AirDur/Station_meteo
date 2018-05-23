#include "boutons.h"

BIT_BOUTON BIT_BOUTON(void)
{
    rPCONG = 0x0;       //IN7~0
    rPUPG  = 0x0;       //pull up enable
    return ~((rPDATG & 0xf0) >> 4) & 0xF;

} // KEYBOARD_STATUS
