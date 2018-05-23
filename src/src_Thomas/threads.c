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
volatile int g_etat_boutons = BOUTON_02;
volatile int g_fin_programme = 0;
volatile t_tendances g_tendances;
volatile t_captors_data g_donnees_capteurs = { 0, 0, 0 };
volatile t_captors_data g_donnees_moyennes_capteurs = { 0, 0, 0 };

static t_captors_data g_dernieres_mesures_tendances = { 0, 0, 0 };
static t_captors_data g_archives_donnees[NB_MAX_ARCHIVES];
static int g_id_archive = 0;
static int g_nb_archives = 0;


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

    g_donnees_moyennes_capteurs.T = 0;
    g_donnees_moyennes_capteurs.P = 0;
    g_donnees_moyennes_capteurs.RH = 0;

    for (i = 0; i < g_nb_archives; ++i)
    {
        g_donnees_moyennes_capteurs.T  += g_archives_donnees[i].T;
        g_donnees_moyennes_capteurs.P  += g_archives_donnees[i].P;
        g_donnees_moyennes_capteurs.RH += g_archives_donnees[i].RH;
    }

    g_donnees_moyennes_capteurs.T /= g_nb_archives;
    g_donnees_moyennes_capteurs.P /= g_nb_archives;
    g_donnees_moyennes_capteurs.RH /= g_nb_archives;

} // calculer_moyennes

void * verifier_etat_boutons(void * arg)
{
    BIT_BOUTON KbStatus;

    while (!g_fin_programme)
    {
        KbStatus = keyboard_status();

        if((KbStatus & BOUTON_01) == BOUTON_01)
        {
            g_etat_boutons = BOUTON_01;
            while(keyboard_status() == KbStatus);
        }
        else if((KbStatus & BOUTON_02) == BOUTON_02)
        {
            g_etat_boutons = BOUTON_02;
            while(keyboard_status() == KbStatus);
        }
        else if((KbStatus & BOUTON_03) == BOUTON_03)
        {
            g_etat_boutons = BOUTON_03;
            while(keyboard_status() == KbStatus);
        }
        else if((KbStatus & BOUTON_04)== BOUTON_04)
        {
            g_etat_boutons = BOUTON_04;
            while(keyboard_status() == KbStatus);
        }

        usleep(10000);
    }

} // verifier_etat_boutons

void * maj_donnees_capteurs(void * arg)
{
    int i, r;

    for (i = 0; i < NB_MAX_ARCHIVES; ++i)
    {
        g_archives_donnees[i].T  = 0;
        g_archives_donnees[i].P  = 0;
        g_archives_donnees[i].RH = 0;
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

void * maj_tendances(void * arg)
{
    while (!g_fin_programme)
    {
        sleep(INTERVAL_TENDANCES);

        if (g_donnees_capteurs.T > g_dernieres_mesures_tendances.T)
            g_tendances.T = 1;
        else
            g_tendances.T = -1;

        if (g_donnees_capteurs.P > g_dernieres_mesures_tendances.P)
            g_tendances.P = 1;
        else
            g_tendances.P = -1;

        if (g_donnees_capteurs.RH > g_dernieres_mesures_tendances.RH)
            g_tendances.RH = 1;
        else
            g_tendances.RH = -1;

        g_dernieres_mesures_tendances = g_donnees_capteurs;
    }

} // maj_tendances
