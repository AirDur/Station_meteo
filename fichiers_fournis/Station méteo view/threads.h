/**
 * @file     threads.h
 *
 * @author   GOURDON Arnaud
 *           PREVOST Theophile
 *
 * @brief    Modules contenant differents threads.
 */

#ifndef __THREADS_H__
#define __THREADS_H__


#include "capteurs.h"


// Directives de configuration
#ifndef DUREE_VIE_ARCHIVES
    #define DUREE_VIE_ARCHIVES  10        // 1h
#endif
#ifndef INTERVAL_MAJ
    #define INTERVAL_MAJ        1          // 10s
#endif
#ifndef INTERVAL_TENDANCES
    #define INTERVAL_TENDANCES  20          // 20s
#endif

#define NB_MAX_ARCHIVES_HEURE      10  //(DUREE_VIE_ARCHIVES / INTERVAL_MAJ)
#define NB_MAX_ARCHIVES_SEMAINE    168


// Variables globales
extern volatile int button_pressed;
extern volatile int g_etat_boutons;
extern volatile int g_fin_programme;
extern volatile t_tendances g_tendances;
extern volatile t_captors_data g_donnees_capteurs;
extern volatile t_captors_data g_donnees_moyennes_capteurs;

extern t_captors_data g_archives_donnees_heure[NB_MAX_ARCHIVES_HEURE];
extern t_captors_data g_archives_donnees_week[NB_MAX_ARCHIVES_SEMAINE]; //
extern t_captors_data g_archives_tendances[NB_MAX_ARCHIVES_HEURE-1];    //

extern int g_nb_archives_semaine;   //
extern int g_nb_archives_tendances; //


/**
 * @brief     Thread qui verifie l'etat des boutons poussoirs.
 *            Met a jour la variable globale "g_etat_boutons".
 * @param arg .
 * @return    Resultat du thread (rien).
 */
extern void * verifier_etat_boutons(int * arg);

/**
 * @brief     Thread qui recupere les donnees des capteurs.
 *            Met a jour les variables globales
 *            "g_donnees_capteurs" et "g_donnees_moyennes_capteurs".
 * @param arg Parametres du thread (rien).
 * @return    Resultat du thread (rien).
 */
extern void * maj_donnees_capteurs(void * arg);

/**
 * @brief     Thread qui met a jour les tendances.
 *            Met a jour la variable globale "g_tendances".
 * @param arg Parametres du thread (rien).
 * @return    Resultat du thread (rien).
 */
extern void * maj_tendances(void * arg);


#endif /* __THREADS_H__ */
