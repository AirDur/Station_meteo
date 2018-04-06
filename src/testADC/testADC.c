/**
 * @file     	testADC.c
 * @author   	Patrick BREUGNON
 * @brief    	Fontion permettant de tester les interfaces ADCs
 *		puis d'afficher la valeurs de tensions présentent
 *		aux entrées.
 *		Ne pas oublier de charger le driver ADC en effectuant 
 *		la commande 
 *		# insmod adc_s3c44.o
 * 		On peut vérifier si le driver est chargé par la présence 
 *		du fichier spécial situé dans /dev
 *		# ls /dev/adc
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


#define ADC_VERSION           0x400
#define ADC_SAMPLING          0x800
#define ADC_CHANNEL           0x500

// tapez la commande
// # testADC /dev/adc 1
// pour lire la tension présente à l'entrée 1

int main(int argc, char**argv)
{
	short data;
	int fd, ret, n;
	char adc_device[20] ;

	if (argc < 3) 
	{
		printf("Bad command\n");
		printf("Usage   : testADC [DEVICE]  [channel]\n");
		printf("Exemple : testADC /dev/adc 1\n");
		return (-1);
	}
	strcpy(adc_device, argv[1]) ;


	if ((fd = open(adc_device,O_RDWR)) < 0 ) 
	{
		perror("open");
		printf("Error opening %s\n", adc_device);
		return (-1);
	}

	sscanf(argv[2], "%x", &n);
	printf("\n--- Test de lecture ADC channel %d ---\n\n", n);
	ret = ioctl(fd, ADC_CHANNEL, n);
        if(ret <0)
        {
             perror("ADC_CHANNEL ioctl cmd");
             close(fd);
             return (-1);
	}

	read(fd, &data, 2);
	printf("val[%d] = %d\n", n, (int)data);
	close(fd);
	return 0;
}


