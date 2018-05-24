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
#include <math.h>
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

int configuration_i2c(int buffer, unsigned char mask);


/************    ****************
*********** Temperature en Celsius ************
***********          ***********/

//calcul de la température à partir des données brutes en Celsius
double calcul_temperature_Celsius(short donnees_brut) ;


int lecture_temperature_Celsius (int buffer, double * Temperature) ;

int lancement_temperature_Celsius(double *temperature, int buffer) ;


/************    ****************
*********** Temperature en Farenheit ************
***********          ***********/

//calcul de la température à partir des données brutes en Farenheit
double calcul_temperature_Farenheit(short donnees_brut);


int lecture_temperature_Farenheit(int buffer, double * Temperature) ;


int lancement_temperature_Farenheit(double *temperature, int buffer);


/************    ****************
*********** Pression en hPa(hectopascali) ************
***********          ***********/
double calcul_pression_hPa(short donnees_brut);


int ADC_to_pression_hPa(double *pression, int buffer) ;



/************    ****************
*********** Pression en mmHg (Millimètre de mercure)************
***********          ***********/
double calcul_pression_mmHg(short donnees_brut);

int ADC_to_pression_mmHg(double *pression, int buffer) ;


/************    ****************
*********** Humidité relative en pourcentages% ************
***********          ***********/

double calcul_humidite_pourcentage(short donnees_brut, double temperature);


int ADC_to_humidity_poucentage(double temperature, double *humidite, int buffer) ;


/************    ****************
*********** Humidité relative en g/m¨3 ************
***********          ***********/

double calcul_humidite_absolue(short donnees_brut, double temperature);


int ADC_to_humidity_absolue(double temperature, double *humidite, int buffer) ;
/************    ****************
*********** Humidité  ************
***********          ***********/


/************    ****************
 *********** New implementation ****
 ***********          ***********/

 double humidite_pression_temperature0(double pression, double temperature) ;


 int ADC_to_humidity0_poucentage(double temperature,double pression, double *humidite, int buffer) ;

//calcul de l'humidité à partir de la pression et de la temperature
double humidite_pression_temperature(short donnees_brut_pression,double pression,short donnees_brut_temperature, double temperature);

int ADC_to_humidity2_poucentage(double temperature,double pression, double *humidite, int buffer) ;

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
#endif
