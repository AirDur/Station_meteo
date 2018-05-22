/**
* @file     main.c
*
* @author   GOURDON Arnaud
*           PREVOST Theophile
*
* @brief    Programme principal de la station meteorologique.
*           Gestion de l'affichage et des differents menus.
*/

#define MWINCLUDECOLORS
#include <graphics.h>
#include "nano-X.h"
#include <pthread.h>
#include <stdio.h>
#include "buttons.h"
#include "threads.h"
#include "menus.h"


int main(int ac,char **av){
  //GR_WINDOW_ID w;
  //GR_GC_ID gc;
  //GR_EVENT event;
  int current_screen = CURRENT_DATA;
  int button = BUTTON_NONE;
  int orientation = 0;
  pthread_t th_boutons, th_capteurs, th_tendances;

  /*if (GrOpen() < 0)
  {
    printf("Can't open graphics\n");
    exit(1);
  }
  */
  // Configuration de l'ecran
  w = GrNewWindow(1, 0, 0, 160, 240, 0, BLACK, WHITE);
  gc = GrNewGC();
  GrSetGCForeground(gc, WHITE);
  GrSetGCUseBackground(gc, GR_FALSE);
  GrSelectEvents(w, GR_EVENT_MASK_EXPOSURE);
  GrMapWindow(w);

  // Creation des threads
  pthread_create(&th_boutons,   NULL, &verifier_etat_boutons, &button);
  pthread_create(&th_capteurs,  NULL, &maj_donnees_capteurs,  NULL);
  pthread_create(&th_tendances, NULL, &maj_tendances,         NULL);

  // Boucle principale
  //on affiche qdf même qqchose au début
  affichage_current_data(w,gc,&g_donnees_capteurs);






  while (!g_fin_programme){

    //while(button_pressed){

    GrGetNextEventTimeout(&event, 1000);

    switch (event.type) {
      case GR_EVENT_TYPE_TIMEOUT:
        if (button==BUTTON_01) {
          button = BUTTON_NONE;
          current_screen = (current_screen+1)%3;
        }

        switch(current_screen){
          case CURRENT_DATA:
            GrClearWindow(w,0);
            affichage_current_data(w,gc,&g_donnees_capteurs);

            switch(button){

              case BUTTON_02:
                button = BUTTON_NONE;
                current_orientation = (current_orientation+1)%3;
                break;

              case BUTTON_03:
                button = BUTTON_NONE;
                current_format_temp = (current_format_temp+1)%3;
                break;

              case BUTTON_04:
                button = BUTTON_NONE;
                current_format_pressure = (current_format_pressure+1)%3;
                break;
            }
            break;

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

            }

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

}
