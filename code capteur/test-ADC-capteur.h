/**
 * @file     test-ADC-capteur.h
 *
 * @author   GATTAZ THomas
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
