#include "capteurs.h"
#include "affichage.h"

int main(int argc, char**argv)
{
  int code_erreur, buffer, fd, ret;
  short data;
  double temperature = 0.0, humidite, pression;
  double Vout, Vs;

  if (lancement_temperature(&temperature, buffer); != EXIT_SUCCESS)
  {
      perror("[main.c] lancement temperature");
      return EXIT_FAILURE;
  }

  //RECUPERATION DONNES CAPTEUR HUMIDITE/PRESSION
  if ((fd = open(ADC_DEVICE,O_RDONLY)) < 0 )
	{
		printf("[TEST ADC CAPTEUR] Problème d'ouverture de : %s\n", ADC_DEVICE);
    return EXIT_FAILURE;
	}

  //CONFIGURATION DU CANAL POUR L'HUMIDITE
  ret = ioctl(fd, ADC_CHANNEL, ADC_CHAN_HUMIDITY); //configuration du canal
  if(ret < 0)
  {
    perror("[TEST ADC CAPTEUR] Configuration du canal Humidité");
    return EXIT_FAILURE;
  }

  read(fd, &data, 2);  // Lecture de la valeur du capteur

  // Calcul humidite physique
  Vout = (2.5 / 1024) * data * 2;
  Vs = 5;
  humidite = (Vout - Vs * 0.16) / (Vs * 0.0062);  // Calcul humidite
  humidite = humidite / (1.0546 - 0.00216 * temperature); // Calcul de l'humidite avec T


  //CONFIGURATION DU CANAL POUR LA PRESSION
  ret = ioctl(fd, ADC_CHANNEL, ADC_CHAN_PRESSURE); // Configuration du canal
  if(ret < 0)
  {
      perror("[[TEST ADC CAPTEUR] Configuration du canal Pression");
      return EXIT_FAILURE;
  }

  read(fd, &data, 2);  // Lecture de la valeur du capteur

  Vout = (2.5 / 1024) * data * 2;
  Vs = 5.1;
  pression = (Vout + Vs * 0.1518) / (Vs * 0.01059) * 10; // Calcul de la pression



  //FERMETURE DU PROGRAMME
  close(fd);

  affichage(temperature, humidite, pression);

  return EXIT_SUCCESS;
}
