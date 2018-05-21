#include "capteurs.h"
#include "affichage.h"
#include "boutons.h"

int main(int argc, char**argv)
{
  int buffer, ret, i=0;

  double temperature = 0.0, humidite = 0.0, pression = 0.0;

  int ok = 1;
  do{
       ok = 0;
       while(ok == 0)
        {
            if(Push_Button() == 1)
            {
              printf("===Premier button appuyé===\n") ;
               if ( affichage_main_bouton_1() != EXIT_SUCCESS )
				  {
					  perror("[main.c] affichage bouton 1");
					  return EXIT_FAILURE;
				  }
				  
              sleep(2);
              while(Push_Button() != 4){}
              ok = 1;
              break;
            }
            if(Push_Button() == 2)
            {
              printf("===Deuxième bouton===\n") ;
              if ( affichage_main_bouton_2() != EXIT_SUCCESS )
				  {
					  perror("[main.c] affichage bouton 2");
					  return EXIT_FAILURE;
				  }
              sleep(2);
              while(Push_Button() != 4){}
              ok = 1;
              break;
            }
            if(Push_Button() == 3)
            {
              printf("===Troisième bouton===\n") ;
              if ( affichage_main_bouton_3() != EXIT_SUCCESS )
				  {
					  perror("[main.c] affichage bouton 3");
					  return EXIT_FAILURE;
				  }
              sleep(4);
              ok = 0;
              break;
            }
            if(Push_Button() == 4)
            {
              printf("===Quatrième bouton===\n") ;
              if ( affichage_main_bouton_4() != EXIT_SUCCESS )
				  {
					  perror("[main.c] affichage bouton 4");
					  return EXIT_FAILURE;
				  }
              sleep(4);
              ok = 0;
              break;
            }
        }
}while(ok == 1);

  return 0;
}
