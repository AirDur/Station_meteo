#include "capteurs.h"
#include "affichage.h"

int main(int argc, char**argv)
{
  int buffer, ret;

  double temperature = 0.0, humidite, pression;


  if ( lancement_temperature(&temperature, buffer) != EXIT_SUCCESS )
  {
      perror("[main.c] lancement temperature");
      return EXIT_FAILURE;
  }

  if ( calcul_humidite_pression(temperature, &pression, &humidite, buffer) != EXIT_SUCCESS )
  {
      perror("[main.c] calcul_humidite_pression");
      return EXIT_FAILURE;
  }



  //FERMETURE DU PROGRAMME
  close(fd);

  affichage(temperature, humidite, pression);

  return EXIT_SUCCESS;
}
