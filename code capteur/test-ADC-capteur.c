/**
 * @file     test-ADC-capteur.c
 *
 * @author   GATTAZ Thomas
 *
 * @brief    Fonctions permettant la lecture des donnees des capteurs ADC.
 */

#include "test-ADC-capteur.h"


#define ADC_CHAN_PRESSURE    4
#define ADC_CHAN_HUMIDITY    5

void main()
{
  short data ;
  int fd, ret ;
  double RH, P, T=27 ;
  double Vout, Vs;

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
  RH = (Vout - Vs * 0.16) / (Vs * 0.0062);  // Calcul humidite
  RH = RH / (1.0546 - 0.00216 * T); // Calcul de l'humidite avec T

  //Affichage humidité
  printf("Valeur de l'humidité : %lf", &RH);


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
  P = (Vout + Vs * 0.1518) / (Vs * 0.01059) * 10; // Calcul de la pression

  //Affichage humidité
  printf("Valeur de la pression : %lf", &P);

  //FERMETURE DU PROGRAMME
  close(fd);
  return EXIT_SUCCESS;
}
