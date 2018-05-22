#include "affichage.h"

int affichage_current_data(GR_WINDOW_ID w, GR_GC_ID gc, double * temperature, double * humidite, double * pression){

  char sT[64], sP[64], sRH[64];
  /*
  GrFillRect(w,gc,0,0,160,4);
  GrFillRect(w,gc,0,4,4,240);
  GrFillRect(w,gc,0,236,160,4);
  GrFillRect(w,gc,156,4,4,240);
  GrFillRect(w,gc,0,40,160,2);
  GrFillRect(w,gc,80,40,2,200);
  GrFillRect(w,gc,80,140,80,2);
  GrText(w, gc, 50, 30, "Donnees", -1, GR_TFASCII);



  get_Temp(sT,p);
  get_Pressure(sP,p);
  sprintf(sRH, "%.1f %%\n",  p->RH);

  if(current_orientation == 0){ //original
    GrText(w, gc, 15, 145,  sT,  -1, GR_TFASCII);
    GrText(w, gc, 90, 100, sP,  -1, GR_TFASCII);
    GrText(w, gc, 90, 190, sRH, -1, GR_TFASCII);
  }else if(current_orientation == 1) {
    //tourne d'un tour.
    GrText(w, gc, 15, 145,  sRH,  -1, GR_TFASCII);
    GrText(w, gc, 90, 100, sT,  -1, GR_TFASCII);
    GrText(w, gc, 90, 190, sP, -1, GR_TFASCII);
  }else{
    //encore un
    GrText(w, gc, 15, 145,  sP,  -1, GR_TFASCII);
    GrText(w, gc, 90, 100, sRH,  -1, GR_TFASCII);
    GrText(w, gc, 90, 190, sT, -1, GR_TFASCII);
  }
  */
  sprintf(sT, "%lf celsius", *temperature);
  sprintf(sP, "%lf bar", *pression);
  sprintf(sRH, "%lf %%", *humidite);

  GrText(w, gc, 25, 20,"OM a perdu", 16, GR_TFASCII);
  GrLine(w, gc, 10, 40, 130, 40);
  GrText(w, gc, 10, 70," - Temperature :", 17, GR_TFASCII);
  GrText(w, gc, 10, 90,sT, 18, GR_TFASCII);
  GrText(w, gc, 10, 120," - Pression :", 14, GR_TFASCII);
  GrText(w, gc, 10, 140,sP, 15, GR_TFASCII);
  GrText(w, gc, 10, 170," - Humidite :", 14, GR_TFASCII);
  GrText(w, gc, 10, 190,sRH, 12, GR_TFASCII);

  return EXIT_SUCCESS;

}
