
#include "boutons.h" //le fichier .h

/**
 * KEYBOARD_STATUS
 * @brief Retourne le bit du bouton pressé
 */
unsigned int KEYBOARD_STATUS(void) {

  rPCONG = 0x0; //IN7~0
  rPUPG = 0x0; //pull up enable
  return (~((rPDATG & 0xf0) >> 4) & 0xF);
}

/*** fonction boutons poussoir****/
unsigned int Push_Button(void) {
  int Etat_bouton = KEYBOARD_STATUS(); //====recuperation fonction qui renvoie 1,2,3,4

  if ((Etat_bouton & BUTTON_01) == 1) //le bouton poussoir 1 recuperé ----> Button Ox1, S2 pressed
    return 1;

  if ((Etat_bouton & BUTTON_02) == 2) //le bouton poussoir 2 recuperé ----> Button 0x2, S3 pressed
    return 2;

  if ((Etat_bouton & BUTTON_03) == 4) //le bouton poussoir 3 recuperé ----> Button 0x4, S4 pressed
    return 3;

  if ((Etat_bouton & BUTTON_04) == 8) //le bouton poussoir 4 recuperé ----> Button 0x8, S5 pressed
    return 4;

  return -1;
}
