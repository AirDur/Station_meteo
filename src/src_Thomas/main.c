/**
 * @file     main.c
 *
 * @author   GOURDON Arnaud
 *           PREVOST Theophile
 *
 * @brief    Programme principal de la station meteorologique.
 *           Gestion de l'affichage et des differents menus.
 */

#include <graphics.h>
#include "nano-X.h"
#include <pthread.h>
#include <stdio.h>
#include "boutons.h"
#include "threads.h"


int main(int ac,char **av)
{
    GR_WINDOW_ID w;
    GR_GC_ID gc;
    GR_EVENT event;
    pthread_t th_boutons, th_capteurs, th_tendances;

    if (GrOpen() < 0)
    {
        printf("Can't open graphics\n");
        exit(1);
    }

    // Configuration de l'ecran
    w = GrNewWindow(1, 0, 0, 160, 240, 0, WHITE, BLACK);
    gc = GrNewGC();
    GrSetGCForeground(gc, BLACK);
    GrSetGCUseBackground(gc, GR_FALSE);
    GrSelectEvents(w, GR_EVENT_MASK_EXPOSURE | GR_EVENT_MASK_TIMEOUT);
    GrMapWindow(w);

    // Creation des threads
    pthread_create(&th_boutons,   NULL, &verifier_etat_boutons, NULL);
    pthread_create(&th_capteurs,  NULL, &maj_donnees_capteurs,  NULL);
    pthread_create(&th_tendances, NULL, &maj_tendances,         NULL);

    // Boucle principale
    while (!g_fin_programme)
    {
        switch (g_etat_boutons)
        {
          case BOUTON_1:       // MENU "TEMPS REEL"

            GrGetNextEventTimeout(&event, 1000);
            switch (event.type)
            {
              case GR_EVENT_TYPE_EXPOSURE:
                affichage_menu_01(w, gc, &g_donnees_capteurs);
                break;
              case GR_EVENT_TYPE_TIMEOUT:
                GrClearWindow(w, 1);
                break;
            }
            break;

          case BOUTON_2:       // MENU "MOYENNES DES DONNEES"

            GrGetNextEventTimeout(&event, 1000);
            switch (event.type)
            {
              case GR_EVENT_TYPE_EXPOSURE:
                affichage_menu_02(w, gc, &g_donnees_moyennes_capteurs);
                break;
              case GR_EVENT_TYPE_TIMEOUT:
                GrClearWindow(w, 1);
                break;
            }
            break;

          case BOUTON_3:       // MENU "TENDANCES"

            GrGetNextEventTimeout(&event, 1000);
            switch (event.type)
            {
              case GR_EVENT_TYPE_EXPOSURE:
                affichage_menu_03(w, gc, &g_tendances);
                break;
              case GR_EVENT_TYPE_TIMEOUT:
                GrClearWindow(w, 1);
                break;
            }
            break;

          case BOUTON_4:       // ARRET DE L'APPLICATION

            GrGetNextEventTimeout(&event, 3000);
            switch (event.type)
            {
              case GR_EVENT_TYPE_EXPOSURE:
                affichage_menu_04(w, gc);
                break;
              case GR_EVENT_TYPE_TIMEOUT:
                g_fin_programme = 1;
                break;
            }
            break;
        }
    }

    // Destruction des ressources
    GrClose();

    // Attente de la fin des threads
    pthread_join(th_capteurs,  NULL);
    pthread_join(th_boutons,   NULL);
    pthread_join(th_tendances, NULL);

    printf("Fin du programme\n");

    return 0;

} // main
