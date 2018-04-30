#include "capteurs.h"
#include "affichage.h"

int main(int argc, char**argv)
{
  int code_erreur, buffer, fd, ret;
  short data ;
  double temperature, RH, P;
  double Vout, Vs;


  temperature = 0;

  //Ouverture du perifique i2c :
  if ((buffer = open(I2C_FILE, O_RDWR)) < 0)
  {
      perror("[testTemperature] /dev/i2c0 n'existe pas : le driver n'est pas installe.");
      return EXIT_FAILURE;
  }

  code_erreur = configuration_i2c(buffer, CONFIG_TEMP_R0 | CONFIG_TEMP_R1);
  if (code_erreur != EXIT_SUCCESS)
  {
      perror("[testTemperature] Configuration registre de temperature");
      return EXIT_FAILURE;
  }

  code_erreur = lecture_temperature(buffer, &temperature);
  if (code_erreur != EXIT_SUCCESS)
  {
      perror("[testTemperature] Recuperation temperature");
      return EXIT_FAILURE;
  }
  sleep(2);

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
  RH = RH / (1.0546 - 0.00216 * temperature); // Calcul de l'humidite avec T


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



  //FERMETURE DU PROGRAMME
  close(fd);

  affichage(temperature, RH, P);

  return EXIT_SUCCESS;
}
