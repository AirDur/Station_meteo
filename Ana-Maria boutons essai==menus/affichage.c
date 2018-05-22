#include "affichage.h"

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
}
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

	sprintf(sT, "%lf celsius", temperature);
	sprintf(sP, "%lf bar", pression);
	sprintf(sH, "%lf %%", humidite);

	GrText(w, gc, 25, 20,"OM a perdu", 16, GR_TFASCII);
	GrLine(w, gc, 10, 40, 130, 40);
	GrText(w, gc, 10, 70," - Temperature :", 17, GR_TFASCII);
	GrText(w, gc, 10, 90,sT, 18, GR_TFASCII);
	GrText(w, gc, 10, 120," - Pression :", 14, GR_TFASCII);
	GrText(w, gc, 10, 140,sP, 15, GR_TFASCII);
	GrText(w, gc, 10, 170," - Humidite :", 14, GR_TFASCII);
	GrText(w, gc, 10, 190,sH, 12, GR_TFASCII);
	//GrLine(w, gc, 10, 210, 130, 210);

	/*Enter event loop **/
	//for(;;)
	//  GrGetNextEvent(&event);
	 GrClose();

	 //le prochain bouton
	 if ( affichage_main() != EXIT_SUCCESS )
	 {
	 perror("[main.c] affichage menu");
	 return EXIT_FAILURE;
	 }

 return EXIT_SUCCESS;
}


int affichage_main_bouton_1(){
	int buffer, ret, i=0;

	double temperature = 0.0, humidite = 0.0, pression = 0.0;

	  if ( lancement_temperature(&temperature, buffer) != EXIT_SUCCESS )
	  {
		  perror("[main.c] lancement température");
		  return EXIT_FAILURE;
	  }

	  if ( ADC_to_humidity(temperature, &humidite, buffer) != EXIT_SUCCESS )
	  {
		  perror("[main.c] lancement humidité");
		  return EXIT_FAILURE;
	  }

	  if ( ADC_to_pression(&pression, buffer) != EXIT_SUCCESS )
	  {
		  perror("[main.c] lancement pression");
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

 GR_WINDOW_ID w;
 GR_EVENT event;
 GR_GC_ID gc;
 GR_FONT_ID font;

 system("./nano-X &");


 //configuration pour les préférences d'afichage
 if (GrOpen() < 0)                            //fonction graphics routines
 {
         printf("Can't open graphics\n");
         exit(1);
     }

     // parametrès dans l'ordre parent, coordonées x et y (20,100) ,
     //largeur 120, hauteur 120 , bordersize 5 ,backgroud WHITE , bordercolor BLACK

     w = GrNewWindow(GR_ROOT_WINDOW_ID, 0, 0, 240, 240,5, WHITE, BLACK);
     GrMapWindow(w);

       //function allocates a new graphic context with all parameters set to the default values.
       gc = GrNewGC();
       //font=GrCreateFont("Helvetica",6,NULL);
      // GrSetFontRotation(font,90);

       GrText(w, gc, 0, 20,"Coucou-- Deuxieme bouton" , 6, GR_TFASCII);
       /*Enter event loop **/
      // for(;;)
      //   GrGetNextEvent(&event);

       //function flushes any buffeBLACK function calls and closes the connection created with the GrOpen function.
      GrClose();
			//le prochain bouton
	 	 if ( affichage_main() != EXIT_SUCCESS )
		 	 {
		 	 perror("[main.c] affichage menu");
		 	 return EXIT_FAILURE;
		 	 }
      return EXIT_SUCCESS;
}

int affichage_main_bouton_2(){
	int buffer, ret, i=0;

	double temperature = 0.0, humidite = 0.0, pression = 0.0;

	  if ( lancement_temperature(&temperature, buffer) != EXIT_SUCCESS )
	  {
	      perror("[main.c] lancement température");
	      return EXIT_FAILURE;
	  }

	  if ( ADC_to_humidity(temperature, &humidite, buffer) != EXIT_SUCCESS )
	  {
	      perror("[main.c] lancement humidité");
	      return EXIT_FAILURE;
	  }

	  if ( ADC_to_pression(&pression, buffer) != EXIT_SUCCESS )
	  {
	      perror("[main.c] lancement pression");
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

  GR_WINDOW_ID w;
  GR_EVENT event;
	GR_GC_ID gc;
  GR_FONT_ID font;

  system("./nano-X &");


  //configuration pour les préférences d'afichage
  if (GrOpen() < 0)                            //fonction graphics routines
  {
          printf("Can't open graphics\n");
          exit(1);
      }

      // parametrès dans l'ordre parent, coordonées x et y (20,100) ,
      //largeur 120, hauteur 120 , bordersize 5 ,backgroud WHITE , bordercolor BLACK

      w = GrNewWindow(GR_ROOT_WINDOW_ID, 0, 0, 240, 240,5, WHITE, BLACK);
      GrMapWindow(w);

        //function allocates a new graphic context with all parameters set to the default values.
        gc = GrNewGC();
      //  font=GrCreateFont("Helvetica",6,NULL);
      //  GrSetFontRotation(font,90);

        GrText(w, gc, 0, 20,"Coucou-- Troisième bouton" , 6, GR_TFASCII);
        /*Enter event loop **/
      //  for(;;)
      //    GrGetNextEvent(&event);

        //function flushes any buffeBLACK function calls and closes the connection created with the GrOpen function.
       GrClose();
			 //le prochain bouton
			 if ( affichage_main() != EXIT_SUCCESS )
			 {
			 perror("[main.c] affichage menu");
			 return EXIT_FAILURE;
			 }
       return EXIT_SUCCESS;

}

int affichage_main_bouton_3(){
	int buffer, ret, i=0;

	double temperature = 0.0, humidite = 0.0, pression = 0.0;

	if ( lancement_temperature(&temperature, buffer) != EXIT_SUCCESS )
	{
	    perror("[main.c] lancement température");
	    return EXIT_FAILURE;
	}

	if ( ADC_to_humidity(temperature, &humidite, buffer) != EXIT_SUCCESS )
	{
	    perror("[main.c] lancement humidité");
	    return EXIT_FAILURE;
	}

	if ( ADC_to_pression(&pression, buffer) != EXIT_SUCCESS )
	{
	    perror("[main.c] lancement pression");
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

  GR_WINDOW_ID w;
  GR_EVENT event;
	GR_GC_ID gc;
  GR_FONT_ID font;

  system("./nano-X &");


  //configuration pour les préférences d'afichage
  if (GrOpen() < 0)                            //fonction graphics routines
  {
          printf("Can't open graphics\n");
          exit(1);
      }

      // parametrès dans l'ordre parent, coordonées x et y (20,100) ,
      //largeur 120, hauteur 120 , bordersize 5 ,backgroud WHITE , bordercolor BLACK

      w = GrNewWindow(GR_ROOT_WINDOW_ID, 0, 0, 240, 240,5, WHITE, BLACK);
      GrMapWindow(w);

        //function allocates a new graphic context with all parameters set to the default values.
        gc = GrNewGC();
      //  font=GrCreateFont("Helvetica",6,NULL);
      //  GrSetFontRotation(font,90);

        GrText(w, gc, 0, 20,"Coucou-- Quatrieme bouton" , 6, GR_TFASCII);
        /*Enter event loop **/
      //  for(;;)
    //      GrGetNextEvent(&event);

        //function flushes any buffeBLACK function calls and closes the connection created with the GrOpen function.
       GrClose();
			 //le prochain bouton
			 if ( affichage_main() != EXIT_SUCCESS )
			 {
			 perror("[main.c] affichage menu");
			 return EXIT_FAILURE;
			 }

       return EXIT_SUCCESS;

}

int affichage_main_bouton_4(){
	int buffer, ret, i=0;

	double temperature = 0.0, humidite = 0.0, pression = 0.0;

	  if ( lancement_temperature(&temperature, buffer) != EXIT_SUCCESS )
	  {
	      perror("[main.c] lancement température");
	      return EXIT_FAILURE;
	  }

	  if ( ADC_to_humidity(temperature, &humidite, buffer) != EXIT_SUCCESS )
	  {
	      perror("[main.c] lancement humidité");
	      return EXIT_FAILURE;
	  }

	  if ( ADC_to_pression(&pression, buffer) != EXIT_SUCCESS )
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
