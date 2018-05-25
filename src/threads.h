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
    #define DUREE_VIE_ARCHIVES  600         // 10 min
#endif
#ifndef INTERVAL_TENDANCES
    #define INTERVAL_MAJ        1           // 5 secs
#endif
#ifndef INTERVAL_TENDANCES
    #define INTERVAL_TENDANCES  10          // 10 secs
#endif

#define NB_MAX_ARCHIVES     (DUREE_VIE_ARCHIVES / INTERVAL_MAJ)


// Variables globales
extern volatile int g_etat_boutons;
extern volatile int g_etat_bouton_1;
extern volatile int g_fin_programme;
extern volatile t_tendances g_tendances;
extern volatile t_captors_data g_donnees_capteurs;
extern volatile t_captors_data g_donnees_moyennes_capteurs;
extern volatile t_captors_data g_archives_donnees[NB_MAX_ARCHIVES];
extern int g_nb_archives;

/**
 * @brief     Thread qui verifie l'etat des boutons poussoirs.
 *            Met a jour la variable globale "g_etat_boutons".
 * @param arg Parametres du thread (rien).
 * @return    Resultat du thread (rien).
 */
extern void * verifier_etat_boutons(void * arg);

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
