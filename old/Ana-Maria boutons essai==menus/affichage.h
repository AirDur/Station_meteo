#ifndef AFFICHAGE_H
#define AFFICHAGE_H


#include "nano-X.h"

#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <float.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <graphics.h>

#define BLACK			MWRGB( 0  , 0  , 0   )
#define WHITE			MWRGB( 255, 255, 255 )


//int affichage_main();
/************    ****************
*********** Bouton 1 ************
***********          ***********/
int affichage_bouton1(double temperature, double pression, double humidite);
int affichage_main_bouton_1();

/************    ****************
*********** Bouton 2 ************
***********          ***********/
int affichage_bouton2(double temperature, double pression, double humidite);
int affichage_main_bouton_2();

/************    ****************
*********** Bouton 3 ************
***********          ***********/
int affichage_bouton3(double temperature, double pression, double humidite);
int affichage_main_bouton_3();

/************    ****************
*********** Bouton 4 ************
***********          ***********/
int affichage_bouton4(double temperature, double pression, double humidite);
int affichage_main_bouton_4();

#endif
