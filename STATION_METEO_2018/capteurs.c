/**
 * @file     capteurs.c
 *
 * @author   GATTAZ Thomas
 *           DURET Romain
 *
 * @brief    Toutes les fonctions permettant la lecture des données des capteurs.
 */

 #include "capteurs.h"

 #define ADC_CHAN_PRESSURE    4
 #define ADC_CHAN_HUMIDITY    5

 int configuration_i2c(int buffer, unsigned char mask)
 {
   int code_erreur;
   unsigned char bus[2];

   code_erreur = ioctl(buffer, I2C_SLAVE, I2C_ADDRESS_TEMP);
   if(code_erreur < 0)
   {    //si le fichier n'existe pas on affiche un message d'erreur
        perror("[configuration_i2c] ADC_CHANNEL ioctl cmd");
        close(buffer);
        return EXIT_FAILURE;
   }

   bus[0] = 0x01;                               // Configuration Register
   bus[1] = mask;                               // Masque de configuration


   code_erreur = write(buffer, bus, 2); //ecriture de deux valeurs sur fd
   if (code_erreur < 0)
   {
       perror("[configuration_i2c] Erreur d'ecriture sur le bus i2c ");
       return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
 }

 int lecture_temperature (int buffer, double * Temperature) {
   int code_erreur;
   unsigned char bus[2];
   short donnees_brut;

   code_erreur = ioctl(buffer, I2C_SLAVE, I2C_ADDRESS_TEMP);
   if(code_erreur < 0)
   {
        perror("[lecture_temperature] Probleme de configuration de i2c");
        close(buffer);
        return EXIT_FAILURE;
   }


 	bus[0] = OCTET_ZERO;

   code_erreur = write(buffer, bus, 1); //ecriture de deux valeurs sur fd
   if (code_erreur < 0)
   {
     perror("[lecture_temperature] Probleme de configuration des registres");
     close(buffer);
     return EXIT_FAILURE;
   }
   code_erreur = read(buffer, bus, 2); //ecriture de deux valeurs sur fd
   if (code_erreur < 0)
   {
     perror("[lecture_temperature] Probleme de lecture du registre de temperature");
     close(buffer);
     return EXIT_FAILURE;
   }

   donnees_brut = (bus[0] << 4) | (bus[1] >> 4);
   *Temperature = donnees_brut * 0.0625;                          // 0.0625 = 128 / 2048

 	printf("temperature : %lf\n", *Temperature);

 	return EXIT_SUCCESS;
 }

 int main()
 {
   short data ;
   int fd, ret ;
   int code_erreur, buffer;
   double RH, P, T;
   double Vout, Vs, temperature;

   while(1) {
     T = 0;



     //RECUPERATION DE DONNEES CAPTEUR TEMPERATURE
     if ((buffer = open(I2C_FILE, O_RDWR)) < 0)
     {
         perror("[I2C CAPTEUR] /dev/i2c0 n'existe pas : le driver n'est pas installe.");
         return EXIT_FAILURE;
     }

     //CONFIGURATION DU CANAL POUR LA TEMPERATURE
     code_erreur = configuration_i2c(buffer, CONFIG_TEMP_R0 | CONFIG_TEMP_R1);
     if (code_erreur != EXIT_SUCCESS)
     {
         perror("[I2C CAPTEUR] Configuration registre de temperature");
         return EXIT_FAILURE;
     }

     code_erreur = lecture_temperature(buffer, &T);
     if (code_erreur != EXIT_SUCCESS)
     {
         perror("[I2C CAPTEUR] Recuperation temperature");
         return EXIT_FAILURE;
     }
     sleep(2);
   }

   //RECUPERATION DONNEES CAPTEUR HUMIDITE/PRESSION
   if ((fd = open(ADC_DEVICE,O_RDONLY)) < 0 )
 	{
 		printf("[ADC CAPTEUR] Problème d'ouverture de : %s\n", ADC_DEVICE);
     return EXIT_FAILURE;
 	}

   //CONFIGURATION DU CANAL POUR L'HUMIDITE
   ret = ioctl(fd, ADC_CHANNEL, ADC_CHAN_HUMIDITY); //configuration du canal
   if(ret < 0)
   {
     perror("[ADC CAPTEUR] Configuration du canal Humidité");
     return EXIT_FAILURE;
   }

   read(fd, &data, 2);  // Lecture de la valeur du capteur

   // Calcul humidite physique
   Vout = (2.5 / 1024) * data * 2;
   Vs = 5;
   RH = (Vout - Vs * 0.16) / (Vs * 0.0062);  // Calcul humidite
   RH = RH / (1.0546 - 0.00216 * T); // Calcul de l'humidite avec T

   //Affichage humidité
   printf("Valeur de l'humidité : %lf \n", RH);


   //CONFIGURATION DU CANAL POUR LA PRESSION
   ret = ioctl(fd, ADC_CHANNEL, ADC_CHAN_PRESSURE); // Configuration du canal
   if(ret < 0)
   {
       perror("[[ADC CAPTEUR] Configuration du canal Pression");
       return EXIT_FAILURE;
   }

   read(fd, &data, 2);  // Lecture de la valeur du capteur

   Vout = (2.5 / 1024) * data * 2;
   Vs = 5.1;
   P = (Vout + Vs * 0.1518) / (Vs * 0.01059) * 10; // Calcul de la pression

   //Affichage humidité
   printf("Valeur de la pression : %lf \n", P);

   //FERMETURE DU PROGRAMME
   close(fd);
   return EXIT_SUCCESS;
 }
