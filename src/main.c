#include "capteurs.h"
#include "affichage.h"

int main(int argc, char**argv)
{
  int buffer, ret;

  double temperature = 0.0, humidite = 0.0, pression = 0.0;


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

  if ( ADC_to_pression(temperature, &pression, buffer) != EXIT_SUCCESS )
  {
      perror("[main.c] lancement pression");
      return EXIT_FAILURE;
  }

  close(buffer);

  if ( affichage(temperature, humidite, pression) != EXIT_SUCCESS )
  {
      perror("[main.c] affichage");
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
