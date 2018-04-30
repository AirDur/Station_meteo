#include "affichage.h"

void affichage(double temperature, double pression, double humidite) {
/*  GR_WINDOW_ID w;
  GR_GC_ID gc;
  GR_EVENT event;
  char sT[64], sP[64], sRH[64];

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


  sprintf(sT, "%.1f °C\n", temperature);
  sprintf(sP, "%.1f bar\n", pression);
  sprintf(sRH, "%.1f %%\n", humidite);

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
}
*/
GR_WINDOW_ID w;
GR_EVENT event;
GR_GC_ID gc;

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

    gc = GrNewGC();
    GrText(w, gc, 0, 20,"Ceci est un test", -1, GR_TFASCII);


      //function flushes any buffered function calls and closes the connection created with the GrOpen function.
     GrClose();


     return (0);

   }
