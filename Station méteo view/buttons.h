
#ifndef __BUTTONS_H__
#define __BUTTONS_H__


#include "44b.h"


// Definition de type
#define KEYBOARD_STATUS     unsigned int

// Flag des differents boutons
#define BUTTON_NONE         0x00
#define BUTTON_01           0x01
#define BUTTON_02           0x02
#define BUTTON_03           0x04
#define BUTTON_04           0x08


/**
 * @brief  Recupere l'etat des boutons.
 * @return Etat des boutons.
 */
extern KEYBOARD_STATUS keyboard_status(void);


#endif /* __BUTTONS_H__ */
