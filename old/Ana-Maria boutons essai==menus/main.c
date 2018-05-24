
#include "capteurs.h"
#include "affichage.h"
#include "boutons.h"

int main(int argc, char * * argv) {
  int buffer, ret, i = 0;
  unsigned int KbStatus;
  double temperature = 0.0, humidite = 0.0, pression = 0.0;

  while (1) {
    //premier bouton
    KbStatus = KEYBOARD_STATUS();
    if ((KbStatus & BUTTON_01) == 1) {
      //action  fenetre bouton 1
      printf("===Premier button appuyé===\n"); //dans le terminal
      if (affichage_main_bouton_1() != EXIT_SUCCESS) {
        perror("[main.c] affichage bouton 1");
        return EXIT_FAILURE;
      }
      sleep(5000); //en milisecondes
      printf("==Voila le sleep est terminée==");
      while (KEYBOARD_STATUS() == KbStatus);
    }

    if ((KbStatus & BUTTON_02) == 2) {
      //action  fenetre bouton 2
      printf("===Deuxième button appuyé===\n"); //dans le terminal
      if (affichage_main_bouton_2() != EXIT_SUCCESS) {
        perror("[main.c] affichage bouton 2");
        return EXIT_FAILURE;
      }
      sleep(5000); //en milisecondes
      printf("==Voila le sleep est terminée==");
      while (KEYBOARD_STATUS() == KbStatus);
    }

    if ((KbStatus & BUTTON_03) == 4) {
      //action  fenetre bouton 3
      printf("===Troisième bouton===\n"); //dans le terminal
      if (affichage_main_bouton_3() != EXIT_SUCCESS) {
        perror("[main.c] affichage bouton 3");
        return EXIT_FAILURE;
      }
      sleep(5000); //en milisecondes
      printf("==Voila le sleep est terminée==");

      while (KEYBOARD_STATUS() == KbStatus);
    }

    if ((KbStatus & BUTTON_04) == 8) {
      //action  fenetre bouton 4
      printf("===Quatrième bouton===\n"); //dans le terminal
      if (affichage_main_bouton_4() != EXIT_SUCCESS) {
        perror("[main.c] affichage bouton 4");
        return EXIT_FAILURE;
      }
      sleep(5000); //en milisecondes
      printf("==Voila le sleep est terminée==");
      while (KEYBOARD_STATUS() == KbStatus);
    }
  }
  return 0;
}
