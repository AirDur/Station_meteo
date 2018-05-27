#ifndef __MENUS_H__
#define __MENUS_H__

#include "nano-X.h"
#include <math.h>

// Image présent sur la carte :
#define IMG_UP        "/home/img/up.gif"
#define IMG_DOWN      "/home/img/down.gif"
#define IMG_RIGHT     "/home/img/right.gif"

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
 * @brief    Affichage d'un ecran de fin d'application.
 * @param w  ID de la fenetre.
 * @param gc ID de la configuration graphique.
 * @return   EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int affichage_menu_04(GR_WINDOW_ID w, GR_GC_ID gc);

/**
 * Affichage de la courbe de tendance pour la température
 * @param  w                  [fenetre graphique]
 * @param  gc                 [configuration graphique]
 * @param  g_archives_donnees [donnée à utiliser pour afficher la tendance]
 * @param  g_nb_archives      [nombre d'archive]
 * @return                    EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int affichage_menu_courbes_temperature_celsius(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data g_archives_donnees, int g_nb_archives);
*
/**
 * Affichage de la courbe de tendance pour la pression
 * @param  w                  [fenetre graphique]
 * @param  gc                 [configuration graphique]
 * @param  g_archives_donnees [donnée à utiliser pour afficher la tendance]
 * @param  g_nb_archives      [nombre d'archive]
 * @return                    EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int affichage_menu_courbes_pression_hPa(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data g_archives_donnees, int g_nb_archives);

/**
 * Affichage de la courbe de tendance pour la humidite
 * @param  w                  [fenetre graphique]
 * @param  gc                 [configuration graphique]
 * @param  g_archives_donnees [donnée à utiliser pour afficher la tendance]
 * @param  g_nb_archives      [nombre d'archive]
 * @return                    EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int affichage_menu_courbes_humidite_gm3(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data g_archives_donnees, int g_nb_archives);

#endif /* __MENUS_H__ */
