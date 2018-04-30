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


#include <nano-X.h>


// Chemin vers les fichiers images representant les tendances.
#define IMG_UP     "/home/img/up.gif"
#define IMG_DOWN   "/home/img/down.gif"


/**
 * @brief    Affichage les donnees des capteurs en temps reel.
 * @param w  ID de la fenetre.
 * @param gc ID de la configuration graphique.
 * @param p  Pointeur sur les donnees des capteurs.
 * @return   EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int affichage_menu_01(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data p);

/**
 * @brief    Affichage des moyennes des donnees des capteurs.
 * @param w  ID de la fenetre.
 * @param gc ID de la configuration graphique.
 * @param p  Pointeur sur les moyennes des donnees des capteurs.
 * @return   EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int affichage_menu_02(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data p);

/**
 * @brief    Affichage des tendances des donnees des capteurs.
 * @param w  ID de la fenetre.
 * @param gc ID de la configuration graphique.
 * @param p  Pointeur sur les tendances des donnees des capteurs.
 * @return   EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int affichage_menu_03(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_tendances p);

/**
 * @brief    Affichage d'un ecran de fin d'application.
 * @param w  ID de la fenetre.
 * @param gc ID de la configuration graphique.
 * @return   EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int affichage_menu_04(GR_WINDOW_ID w, GR_GC_ID gc);


#endif /* __MENUS_H__ */
