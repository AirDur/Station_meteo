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
  GR_FONT_ID    font;
  int buffer, ret, i=0;

  double temperature = 0.0, humidite = 0.0, pression = 0.0;

  if (GrOpen() < 0) {
    perror("[main.c] Impossible d'ouvrir le module graphique.");
    return EXIT_FAILURE;
  }

  w = GrNewWindow(1, 0, 0, 160, 240, 0, BLACK, WHITE);
  gc = GrNewGC();
  GrMapWindow(w);

  if(get_donnees(&buffer, &temperature, &humidite, &pression) == EXIT_FAILURE) {
    perror("[main.c] Problème au niveau de la récupération des données.");
    return EXIT_FAILURE;
  }

  if(affichage_current_data(w, gc, &temperature, &humidite, &pression) == EXIT_FAILURE) {
    perror("[main.c] Impossible d'afficher.");
    return EXIT_FAILURE;
  }

  //GrLine(w, gc, 10, 210, 130, 210);

  /*Enter event loop **/
  for(;;)
    GrGetNextEvent(&event);




    //function flushes any buffered function calls and closes the connection created with the GrOpen function.
   GrClose();

   return EXIT_SUCCESS;
   /*
    if ( lancement_temperature(&temperature, buffer) != EXIT_SUCCESS )
    {
        perror("[main.c] lancement température");
        return EXIT_FAILURE;
    }

    if ( ADC_to_humidity(temperature, &humidite, buffer) != EXIT_SUCCESS )
    {
        perror("[main.c] lancement humidité");
        return EXIT_FAILURE;
    }

    if ( ADC_to_pression(&pression, buffer) != EXIT_SUCCESS )
    {
        perror("[main.c] lancement pression");
        return EXIT_FAILURE;
    }

    close(buffer);

    if ( affichage(temperature, pression, humidite) != EXIT_SUCCESS )
    {
        perror("[main.c] affichage");
        return EXIT_FAILURE;
    }


  return EXIT_SUCCESS; */
}
