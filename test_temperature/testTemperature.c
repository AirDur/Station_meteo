/**
 * @file     	testI2C.c
 * @author   	Patrick BREUGNON
 * @brief    	Fontion permettant de tester l'interface i2c
 *		Ne pas oublie de charger le driver i2c en effectuant
 *		la commande
 *		# insmod i2c_s3c44.o
 * 		On peut verifier si le driver est charge par la presence
 *		du fichier special situe dans /dev
 *		# ls /dev/i2c0c
 */


#include "testTemperature.h"

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
  *Temperature = donnees_brut * 0.0625;                          // 0.0625 = 128 / 2048

	printf("temperature : %lf\n", *Temperature);

	return EXIT_SUCCESS;
}


int main(int argc, char**argv)
{
  int code_erreur, buffer;
  double temperature;

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

}
