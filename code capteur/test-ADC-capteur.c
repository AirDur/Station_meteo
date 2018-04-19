/**
 * @file     test-ADC-capteur.c
 *
 * @author   GATTAZ Thomas
 *
 * @brief    Fonctions permettant la lecture des donnees des capteurs ADC.
 */

#include "capteurs.h"

#define ADC_CHANNEL           0x500

#define ADC_CHAN_PRESSURE    4
#define ADC_CHAN_HUMIDITY    5

void main()
{
	char adc_device[20] ;

  ret = ioctl(fd, ADC_CHANNEL, ADC_CHAN_HUMIDITY); //configuration du canal
}
