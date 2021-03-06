/**
 * @file     threads.c
 *
 * @author   GOURDON Arnaud
 *           PREVOST Theophile
 *
 * @brief    Modules contenant differents threads.
 */

#include "boutons.h"
#include "threads.h"


#define NB_MAX_ARCHIVES     (DUREE_VIE_ARCHIVES / INTERVAL_MAJ)


// Initialisation des variables globales
volatile int g_etat_boutons = BOUTON_1;
volatile int g_etat_bouton_1 = 1;
volatile int g_fin_programme = 0;
volatile t_tendances g_tendances;
volatile t_captors_data g_donnees_capteurs = { 0, 0, 0, 0, 0, 0 };
volatile t_captors_data g_donnees_moyennes_capteurs = { 0, 0, 0, 0, 0, 0};

static t_captors_data g_dernieres_mesures_tendances = { 0, 0, 0 };
volatile t_captors_data g_archives_donnees[NB_MAX_ARCHIVES];
static int g_id_archive = 0;
int g_nb_archives = 0;

/**
 * Fonction d'archivage des données en temps réél.
 */
void archiver_donnees_capteurs(void)
{
    g_archives_donnees[g_id_archive] = g_donnees_capteurs;
    g_id_archive = (g_id_archive + 1) % NB_MAX_ARCHIVES;

    if (g_nb_archives < NB_MAX_ARCHIVES)
        ++g_nb_archives;

} // archiver_donnees_capteurs

void calculer_moyennes(void)
{
    int i;

    g_donnees_moyennes_capteurs.Tc = 0;
    g_donnees_moyennes_capteurs.Ph = 0;
    g_donnees_moyennes_capteurs.Hr = 0;
    g_donnees_moyennes_capteurs.Tf = 0;
    g_donnees_moyennes_capteurs.Pm = 0;
    g_donnees_moyennes_capteurs.Ha = 0;

    for (i = 0; i < g_nb_archives; ++i)
    {
        g_donnees_moyennes_capteurs.Tc  += g_archives_donnees[i].Tc;
        g_donnees_moyennes_capteurs.Ph  += g_archives_donnees[i].Ph;
        g_donnees_moyennes_capteurs.Hr += g_archives_donnees[i].Hr;
        g_donnees_moyennes_capteurs.Tf  += g_archives_donnees[i].Tf;
        g_donnees_moyennes_capteurs.Pm  += g_archives_donnees[i].Pm;
        g_donnees_moyennes_capteurs.Ha += g_archives_donnees[i].Ha;
    }

    g_donnees_moyennes_capteurs.Tc /= g_nb_archives;
    g_donnees_moyennes_capteurs.Ph /= g_nb_archives;
    g_donnees_moyennes_capteurs.Hr /= g_nb_archives;
    g_donnees_moyennes_capteurs.Tf /= g_nb_archives;
    g_donnees_moyennes_capteurs.Pm /= g_nb_archives;
    g_donnees_moyennes_capteurs.Ha /= g_nb_archives;

} // calculer_moyennes

void * verifier_etat_boutons(void * arg)
{
    BIT_BOUTON KbStatus;

    while (!g_fin_programme)
    {
        KbStatus = KEYBOARD_STATUS();

        if((KbStatus & BOUTON_1) == BOUTON_1)
        {
            if(g_etat_boutons == BOUTON_1)    g_etat_bouton_1 = -g_etat_bouton_1;
            else                              g_etat_boutons = BOUTON_1;
            while(KEYBOARD_STATUS() == KbStatus);
        }
        else if((KbStatus & BOUTON_2) == BOUTON_2)
        {
            if(g_etat_boutons == BOUTON_2)    g_etat_bouton_1 = -g_etat_bouton_1;
            else                              g_etat_boutons = BOUTON_2;
            while(KEYBOARD_STATUS() == KbStatus);
        }
        else if((KbStatus & BOUTON_3) == BOUTON_3)
        {
            if (g_etat_boutons == BOUTON_3)  g_etat_bouton_1 = (g_etat_bouton_1+1)%3;
            else                              g_etat_boutons = BOUTON_3;
            while(KEYBOARD_STATUS() == KbStatus);
        }
        else if((KbStatus & BOUTON_4)== BOUTON_4)
        {
            g_etat_boutons = BOUTON_4;
            while(KEYBOARD_STATUS() == KbStatus);
        }

        usleep(1000);
    }

} // verifier_etat_boutons

void * maj_donnees_capteurs(void * arg)
{
    int i, r;

    for (i = 0; i < NB_MAX_ARCHIVES; ++i)
    {
        g_archives_donnees[i].Tc    = 0;
        g_archives_donnees[i].Tf    = 0;
        g_archives_donnees[i].Ph    = 0;
        g_archives_donnees[i].Pm    = 0;
        g_archives_donnees[i].Hr    = 0;
        g_archives_donnees[i].Ha    = 0;
    }

    while (!g_fin_programme)
    {
        r = lire_donnees_capteurs(&g_donnees_capteurs);
        if (r == EXIT_SUCCESS)
        {
            archiver_donnees_capteurs();
            calculer_moyennes();
            sleep(INTERVAL_MAJ);
        }
        else
        {
            g_fin_programme = 1;
        }
    }

} // maj_donnees_capteurs
