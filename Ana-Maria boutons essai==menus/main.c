#include "capteurs.h"
#include "affichage.h"
#include "boutons.h"

int main(int argc, char**argv)
{
  int buffer, ret, i=0;

  double temperature = 0.0, humidite = 0.0, pression = 0.0;

  int execute = 1;

  do {
    unsigned int pressed = Push_Button();
    if(pressed == 1) {
      printf("===Premier button appuyé===\n") ;
      if ( affichage_main_bouton_1() != EXIT_SUCCESS ) {
			  perror("[main.c] affichage bouton 1");
			  return EXIT_FAILURE;
		  }
		  sleep(2);
      //while(Push_Button() != 4){}
      //break;
    }
    else if(pressed == 2) {
      printf("===deuxième button appuyé===\n") ;
      if ( affichage_main_bouton_1() != EXIT_SUCCESS ) {
			  perror("[main.c] affichage bouton 2");
			  return EXIT_FAILURE;
		  }
		  sleep(2);
      //while(Push_Button() != 4){}
      break;
    }
    else if(pressed == 3) {
      printf("===Troisième button appuyé===\n") ;
      if ( affichage_main_bouton_1() != EXIT_SUCCESS ) {
			  perror("[main.c] affichage bouton 3");
			  return EXIT_FAILURE;
		  }
		  sleep(2);
      //while(Push_Button() != 4){}
      break;
    } else if(pressed == 4) {
      printf("===Quatrieme button appuyé===\n") ;
      if ( affichage_main_bouton_1() != EXIT_SUCCESS ) {
			  perror("[main.c] affichage bouton 4");
			  return EXIT_FAILURE;
		  }
		  sleep(2);
      //while(Push_Button() != 4){}
      execute = 0;
      break;
    }
  } while ( execute == 1 );

  return 0;
}
