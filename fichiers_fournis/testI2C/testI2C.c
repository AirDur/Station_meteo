/**
 * @file     	testI2C.c
 * @author   	Patrick BREUGNON
 * @brief    	Fontion permettant de tester l'interface i2c
 *		Ne pas oublié de charger le driver i2c en effectuant
 *		la commande
 *		# insmod i2c_s3c44.o
 * 		On peut vérifier si le driver est chargé par la présence
 *		du fichier spécial situé dans /dev
 *		# ls /dev/i2c0c
 */


#include "44b.h"
#include<time.h>
#include<signal.h>

#include <stdio.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define I2C_SLAVE             0x0703

int main(int argc, char**argv)
{
	int fd, ret;
	unsigned char val[2];

	val[0]=0x01;
	val[1]=0x60;



	//Permet d'acceder aux données du capteur de température

	if ((fd = open("/dev/i2c0",O_RDWR)) < 0 ) //on ouvre le fichier special i2c0 en lecture et ecriture
	{
		perror("open");
		printf("file /dev/i2c0 doesn't exist");
		printf("you must type the command");
		printf("insmod i2c_s3c44.o");
		return (-1);
	}

	ret = ioctl(fd, I2C_SLAVE, 0x92);
	//0x92 c'est l'adresse
        if(ret <0)
        {    //si le fichier n'existe pas on affiche un message d'erreur
             perror("ADC_CHANNEL ioctl cmd");
             close(fd);
             return (-1);
	}

	write(fd, val ,2);//ecriture de deux valeurs sur fd
	read(fd, val, 1);//lecture d'une valeur sur fd
	printf("val[%d] = %x\n", 0x92, val[0]);

	val[0]=0x00;
	write(fd, val ,1);
	read(fd, val, 2);
	return 0;
}
