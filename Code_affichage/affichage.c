/**
 * @file     	affichage.c
 * @author
 * @brief    	Fontion de base permettant d'afficher sur l'écran un message
 */

#include "affichage.h"

int main(int acgc,char **argv)
{
  	GR_WINDOW_ID window;
    GR_GC_ID gc;
    GR_EVENT event;


    unsigned int KbStatus;        //test après sur le changement d'écran avec les boutons
    int delay_val = 200;          //l'attente pour les boutons


  	system("./nano-X &");


  	//configuration pour les préférences d'afichage
  	if (GrOpen() < 0)                            //fonction graphics routines
  	{
          	printf("Can't open graphics\n");
          	exit(1);
      	}

        // parametrès dans l'ordre parent, coordonées x et y (20,100) ,
        //largeur 120, hauteur 120 , bordersize 5 ,backgroud WHITE , bordercolor BLACK

      	window = GrNewWindow(GR_ROOT_WINDOW_ID, 20, 100, 120, 120,5, WHITE, BLACK);


        //function allocates a new graphic context with all parameters set to the default values.
        gc = GrNewGC();                 //fonction ligne 534 dans nano-X.h c'est GR_GC_ID

        // function sets the foreground color of the specified graphics context.
      	GrSetGCForeground(gc, BLACK);    //ligne 608

        //function sets the drawing mode for bitmaps and text.
        GrSetGCUseBackground(gc, GR_FALSE); //

        /** function tells the server which events the client want to receive for the  window and
        replaces any previously selected event mask for the window.**/
        GrSelectEvents(window, GR_EVENT_MASK_EXPOSURE);

        //maps a window and any unmapped children of that window
        GrMapWindow(window);

        //=====================essai affichage text
        GrText(window, gc, 0, 20,"=== Un premier affichage de la station meteo :)==" , -1, GR_TFASCII);


       //function flushes any buffered function calls and closes the connection created with the GrOpen function.
        GrClose();
      return (0);
}
    	//============utilisation des boutons ça serait après
      //*********************************************************
/**     	while(1)
      	{
  	          KbStatus = KEYBOARD_STATUS();
      		  if((KbStatus & 0x1)==1)
      		 {
                     printf("Button Ox1, S2 pressed\n") ;
      	 		while( KEYBOARD_STATUS() == KbStatus) ;
      	    		msleep(delay_val) ;
         		}

      		if((KbStatus & 0x2)==2) //bouton 3
      		{
  			//affichage et positionnement des valeurs(température, humidité et pression)sur l'ecran LCD
  	  		for (;;) {
          		GrGetNextEvent(&event);
          		switch (event.type) {
         	 		case GR_EVENT_TYPE_EXPOSURE:
          		GrText(w, gc, 0, 20,valeur1 , -1, GR_TFASCII);
  	  		GrText(w, gc, 0, 60,valeur2 , -1, GR_TFASCII);
  	  		GrText(w, gc, 0, 100,valeur3 , -1, GR_TFASCII);
  			while( KEYBOARD_STATUS() == KbStatus) ;
      		}

      		if((KbStatus & 0x4)==4)
      		{
      			printf("Button 0x4, S4 pressed\n") ;
      			while( KEYBOARD_STATUS() == KbStatus) ;
      		}

      		if((KbStatus & 0x8)==8)
      		{
      			printf("Button 0x8, S5 pressed\n") ;
      			while( KEYBOARD_STATUS() == KbStatus) ;
      		}
      	}
          break;
      }
    }*/
