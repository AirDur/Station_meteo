#include "affichage.h"
/*** ???? necessaire on va voir le test
int affichage_main(){

		if(Push_Button() == 1)
		{
			printf("===Premier button appuyé===\n") ;
			 if ( affichage_main_bouton_1() != EXIT_SUCCESS )
				{
					perror("[main.c] affichage bouton 1");
					return EXIT_FAILURE;
				}
	  }
	if(Push_Button() == 2)
	{
		printf("===Deuxième button appuyé===\n") ;
		 if ( affichage_main_bouton_2() != EXIT_SUCCESS )
			{
			perror("[main.c] affichage bouton 2");
			return EXIT_FAILURE;
			}
	}
	if(Push_Button() == 3)
	{
		printf("===Troisième button appuyé===\n") ;
		 if ( affichage_main_bouton_3() != EXIT_SUCCESS )
		{
		perror("[main.c] affichage bouton 3");
		return EXIT_FAILURE;
		}
	}
	if(Push_Button() == 4)
	{
		printf("===Quatrième button appuyé===\n") ;
		 if ( affichage_main_bouton_4() != EXIT_SUCCESS )
			{
			perror("[main.c] affichage bouton 4");
			return EXIT_FAILURE;
			}
	}

}***/
/************    ****************
*********** Bouton 1 ************
***********          ***********/

int affichage_bouton1(double temperature, double pression, double humidite) {

	GR_WINDOW_ID  w;
	GR_EVENT      event;
	GR_GC_ID      gc;
	GR_FONT_ID    font;
	char sT[16], sP[16], sH[16];

	system("./nano-X &");


	//configuration pour les préférences d'afichage
	if (GrOpen() < 0)                            //fonction graphics routines
	{
		printf("Can't open graphics\n");
		return EXIT_FAILURE;
	}

	// parametrès dans l'ordre parent, coordonées x et y (20,100) ,
	//largeur 120, hauteur 120 , bordersize 5 ,backgroud WHITE , bordercolor BLACK

	w = GrNewWindow(1, 10, 20, 140, 200, 5, BLACK, WHITE);
	GrMapWindow(w);

	gc = GrNewGC();

	//font=GrCreateFont("Helvetica",6,NULL);
	//GrSetFontRotation(font, 90);

	sprintf(sT, "%lf Celsius", temperature);
	sprintf(sP, "%lf hPa", pression);
	sprintf(sH, "%lf %%", humidite);

	GrText(w, gc, 25, 20,"Valeurs capteurs", 16, GR_TFASCII);
	GrLine(w, gc, 10, 40, 130, 40);
	GrText(w, gc, 10, 70," - Temperature :", 17, GR_TFASCII);
	GrText(w, gc, 10, 90,sT, 18, GR_TFASCII);
	GrText(w, gc, 10, 120," - Pression :", 14, GR_TFASCII);
	GrText(w, gc, 10, 140,sP, 15, GR_TFASCII);
	GrText(w, gc, 10, 170," - Humidite :", 14, GR_TFASCII);
	GrText(w, gc, 10, 190,sH, 12, GR_TFASCII);
	//GrLine(w, gc, 10, 210, 130, 210);

	/*Enter event loop il n'y a pas de loop?????? si GrDestroyWindow(w); **/
	for(;;){
	 GrGetNextEvent(&event);
	  switch (event.type) {
	 case GR_EVENT_TYPE_CLOSE_REQ:
		 GrClose();
		 exit(0);
 }
}
return EXIT_SUCCESS;
	 //le prochain bouton
/**	 if ( affichage_main() != EXIT_SUCCESS )
	 {
	 perror("[main.c] affichage menu");
	 return EXIT_FAILURE;
	 }
 **/
}


int affichage_main_bouton_1(){
	int buffer, ret, i=0;

	double temperature = 0.0, humidite = 0.0, pression = 0.0;

	  if ( lancement_temperature_Celsius(&temperature, buffer) != EXIT_SUCCESS )
	  {
		  perror("[main.c] lancement température");
		  return EXIT_FAILURE;
	  }

		if ( ADC_to_pression_hPa(&pression, buffer) != EXIT_SUCCESS )
			  {
				  perror("[main.c] lancement pression");
				  return EXIT_FAILURE;
			  }

	//  if ( ADC_to_humidity_poucentage(temperature, &humidite, buffer) != EXIT_SUCCESS )
			if ( ADC_to_humidity2_poucentage (temperature,pression, &humidite,buffer) != EXIT_SUCCESS )
				{
				  perror("[main.c] lancement humidité");
				  return EXIT_FAILURE;
			  }

	  close(buffer);

	  if ( affichage_bouton1(temperature, pression, humidite) != EXIT_SUCCESS )
	  {
		  perror("[main.c] affichage");
		  return EXIT_FAILURE;
	  }
		return EXIT_SUCCESS;
}

/************    ****************
*********** Bouton 2 ************
***********          ***********/

int affichage_bouton2(double temperature, double pression, double humidite) {
	GR_WINDOW_ID  w;
	GR_EVENT      event;
	GR_GC_ID      gc;
	GR_FONT_ID    font;
	char sT[16], sP[16], sH[16];

	system("./nano-X &");


	//configuration pour les préférences d'afichage
	if (GrOpen() < 0)                            //fonction graphics routines
	{
		printf("Can't open graphics\n");
		return EXIT_FAILURE;
	}

	// parametrès dans l'ordre parent, coordonées x et y (20,100) ,
	//largeur 120, hauteur 120 , bordersize 5 ,backgroud WHITE , bordercolor BLACK

	w = GrNewWindow(1, 10, 20, 140, 200, 5, BLACK, WHITE);
	GrMapWindow(w);

	gc = GrNewGC();

	sprintf(sT, "%lf Farenheit", temperature);
	sprintf(sP, "%lf mmHg", pression);
	sprintf(sH, "%lf absolue", humidite);

	GrText(w, gc, 25, 20,"Valeurs capteurs", 16, GR_TFASCII);
	GrLine(w, gc, 10, 40, 130, 40);
	GrText(w, gc, 10, 70," - Temperature :", 17, GR_TFASCII);
	GrText(w, gc, 10, 90,sT, 18, GR_TFASCII);
	GrText(w, gc, 10, 120," - Pression :", 14, GR_TFASCII);
	GrText(w, gc, 10, 140,sP, 15, GR_TFASCII);
	GrText(w, gc, 10, 170," - Humidite :", 14, GR_TFASCII);
	GrText(w, gc, 10, 190,sH, 12, GR_TFASCII);
	//GrLine(w, gc, 10, 210, 130, 210);

	/*Enter event loop il n'y a pas de loop?????? si GrDestroyWindow(w); **/
	for(;;){
	 GrGetNextEvent(&event);
		switch (event.type) {
	 case GR_EVENT_TYPE_CLOSE_REQ:
		 GrClose();
		 exit(0);
 }
}
return EXIT_SUCCESS;
/**
	 //le prochain bouton
	 if ( affichage_main() != EXIT_SUCCESS )
	 {
	 perror("[main.c] affichage menu");
	 return EXIT_FAILURE;
 }**/
}

int affichage_main_bouton_2(){
	int buffer, ret, i=0;

	double temperature = 0.0, humidite = 0.0, pression = 0.0;

		if ( lancement_temperature_Farenheit(&temperature, buffer) != EXIT_SUCCESS )
			{
				perror("[main.c] lancement température");
				return EXIT_FAILURE;
			}

		if ( ADC_to_pression_mmHg(&pression, buffer) != EXIT_SUCCESS )
				{
					perror("[main.c] lancement pression");
					return EXIT_FAILURE;
				}

		if ( ADC_to_humidity_absolue(temperature, &humidite, buffer) != EXIT_SUCCESS )
			{
				perror("[main.c] lancement humidité");
				return EXIT_FAILURE;
			}

		close(buffer);

		if ( affichage_bouton2(temperature, pression, humidite) != EXIT_SUCCESS )
			{
				perror("[main.c] affichage");
				return EXIT_FAILURE;
			}
		return EXIT_SUCCESS;
}

/************    ****************
*********** Bouton 3 ************
***********          ***********/

int affichage_bouton3(double temperature, double pression, double humidite) {
	GR_WINDOW_ID  w;
	GR_EVENT      event;
	GR_GC_ID      gc;
	GR_FONT_ID    font;
	char sT[16], sP[16], sH[16];

	system("./nano-X &");


	//configuration pour les préférences d'afichage
	if (GrOpen() < 0)                            //fonction graphics routines
	{
		printf("Can't open graphics\n");
		return EXIT_FAILURE;
	}

	// parametrès dans l'ordre parent, coordonées x et y (20,100) ,
	//largeur 120, hauteur 120 , bordersize 5 ,backgroud WHITE , bordercolor BLACK

	w = GrNewWindow(1, 10, 20, 140, 200, 5, BLACK, WHITE);
	GrMapWindow(w);

	gc = GrNewGC();

	sprintf(sT, "%lf Celsius", temperature);
	sprintf(sP, "%lf mmHg", pression);
	sprintf(sH, "%lf absolue", humidite);

	GrText(w, gc, 25, 20,"Valeurs capteurs", 16, GR_TFASCII);
	GrLine(w, gc, 10, 40, 130, 40);
	GrText(w, gc, 10, 70," - Temperature :", 17, GR_TFASCII);
	GrText(w, gc, 10, 90,sT, 18, GR_TFASCII);
	GrText(w, gc, 10, 120," - Pression :", 14, GR_TFASCII);
	GrText(w, gc, 10, 140,sP, 15, GR_TFASCII);
	GrText(w, gc, 10, 170," - Humidite :", 14, GR_TFASCII);
	GrText(w, gc, 10, 190,sH, 12, GR_TFASCII);
	//GrLine(w, gc, 10, 210, 130, 210);

	/*Enter event loop il n'y a pas de loop?????? si GrDestroyWindow(w); **/
	for(;;){
	 GrGetNextEvent(&event);
		switch (event.type) {
	 case GR_EVENT_TYPE_CLOSE_REQ:
		 GrClose();
		 exit(0);
 }
}
return EXIT_SUCCESS;
	 //le prochain bouton
	/** if ( affichage_main() != EXIT_SUCCESS )
	 {
	 perror("[main.c] affichage menu");
	 return EXIT_FAILURE;
 }**/
}

int affichage_main_bouton_3(){
	int buffer, ret, i=0;

	double temperature = 0.0, humidite = 0.0, pression = 0.0;

	if ( lancement_temperature_Celsius(&temperature, buffer) != EXIT_SUCCESS )
	{
	    perror("[main.c] lancement température");
	    return EXIT_FAILURE;
	}

	if ( ADC_to_pression_mmHg(&pression, buffer) != EXIT_SUCCESS )
	{
	    perror("[main.c] lancement pression");
	    return EXIT_FAILURE;
	}
	if ( ADC_to_humidity_absolue(temperature, &humidite, buffer) != EXIT_SUCCESS )
		{
		    perror("[main.c] lancement humidité");
		    return EXIT_FAILURE;
		}
	close(buffer);

	if ( affichage_bouton3(temperature, pression, humidite) != EXIT_SUCCESS )
	{
	    perror("[main.c] affichage");
	    return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

/************    ****************
*********** Bouton 4 ************
***********          ***********/
int affichage_bouton4(double temperature, double pression, double humidite) {
	GR_WINDOW_ID  w;
	GR_EVENT      event;
	GR_GC_ID      gc;
	GR_FONT_ID    font;
	char sT[16], sP[16], sH[16];

	system("./nano-X &");


	//configuration pour les préférences d'afichage
	if (GrOpen() < 0)                            //fonction graphics routines
	{
		printf("Can't open graphics\n");
		return EXIT_FAILURE;
	}

	// parametrès dans l'ordre parent, coordonées x et y (20,100) ,
	//largeur 120, hauteur 120 , bordersize 5 ,backgroud WHITE , bordercolor BLACK

	w = GrNewWindow(1, 10, 20, 140, 200, 5, BLACK, WHITE);
	GrMapWindow(w);

	gc = GrNewGC();

	sprintf(sT, "%lf Farenheit", temperature);
	sprintf(sP, "%lf hPa", pression);
	sprintf(sH, "%lf %%", humidite);

	GrText(w, gc, 25, 20,"Valeurs capteurs", 16, GR_TFASCII);
	GrLine(w, gc, 10, 40, 130, 40);
	GrText(w, gc, 10, 70," - Temperature :", 17, GR_TFASCII);
	GrText(w, gc, 10, 90,sT, 18, GR_TFASCII);
	GrText(w, gc, 10, 120," - Pression :", 14, GR_TFASCII);
	GrText(w, gc, 10, 140,sP, 15, GR_TFASCII);
	GrText(w, gc, 10, 170," - Humidite :", 14, GR_TFASCII);
	GrText(w, gc, 10, 190,sH, 12, GR_TFASCII);
	//GrLine(w, gc, 10, 210, 130, 210);

	/*Enter event loop il n'y a pas de loop?????? si GrDestroyWindow(w);??? **/
	for(;;){
	 GrGetNextEvent(&event);
		switch (event.type) {
	 case GR_EVENT_TYPE_CLOSE_REQ:
		 GrClose();
		 exit(0);
 }
}
return EXIT_SUCCESS;
	 //le prochain bouton
	/** if ( affichage_main() != EXIT_SUCCESS )
	 {
	 perror("[main.c] affichage menu");
	 return EXIT_FAILURE;
 }**/
}

int affichage_main_bouton_4(){
	int buffer, ret, i=0;

	double temperature = 0.0, humidite = 0.0, pression = 0.0;

	  if ( lancement_temperature_Farenheit(&temperature, buffer) != EXIT_SUCCESS )
	  {
	      perror("[main.c] lancement température");
	      return EXIT_FAILURE;
	  }

	 // if ( ADC_to_humidity_poucentage(temperature, &humidite, buffer) != EXIT_SUCCESS )
		if ( ADC_to_humidity2_poucentage (temperature,pression, &humidite,buffer) != EXIT_SUCCESS )
			{
		      perror("[main.c] lancement humidité");
		      return EXIT_FAILURE;
		  }

	  if ( ADC_to_pression_hPa(&pression, buffer) != EXIT_SUCCESS )
	  {
	      perror("[main.c] lancement pression");
	      return EXIT_FAILURE;
	  }

	  close(buffer);

	  if ( affichage_bouton4(temperature, pression, humidite) != EXIT_SUCCESS )
	  {
	      perror("[main.c] affichage");
	      return EXIT_FAILURE;
	  }
	return EXIT_SUCCESS;
}
