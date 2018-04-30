/**
 * Permet de lire les donnees du bus i2c
 */

#ifndef CAPTEUR_H
#define CAPTEUR_H

/**
#include "44b.h"
#include <time.h>
#include <signal.h>
*/
#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <pthread.h>

#define MWINCLUDECOLORS
#define ADC_CHAN_PRESSURE    4
#define ADC_CHAN_HUMIDITY    5


#define I2C_SLAVE             0x0703        // "adresse" du bus i2c
#define I2C_ADDRESS_TEMP      0x92          // Float address + 1 R/W bit low
#define I2C_FILE              "/dev/i2c0"   //fichier special i2c0

#define OCTET_ZERO            0x00          //Just un octet de 0.

// Bit de configuration du registre de temperature
#define CONFIG_TEMP_R0      0x40
#define CONFIG_TEMP_R1      0x20


#ifndef ADC_CHANNEL
    #define ADC_CHANNEL      0x500
#endif

// Fichier correspondant aux peripheriques de la carte
#define ADC_DEVICE           "/dev/adc"


#ifndef LECTURE_DOUBLE
union lecture_double {
  char tab[8];
  double data;
}
#endif


extern int lire_humidite(int fd, double T, double * RH);

/**
 * @brief    Lecture du niveau de pression.
 * @param fd Descripteur de fichier du peripherique (CAN).
 * @param P  Parametre de sortie. Valeur de la pression mesuree (en hPA).
 * @return   EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int lire_pression(int fd, double * P);

/**
 * @brief    Lectures des donnees de tous les capteurs.
 * @param p  Parametre de sortie. Valeurs des differentes sondes.
 * @return   EXIT_SUCCESS ou EXIT_FAILURE
 */
int lancement_temperature(double *temperature, int buffer);

int clacul_humidite_pression(double temperature, double *pression, double *humidite, int buffer);

#endif
