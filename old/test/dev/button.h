#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include "44b.h"

/**
 * Nous définissons des flags pour chaque bouton, au lieu de manipuler directement
 * des données hexadécimals.
**/
#define BIT_ZERO            0x0
#define BUTTON_01           0x1
#define BUTTON_02           0x2
#define BUTTON_03           0x4
#define BUTTON_04           0x8


/**
 * @brief  Recupere l'etat des boutons.
 * @return Etat des boutons.
 */
extern unsigned int keyboard_status(void);


#endif /* __BUTTONS_H__ */
