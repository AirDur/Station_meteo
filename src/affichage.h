#ifndef AFFICHAGE_H
#define AFFICHAGE_H


#include "nano-X.h"

#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <pthread.h>
#include <string.h>
#include <graphics.h>

#define BLACK			MWRGB( 0  , 0  , 0   )
#define WHITE			MWRGB( 255, 255, 255 )

void affichage(double temperature, double pression, double humidite);

#endif
