#ifndef __THREADS_H__
#define __THREADS_H__

#include "capteurs.h"

// Directives de configuration
#ifndef DUREE_VIE_ARCHIVES
    #define DUREE_VIE_ARCHIVES  600         // 10 minutes
#endif
#ifndef INTERVAL_TENDANCES
    #define INTERVAL_MAJ        1           // 5 secondes
#endif
#ifndef INTERVAL_TENDANCES
    #define INTERVAL_TENDANCES  10          // 10 seconde
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
 *            et qui met a jour la variable globale "g_etat_boutons".
 */
extern void * verifier_etat_boutons(void * arg);

/**
 * @brief     Thread qui recupere les donnees des capteurs.
 *            Met a jour g_archives_donnees
 */
extern void * maj_donnees_capteurs(void * arg);

#endif /* __THREADS_H__ */
