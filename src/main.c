#include <pthread.h>
#include <stdio.h>

#include "nano-X.h"
#include "capteurs.h"
#include "affichage.h"
#include "boutons.h"


int main(int argc, char**argv)
{
  GR_WINDOW_ID  w;
  GR_EVENT      event;
  GR_GC_ID      gc;
  int   buffer
      , i                   = 0
      , programme_en_cours  = 1
      , selection_ecran     = DONNEES_ACTUELLES
      , bouton              = NO_BUTTON;

  double temperature = 0.0, humidite = 0.0, pression = 0.0;

  // On appelle le module graphique
  if (GrOpen() < 0) {
    perror("[main.c] Impossible d'ouvrir le module graphique.");
    return EXIT_FAILURE;
  }

  //ON crée une fenetre graphique
  w = GrNewWindow(1, 0, 0, 160, 240, 0, BLACK, WHITE);
  GrMapWindow(w);
  gc = GrNewGC();

  //On récupère les données une premiere fois
  if(get_donnees(&buffer, &temperature, &humidite, &pression) == EXIT_FAILURE) {
    perror("[main.c] Problème au niveau de la récupération des données.");
    return EXIT_FAILURE;
  }

  //On affiche les données une premiere fois
  if(affichage_current_data(w, gc, &temperature, &humidite, &pression) == EXIT_FAILURE) {
    perror("[main.c] Impossible d'afficher.");
    return EXIT_FAILURE;
  }

  //On lit les boutons en boucle.
  while (programme_en_cours == 1){
    //lit le bouton sélectionné
    GrGetNextEventTimeout(&event, 2000);

    switch (event.type) {
      case GR_EVENT_TYPE_TIMEOUT:
        if (bouton == BUTTON_01) {
          bouton = NO_BUTTON;
          selection_ecran = CURRENT_DATA; //(selection_ecran+1)%3; //JSP A QUOI CA SERT
        }

        switch(selection_ecran){
          case DONNEES_ACTUELLES:
            GrClearWindow(w,0); //On réactualise les données
            affichage_current_data(w,gc,&g_donnees_capteurs);

            switch(bouton){

              case BUTTON_02:
                bouton = NO_BUTTON;
                printf("bouton 2\n");
                //current_orientation = (current_orientation+1)%3;
                break;

              case BUTTON_03:
                bouton = NO_BUTTON;
                printf("bouton 3\n");
                //current_format_temp = (current_format_temp+1)%3;
                break;

              case BUTTON_04:
                bouton = NO_BUTTON;
                printf("bouton 4\n");
                programme_en_cours = 0;
                //current_format_pressure = (current_format_pressure+1)%3;
                break;
            }
            break;
          /*
          case PAST_DATA:
            GrClearWindow(w,0);
            //------------------------------------------AAAAAAAAAAAAAAAAAAAAAAAA
            affichage_menu_02(w, gc, g_archives_donnees_week,g_nb_archives_semaine);

            switch(button){

              case BUTTON_02:
                button = BUTTON_NONE;
                current_data = (current_data+1)%3;
                break;

              case BUTTON_03:
                button = BUTTON_NONE;
                switch(current_data){
                  case DATA_TEMP:
                    current_format_temp = (current_format_temp+1)%3;
                    break;
                  case DATA_PRESSURE:
                    current_format_pressure = (current_format_pressure+1)%2;
                    break;
                }
                break;

            }

            break;
          case FUTURE_DATA:
            GrClearWindow(w,0);
            //------------------------------------------AAAAAAAAAAAAAAAAAAAAAAAA
            affichage_menu_03(w, gc, g_archives_tendances, g_nb_archives_tendances);

            switch(button){

              case BUTTON_02:
                button = BUTTON_NONE;
                current_data = (current_data+1)%3;
                break;

              case BUTTON_03:
                button = BUTTON_NONE;
                switch(current_data){
                  case DATA_TEMP:
                    current_format_temp = (current_format_temp+1)%3;
                    break;
                  case DATA_PRESSURE:
                    current_format_pressure = (current_format_pressure+1)%2;
                    break;
                }
                break;

            }*/

            break;
        }

      break;
    }
  }



    //function flushes any buffered function calls and closes the connection created with the GrOpen function.
   GrClose();

   return EXIT_SUCCESS;
}
