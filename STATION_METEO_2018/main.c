#include "capteurs.h"


int main(int argc, char**argv)
{
  int code_erreur, buffer;
  double temperature;
  short data ;
  int fd, ret;
  double RH, P;
  double Vout, Vs;

  GR_WINDOW_ID w;
  GR_GC_ID gc;
  GR_EVENT event;
  char sT[64], sP[64], sRH[64];

  temperature = 0;

  //Ouverture du perifique i2c :
  if ((buffer = open(I2C_FILE, O_RDWR)) < 0)
  {
      perror("[testTemperature] /dev/i2c0 n'existe pas : le driver n'est pas installe.");
      return EXIT_FAILURE;
  }

  code_erreur = configuration_i2c(buffer, CONFIG_TEMP_R0 | CONFIG_TEMP_R1);
  if (code_erreur != EXIT_SUCCESS)
  {
      perror("[testTemperature] Configuration registre de temperature");
      return EXIT_FAILURE;
  }

  code_erreur = lecture_temperature(buffer, &temperature);
  if (code_erreur != EXIT_SUCCESS)
  {
      perror("[testTemperature] Recuperation temperature");
      return EXIT_FAILURE;
  }
  sleep(2);

  //RECUPERATION DONNES CAPTEUR HUMIDITE/PRESSION
  if ((fd = open(ADC_DEVICE,O_RDONLY)) < 0 )
	{
		printf("[TEST ADC CAPTEUR] Problème d'ouverture de : %s\n", ADC_DEVICE);
    return EXIT_FAILURE;
	}

  //CONFIGURATION DU CANAL POUR L'HUMIDITE
  ret = ioctl(fd, ADC_CHANNEL, ADC_CHAN_HUMIDITY); //configuration du canal
  if(ret < 0)
  {
    perror("[TEST ADC CAPTEUR] Configuration du canal Humidité");
    return EXIT_FAILURE;
  }

  read(fd, &data, 2);  // Lecture de la valeur du capteur

  // Calcul humidite physique
  Vout = (2.5 / 1024) * data * 2;
  Vs = 5;
  RH = (Vout - Vs * 0.16) / (Vs * 0.0062);  // Calcul humidite
  RH = RH / (1.0546 - 0.00216 * temperature); // Calcul de l'humidite avec T




  //CONFIGURATION DU CANAL POUR LA PRESSION
  ret = ioctl(fd, ADC_CHANNEL, ADC_CHAN_PRESSURE); // Configuration du canal
  if(ret < 0)
  {
      perror("[[TEST ADC CAPTEUR] Configuration du canal Pression");
      return EXIT_FAILURE;
  }

  read(fd, &data, 2);  // Lecture de la valeur du capteur

  Vout = (2.5 / 1024) * data * 2;
  Vs = 5.1;
  P = (Vout + Vs * 0.1518) / (Vs * 0.01059) * 10; // Calcul de la pression



  //FERMETURE DU PROGRAMME
  close(fd);


  //Affichage humidité
  printf(" - Valeur de la pression : %lf \n", P);
  //Affichage humidité
  printf(" - Valeur de l'humidité : %lf \n", RH);



  if (GrOpen() < 0)
  {
    printf("Can't open graphics\n");
    exit(1);
  }

  // Configuration de l'ecran
  w = GrNewWindow(1, 0, 0, 160, 240, 0, BLACK, WHITE);
  gc = GrNewGC();
  GrSetGCForeground(gc, WHITE);
  GrSetGCUseBackground(gc, GR_FALSE);
  GrSelectEvents(w, GR_EVENT_MASK_EXPOSURE);
  GrMapWindow(w);


  sprintf(sT, "%.1f °C\n",temperature);
  sprintf(sP, "%.1f bar\n",P);
  sprintf(sRH, "%.1f %%\n",RH);

  GrFillRect(w,gc,0,0,160,4);
  GrFillRect(w,gc,0,4,4,240);
  GrFillRect(w,gc,0,236,160,4);
  GrFillRect(w,gc,156,4,4,240);
  GrFillRect(w,gc,0,40,160,2);
  GrFillRect(w,gc,80,40,2,200);
  GrFillRect(w,gc,80,140,80,2);
  GrText(w, gc, 50, 30, "DURET le sang", -1, GR_TFASCII);

  GrText(w, gc, 15, 145,  sT,  -1, GR_TFASCII);
  GrText(w, gc, 90, 100, sP,  -1, GR_TFASCII);
  GrText(w, gc, 90, 190, sRH, -1, GR_TFASCII);



  return EXIT_SUCCESS;
}
