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


int affichage_current_data(GR_WINDOW_ID, GR_GC_ID, double *, double *, double *);

#endif
