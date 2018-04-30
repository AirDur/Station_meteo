/**
 * @file     capteurs.c
 *
 * @author   GOURDON Arnaud
 *           PREVOST Theophile
 *
 * @brief    Fonctions permettant la lecture des donnees des capteurs.
 */

#include "capteurs.h"


#define ADC_CHAN_PRESSURE    4
#define ADC_CHAN_HUMIDITY    5

#define I2C_ADDRESS_TEMP     0x92   // Float address + 1 R/W bit low


int config_temperature(int fd, unsigned char mask)
{
    int ret;
    unsigned char octets[2];

    ret = ioctl(fd, I2C_SLAVE, I2C_ADDRESS_TEMP);   // Configuration de l'adresse
    if(ret < 0)
    {
        perror("[Config Temp] Configuration de l'adresse I2C");
        return EXIT_FAILURE;
    }

    octets[0] = 0x01;                               // Configuration Register
    octets[1] = mask;                               // Masque de configuration

    ret = write(fd, octets, 2);                     // Ecriture sur le bus I2C
    if (ret < 0)
    {
        perror("[Config Temp] Ecriture sur le bus I2C");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

} // config_temperature

int lire_temperature(int fd, double * T)
{
    int ret;
    unsigned char octets[2];
    short data;

    ret = ioctl(fd, I2C_SLAVE, I2C_ADDRESS_TEMP);   // Configuration de l'adresse
    if(ret < 0)
    {
        perror("[Lire Temp] Configuration de l'adresse I2C");
        return EXIT_FAILURE;
    }

    octets[0] = 0x00;                               // Temperature Register

    ret = write(fd, octets, 1);                     // Ecriture sur le bus I2C
    if (ret < 0)
    {
        perror("[Lire Temp] Configuration du registre");
        return EXIT_FAILURE;
    }

    ret = read(fd, octets, 2);                      // Lecture Temperature Register
    if (ret < 0)
    {
        perror("[Lire Temp] Lecture du registre de temperature");
        return EXIT_FAILURE;
    }

    data = (octets[0] << 4) | (octets[1] >> 4);
    *T = data * 0.0625;                             // 0.0625 = 128 / 2048
    
    return EXIT_SUCCESS;

} // lire_temperature

int lire_humidite(int fd, double T, double * RH)
{
    int ret;
    short data;
    double Vout, Vs;

    ret = ioctl(fd, ADC_CHANNEL, ADC_CHAN_HUMIDITY); // Configuration du canal
    if(ret < 0)
    {
        perror("[Lire Humidite] Configuration du canal");
        return EXIT_FAILURE;
    }

    read(fd, &data, 2);                             // Lecture de la valeur du capteur

    // Calcul humidite physique
    Vout = (2.5 / 1024) * data * 2;
    Vs = 5;
    *RH = (Vout - Vs * 0.16) / (Vs * 0.0062);       // Calcul humidite
    *RH = *RH / (1.0546 - 0.00216 * T);             // Calcul de l'humidite avec T

    return EXIT_SUCCESS;

} // lire_humidite

int lire_pression(int fd, double * P)
{
    int ret;
    short data;
    double Vout, Vs;

    ret = ioctl(fd, ADC_CHANNEL, ADC_CHAN_PRESSURE); // Configuration du canal
    if(ret < 0)
    {
        perror("[Lire Pression] Configuration du canal");
        return EXIT_FAILURE;
    }

    read(fd, &data, 2);                             // Lecture de la valeur du capteur

    // Calcul pression physique
    Vout = (2.5 / 1024) * data * 2;
    Vs = 5.1;
    *P = (Vout + Vs * 0.1518) / (Vs * 0.01059) * 10;    // Calcul de la pression

    return EXIT_SUCCESS;

} // lire_pression

int lire_donnees_capteurs(t_ptr_captors_data p)
{
    int fd, r;

    // TEMPERATURE
    if ((fd = open(I2C_DEVICE, O_RDWR)) < 0)
    {
        perror("[I2C] Ouverture du peripherique");
        return EXIT_FAILURE;
    }

    r = config_temperature(fd, CONFIG_TEMP_R0 | CONFIG_TEMP_R1);
    if (r != EXIT_SUCCESS)
    {
        perror("[I2C] Configuration registre de temperature");
        return EXIT_FAILURE;
    }
    
    r = lire_temperature(fd, &p->T);
    if (r != EXIT_SUCCESS)
    {
        perror("[I2C] Recuperation temperature");
        return EXIT_FAILURE;
    }

    close(fd);

    // PRESSION ET HUMIDITE
    if ((fd = open(ADC_DEVICE, O_RDONLY)) < 0)
    {
        perror("[ADC] Ouverture du peripherique");
        return EXIT_FAILURE;
    }

    r = lire_pression(fd, &p->P);
    if (r != EXIT_SUCCESS)
    {
        perror("[ADC] Recuperation pression");
        return EXIT_FAILURE;
    }

    r = lire_humidite(fd, p->T, &p->RH);
    if (r != EXIT_SUCCESS)
    {
        perror("[ADC] Recuperation humidite");
        return EXIT_FAILURE;
    }

    close(fd);
    
    return EXIT_SUCCESS;

} // lire_donnees_capteurs
