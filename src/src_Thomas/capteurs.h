#ifndef __CAPTEURS_H__
#define __CAPTEURS_H__


#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

#ifndef ADC_CHANNEL
    #define ADC_CHANNEL      0x500
#endif

#ifndef I2C_SLAVE
    #define I2C_SLAVE        0x0703
#endif

#define OCTET_ZERO            0x00          //Just un octet de 0.

// Fichier correspondant aux peripheriques de la carte
#define ADC_DEVICE           "/dev/adc"
#define I2C_DEVICE           "/dev/i2c0"

// Bit de configuration du registre de temperature
#define CONFIG_TEMP_R0      0x40
#define CONFIG_TEMP_R1      0x20

// Bit de configuration du registre de pression et humidité
//#define MWINCLUDECOLORS
#define I2C_ADDRESS_TEMP     0x92   // Float address + 1 R/W bit low
#define ADC_CHAN_PRESSURE    4
#define ADC_CHAN_HUMIDITY    5


/**
 * @struct t_captors_data
 * @brief  Structure comprenant les valeurs physiques des differents capteurs
 *         (Temperature, pression et humidite)
 */
typedef struct s_captors_data
{
    double T;       /**< Temperature (en C) */
    double P;       /**< Pression (en hPA) */
    double RH;      /**< Humidite (en %) */
} t_captors_data, * t_ptr_captors_data;

/**
 * @struct t_tendances
 * @brief  Structure comprenant les tendances des differents capteurs
 *         (Temperature, pression et humidite).
 *         Croissant si > 0 / Decroissant si < 0
 */
typedef struct s_tendances
{
    int T;          /**< Temperature (en C) */
    int P;          /**< Pression (en hPA) */
    int RH;         /**< Humidite (en %) */
} t_tendances, * t_ptr_tendances;


int get_donnees(int * buffer, double * temperature, double * humidite, double * pression);

/**
 * @brief    Lecture du niveau d'humidite.
 * @param fd Descripteur de fichier du peripherique (CAN).
 * @param T  Valeur de la temperature ambiante.
 * @param RH Parametre de sortie. Valeur de l'humidite mesuree (en pourcent).
 * @return   EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int lire_humidite(int fd, double T, double P, double * RH);
double calcul_humidite(short donnees_brut, double temperature);
int ADC_to_humidity(double temperature, double *humidite, int buffer);

/**
 * @brief    Lecture du niveau de pression.
 * @param fd Descripteur de fichier du peripherique (CAN).
 * @param P  Parametre de sortie. Valeur de la pression mesuree (en hPA).
 * @return   EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int lire_pression(int fd, double * P);
double calcul_pression(short donnees_brut);
int ADC_to_pression(double *pression, int buffer);

/**
 * @brief    Lectures des donnees de tous les capteurs.
 * @param p  Parametre de sortie. Valeurs des differentes sondes.
 * @return   EXIT_SUCCESS ou EXIT_FAILURE
 */
int lancement_temperature(double *temperature, int buffer);
int lecture_temperature (int buffer, double * Temperature);
double calcul_temperature(short donnees_brut);

#endif
