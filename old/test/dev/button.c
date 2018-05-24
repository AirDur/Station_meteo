#include "button.h"

/**
 * KEYBOARD_STATUS
 * @brief Retourne le bit du bouton pressé
*/
unsigned int KEYBOARD_STATUS(void)
   {
   rPCONG = BIT_ZERO;	//IN7~0
   rPUPG = BIT_ZERO;   //pull up enable
   return (~((rPDATG&0xf0)>>4)&0xF);
   }
