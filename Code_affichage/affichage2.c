/**
 * @file     	affichage.c
 * @author
 * @brief    	Fontion de base permettant d'afficher sur l'écran un message
 */

#include "affichage.h"

int main(int acgc,char **argv)
{
  	GR_WINDOW_ID w;
    GR_EVENT event;

  	system("./nano-X &");


  	//configuration pour les préférences d'afichage
  	if (GrOpen() < 0)                            //fonction graphics routines
  	{
          	printf("Can't open graphics\n");
          	exit(1);
      	}

        // parametrès dans l'ordre parent, coordonées x et y (20,100) ,
        //largeur 120, hauteur 120 , bordersize 5 ,backgroud WHITE , bordercolor BLACK

      	w = GrNewWindow(GR_ROOT_WINDOW_ID, 20, 100, 120, 120,5, WHITE, RED);
        GrMapWindow(w);


        /*Enter event loop **/
        for(;;)
          GrGetNextEvent(&event);


          //function flushes any buffered function calls and closes the connection created with the GrOpen function.
         GrClose();
         return (0);
}
