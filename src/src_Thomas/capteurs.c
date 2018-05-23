#include "capteurs.h"



#include "capteurs.h"

int get_donnees(int * buffer, double * temperature, double * humidite, double * pression) {

  if ( lancement_temperature(temperature, *buffer) != EXIT_SUCCESS )
  {
      perror("[main.c] lancement température");
      return EXIT_FAILURE;
  }

  if ( ADC_to_humidity(*temperature, humidite, *buffer) != EXIT_SUCCESS )
  {
      perror("[main.c] lancement humidité");
      return EXIT_FAILURE;
  }

  if ( ADC_to_pression(pression, *buffer) != EXIT_SUCCESS )
  {
      perror("[main.c] lancement pression");
      return EXIT_FAILURE;
  }

  close(*buffer);

  return EXIT_SUCCESS;
}

int configuration_i2c(int buffer, unsigned char mask)
{
  int code_erreur;
  unsigned char bus[2];

  code_erreur = ioctl(buffer, I2C_SLAVE, I2C_ADDRESS_TEMP);
  if(code_erreur < 0)
  {    //si le fichier n'existe pas on affiche un message d'erreur
       perror("[configuration_i2c] ADC_CHANNEL ioctl cmd");
       close(buffer);
       return EXIT_FAILURE;
  }

  bus[0] = 0x01;                               // Configuration Register
  bus[1] = mask;                               // Masque de configuration


  code_erreur = write(buffer, bus, 2); //ecriture de deux valeurs sur fd
  if (code_erreur < 0)
  {
      perror("[configuration_i2c] Erreur d'ecriture sur le bus i2c ");
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

//calcul de la température à partir des données brutes
double calcul_temperature(short donnees_brut){
  return donnees_brut * 0.0625;
}

int lecture_temperature (int buffer, double * Temperature) {
  int code_erreur;
  unsigned char bus[2];
  short donnees_brut;

  code_erreur = ioctl(buffer, I2C_SLAVE, I2C_ADDRESS_TEMP);
  if(code_erreur < 0)
  {
       perror("[lecture_temperature] Probleme de configuration de i2c");
       close(buffer);
       return EXIT_FAILURE;
  }


	bus[0] = OCTET_ZERO;

  code_erreur = write(buffer, bus, 1); //ecriture de deux valeurs sur fd
  if (code_erreur < 0)
  {
    perror("[lecture_temperature] Probleme de configuration des registres");
    close(buffer);
    return EXIT_FAILURE;
  }
  code_erreur = read(buffer, bus, 2); //ecriture de deux valeurs sur fd
  if (code_erreur < 0)
  {
    perror("[lecture_temperature] Probleme de lecture du registre de temperature");
    close(buffer);
    return EXIT_FAILURE;
  }

  donnees_brut = (bus[0] << 4) | (bus[1] >> 4);
  *Temperature = calcul_temperature(donnees_brut);                          // 0.0625 = 128 / 2048

	printf(" - Valeur de température : %lf \n", *Temperature);

	return EXIT_SUCCESS;
}

int lancement_temperature(double *temperature, int buffer) {
  //Ouverture du perifique i2c :
  if ((buffer = open(I2C_DEVICE, O_RDWR)) < 0)
  {
      perror("[testTemperature] /dev/i2c0 n'existe pas : le driver n'est pas installe.");
      return EXIT_FAILURE;
  }

  if (configuration_i2c(buffer, CONFIG_TEMP_R0 | CONFIG_TEMP_R1) != EXIT_SUCCESS)
  {
      perror("[testTemperature] Configuration registre de temperature");
      return EXIT_FAILURE;
  }

  if (lecture_temperature(buffer, temperature) != EXIT_SUCCESS)
  {
      perror("[testTemperature] Recuperation temperature");
      return EXIT_FAILURE;
  }
  sleep(2);

  return EXIT_SUCCESS;
}

double calcul_humidite(short donnees_brut, double temperature){
  double Vout, Vs, resultat1, resultat2;
  Vout = (2.5 / 1024) * donnees_brut * 2;
  Vs = 5;
  printf(" Données brutes : %hi \n", donnees_brut);
  printf(" Temperature = %lf \n", temperature);
  printf(" Vs = %lf  \n", Vs);
  Vout = (2.5 / 1024) * donnees_brut * 2;
  printf(" Vout = %lf \n", Vout);
  resultat1 = (Vout - Vs * 0.16) / (Vs * 0.0062);  // Calcul humidite
  printf(" Resultat 1 = %lf \n", resultat1);
  resultat2 = (resultat1) / (1.0546 - (0.00216 * temperature)); // Calcul de l'humidite avec T
  printf(" Resultat 2 = %lf \n", resultat2);
  return resultat2;
}

int ADC_to_humidity(double temperature, double *humidite, int buffer) {
  short donnees_brut;

  //RECUPERATION DONNES CAPTEUR HUMIDITE/PRESSION
  if ((buffer = open(ADC_DEVICE,O_RDONLY)) < 0 )
  {
    printf("[TEST ADC CAPTEUR] Problème d'ouverture de : %s\n", ADC_DEVICE);
    return EXIT_FAILURE;
  }

  //CONFIGURATION DU CANAL POUR L'HUMIDITE
  if(ioctl(buffer, ADC_CHANNEL, ADC_CHAN_HUMIDITY) < 0)
  {
    perror("[TEST ADC CAPTEUR] Configuration du canal Humidité");
    return EXIT_FAILURE;
  }

  read(buffer, &donnees_brut, 2);  // Lecture de la valeur du capteur

  // Calcul humidite physique

  *humidite = calcul_humidite(donnees_brut, temperature);


	printf(" - Valeur de humidité : %lf \n", *humidite);

  return EXIT_SUCCESS;
}

double calcul_pression(short donnees_brut){
  double Vout, Vs;
  Vout = (2.5 / 1024) * donnees_brut * 2;
  Vs = 5.1;
  return (Vout + Vs * 0.1518) / (Vs * 0.01059) * 10; // Calcul de la pression
}

int ADC_to_pression(double *pression, int buffer) {
  short donnees_brut;

  //RECUPERATION DONNES CAPTEUR HUMIDITE/PRESSION
  if ((buffer = open(ADC_DEVICE,O_RDONLY)) < 0 )
  {
    printf("[TEST ADC CAPTEUR] Problème d'ouverture de : %s\n", ADC_DEVICE);
    return EXIT_FAILURE;
  }

  //CONFIGURATION DU CANAL POUR LA PRESSION
  if(ioctl(buffer, ADC_CHANNEL, ADC_CHAN_PRESSURE) < 0)
  {
      perror("[[TEST ADC CAPTEUR] Configuration du canal Pression");
      return EXIT_FAILURE;
  }

  read(buffer, &donnees_brut, 2);  // Lecture de la valeur du capteur

  *pression = calcul_pression(donnees_brut);

	printf(" - Valeur de pression : %lf \n", *pression);

  return EXIT_SUCCESS;
}
