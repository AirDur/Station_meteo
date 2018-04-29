/**
 * @file     test-ADC-capteur.h
 *
 * @author   GATTAZ THomas
             DURET Romain
 *
 * @brief    Fonctions permettant la lecture des donnees des capteurs.
 */

#ifndef __CAPTEURS_H__
#define __CAPTEURS_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

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
//extern int lire_donnees_capteurs(t_ptr_captors_data p);


#endif /* __CAPTEURS_H__ */