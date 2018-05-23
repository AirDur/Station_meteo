#ifndef __BOUTONS_H__
#define __BOUTONS_H__


#include "44b.h"


// Definition de type
#define KEYBOARD_STATUS     unsigned int

// Flag des differents boutons
#define BOUTON_01           0x01
#define BOUTON_02           0x02
#define BOUTON_03           0x04
#define BOUTON_04           0x08


/**
 * @brief  Recupere l'etat des boutons.
 * @return Etat des boutons.
 */
extern KEYBOARD_STATUS keyboard_status(void);


#endif /* __BOUTONS_H__ */
