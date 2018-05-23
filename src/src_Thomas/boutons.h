#ifndef __BOUTONS_H__
#define __BOUTONS_H__

#include "44b.h"

/**
 * Nous définissons des flags pour chaque bouton, au lieu de manipuler directement
 * des données hexadécimals.
**/
#define BIT_ZERO            0x0
#define NO_BUTTON           0x0
#define BUTTON_01           0x1
#define BUTTON_02           0x2
#define BUTTON_03           0x4
#define BUTTON_04           0x8
#define BIT_BOUTON          unsigned int

/**
 * @brief  Recupere l'etat des boutons.
 * @return Etat des boutons.
 */
extern BIT_BOUTON KEYBOARD_STATUS(void);

#endif
