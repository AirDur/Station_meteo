#ifndef __CAPTEURS_H__
#define __CAPTEURS_H__


#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
//TODO #include <pthread.h>


#ifndef ADC_CHANNEL
    #define ADC_CHANNEL      0x500
#endif

#ifndef I2C_SLAVE
    #define I2C_SLAVE        0x0703
#endif

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


/**
 * @brief      Permet de modifier le registre de configuration
 *             de la sonde de temperature.
 * @param fd   Descripteur de fichier du peripherique (bus I2C).
 * @param mask Masque de bits de configuration de la forme :
                    OS  R1  R0  F1  F0  POL  TM  S
               avec :
                    - OS : OS/ALERT
                    - R1/R0 : Converter resolution
                    - F1/F0 : Fault queue mode
                    - POL : Polarity mode
                    - TM : Thermostat mode
                    - SD : Shutdown mode
 * @return     EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int config_temperature(int fd, unsigned char mask);

/**
 * @brief    Lecture de la temperature.
 * @param fd Descripteur de fichier du peripherique (bus I2C).
 * @param T  Parametre de sortie. Valeur de la temperature mesuree (en C).
 * @return   EXIT_SUCCESS ou EXIT_FAILURE
 */
extern int lire_temperature(int fd, double * T);

/**
 * @brief    Lecture du niveau d'humidite.
 * @param fd Descripteur de fichier du peripherique (CAN).
 * @param T  Valeur de la temperature ambiante.
 * @param RH Parametre de sortie. Valeur de l'humidite mesuree (en pourcent).
 * @return   EXIT_SUCCESS ou EXIT_FAILURE
 */
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
extern int lire_donnees_capteurs(t_ptr_captors_data p);


#endif /* __CAPTEURS_H__ */
