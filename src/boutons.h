#ifndef __BOUTONS_H__
#define __BOUTONS_H__

#include "44b.h"

/**
 * Nous définissons des flags pour chaque bouton, au lieu de manipuler directement
 * des données hexadécimals.
**/
#define BIT_ZERO            0x0
#define AUCUN_BOUTON        0x0
#define BOUTON_1            0x1
#define BOUTON_2            0x2
#define BOUTON_3            0x4
#define BOUTON_4            0x8

#define BIT_BOUTON          unsigned int

 /**
  *  Recupere l'etat des boutons.
  * @return Etat des boutons.
  */
extern BIT_BOUTON KEYBOARD_STATUS(void);

#endif
