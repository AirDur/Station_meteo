#ifndef _BOUTONS_H
#define _BOUTONS_H



//==================constantes boutons
BUTTON1 0x1
BUTTON2 0x2
BUTTON3 0x4
BUTTON4 0x8


//======================les fonctions recuperation boutons
unsigned int KEYBOARD_STATUS(void);
unsigned int Push_Button(void);

#endif /* _BOUTONS_H*/
