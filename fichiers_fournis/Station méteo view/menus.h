/**
 * @file     menu.h
 *
 * @author   GOURDON Arnaud
 *           PREVOST Theophile
 *
 * @brief    Module comprenant les fonctions d'affichage des differents menus.
 */

#ifndef __MENUS_H__
#define __MENUS_H__

#include "nano-X.h"



// Chemin vers les fichiers images representant les tendances.
#define IMG_UP      "/home/img/up.gif"
#define IMG_DOWN    "/home/img/down.gif"

#define CURRENT_DATA            0
#define PAST_DATA               1
#define FUTURE_DATA             2

#define DATA_TEMP               0
#define DATA_PRESSURE           1
#define DATA_HUMIDITY           2

#define FORMAT_TEMP_CELSIUS     0
#define FORMAT_TEMP_FAHRENHEIT  1
#define FORMAT_TEMP_KELVIN      2

#define FORMAT_PRESSURE_PASCAL  0
#define FORMAT_PRESSURE_BAR     1


#define ABCISSE_BOUT 140
#define ABCISSE_ORIGINE 10
#define ORDONNE_BOUT 40
#define ORDONNE_ORIGINE 220




// Variables globales
//extern volatile int current_screen; //0, 1, 2
extern volatile int current_orientation;
extern volatile int current_data; // t= 0, p = 1, h = 2
extern volatile int current_format_temp; //0, 1, 2
extern volatile int current_format_pressure; //0, 1
extern volatile int current_orientation;



/**
 * @brief    Affichage les donnees des capteurs en temps reel.
 * @param w  ID de la fenetre.
 * @param gc ID de la configuration graphique.
 * @param p  Pointeur sur les donnees des capteurs.
 * @return   EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int affichage_current_data(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data p);

/**
 * @brief    Affichage des moyennes des donnees des capteurs.
 * @param w            ID de la fenetre.
 * @param gc           ID de la configuration graphique.
 * @param data         tableau de toutes les données historiques des capteurs de la semaine
 * @param nbArchives   nb de données historiques
 * @return   EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int affichage_menu_02(GR_WINDOW_ID w, GR_GC_ID gc, t_captors_data data[],int nbArchives);

/**
 * @brief    Affichage des moyennes des donnees des capteurs.
 * @param w            ID de la fenetre.
 * @param gc           ID de la configuration graphique.
 * @param data         tableau de toutes les données historiques des capteurs de la semaine
 * @param nbArchives   nb de données tendances
 * @return   EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int affichage_menu_03(GR_WINDOW_ID w, GR_GC_ID gc, t_captors_data data[], int nbArchives);


void get_Pressure(char data[64], t_ptr_captors_data p);
void get_Temp(char data[64], t_ptr_captors_data p);

int next(int current, int max);

#endif /* __MENUS_H__ */
