/**
 * Permet de lire les donnees du bus i2c
 */

#ifndef __TEST__AFFICHAGE__DONNEE__
#define __TEST__AFFICHAGE__DONNEE__

/**
#include "44b.h"
#include <time.h>
#include <signal.h>
*/
#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


#include <string.h>

#define I2C_SLAVE             0x0703        // "adresse" du bus i2c
#define I2C_ADDRESS_TEMP      0x92          // Float address + 1 R/W bit low
#define I2C_FILE              "/dev/i2c0"   //fichier special i2c0

#define OCTET_ZERO            0x00          //Just un octet de 0.

// Bit de configuration du registre de temperature
#define CONFIG_TEMP_R0      0x40
#define CONFIG_TEMP_R1      0x20

#endif
