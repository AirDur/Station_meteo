
#include "capteurs.h"
#include "menus.h"


int affichage_menu_01_a(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data p)
{
    char sT[64], sP[64], sRH[64];

    sprintf(sT, "%lf celsius", p->Tc);
    sprintf(sP, "%lf hPa", p->Ph);
    sprintf(sRH, "%lf %%", p->Hr);

    GrText(w, gc, 25, 20,"---Temps reel---", 16, GR_TFASCII);
    GrLine(w, gc, 10, 40, 130, 40);
    GrText(w, gc, 10, 70," - Temperature :", 17, GR_TFASCII);
    GrText(w, gc, 10, 90,sT, 18, GR_TFASCII);
    GrText(w, gc, 10, 120," - Pression :", 14, GR_TFASCII);
    GrText(w, gc, 10, 140,sP, 15, GR_TFASCII);
    GrText(w, gc, 10, 170," - Humidite rel :", 14, GR_TFASCII);
    GrText(w, gc, 10, 190,sRH, 12, GR_TFASCII);
    GrLine(w, gc, 10, 220, 130, 220);

    return EXIT_SUCCESS;

} // affichage_menu_01


int affichage_menu_01_b(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data p)
{
    char sT[64], sP[64], sRH[64];

    sprintf(sT, "%lf Farenheit", p->Tf);
    sprintf(sP, "%lf mmHg", p->Pm);
    sprintf(sRH, "%lf g/m3", p->Ha);

    GrText(w, gc, 25, 20,"---Temps reel---", 16, GR_TFASCII);
    GrLine(w, gc, 10, 40, 130, 40);
    GrText(w, gc, 10, 70," - Temperature :", 17, GR_TFASCII);
    GrText(w, gc, 10, 90,sT, 18, GR_TFASCII);
    GrText(w, gc, 10, 120," - Pression :", 14, GR_TFASCII);
    GrText(w, gc, 10, 140,sP, 15, GR_TFASCII);
    GrText(w, gc, 10, 170," - Humidite abs :", 14, GR_TFASCII);
    GrText(w, gc, 10, 190,sRH, 12, GR_TFASCII);
    GrLine(w, gc, 10, 220, 130, 220);

    return EXIT_SUCCESS;

} // affichage_menu_01

int affichage_menu_02_a(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data p)
{
  char sT[64], sP[64], sRH[64];

  sprintf(sT, "%lf celsius", p->Tc);
  sprintf(sP, "%lf hPa", p->Ph);
  sprintf(sRH, "%lf %%", p->Hr);

  GrText(w, gc, 25, 20,"---Moyennes---", 16, GR_TFASCII);
  GrLine(w, gc, 10, 40, 130, 40);
  GrText(w, gc, 10, 70," - Temperature :", 17, GR_TFASCII);
  GrText(w, gc, 10, 90,sT, 18, GR_TFASCII);
  GrText(w, gc, 10, 120," - Pression :", 14, GR_TFASCII);
  GrText(w, gc, 10, 140,sP, 15, GR_TFASCII);
  GrText(w, gc, 10, 170," - Humidite rel :", 14, GR_TFASCII);
  GrText(w, gc, 10, 190,sRH, 12, GR_TFASCII);
  GrLine(w, gc, 10, 220, 130, 220);

  return EXIT_SUCCESS;

} // affichage_menu_02

int affichage_menu_02_b(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data p)
{
  char sT[64], sP[64], sRH[64];

  sprintf(sT, "%lf Farenheit", p->Tf);
  sprintf(sP, "%lf mmHg", p->Pm);
  sprintf(sRH, "%lf g/m3", p->Ha);

  GrText(w, gc, 25, 20,"---Moyennes---", 16, GR_TFASCII);
  GrLine(w, gc, 10, 40, 130, 40);
  GrText(w, gc, 10, 70," - Temperature :", 17, GR_TFASCII);
  GrText(w, gc, 10, 90,sT, 18, GR_TFASCII);
  GrText(w, gc, 10, 120," - Pression :", 14, GR_TFASCII);
  GrText(w, gc, 10, 140,sP, 15, GR_TFASCII);
  GrText(w, gc, 10, 170," - Humidite abs :", 14, GR_TFASCII);
  GrText(w, gc, 10, 190,sRH, 12, GR_TFASCII);
  GrLine(w, gc, 10, 220, 130, 220);

  return EXIT_SUCCESS;

} // affichage_menu_02

int affichage_menu_03(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_tendances p)
{
  char sT[64], sP[64], sRH[64];

  sprintf(sT, "%lf celsius", p->T);
  sprintf(sP, "%lf bar", p->P);
  sprintf(sRH, "%lf %%", p->RH);

  GrText(w, gc, 25, 20,"---Tendances---", 16, GR_TFASCII);
  GrLine(w, gc, 10, 40, 130, 40);
  GrText(w, gc, 10, 70," - Temperature :", 17, GR_TFASCII);
  GrText(w, gc, 10, 120," - Pression :", 14, GR_TFASCII);
  GrText(w, gc, 10, 170," - Humidite rel :", 14, GR_TFASCII);
  GrLine(w, gc, 10, 200, 130, 200);

  if (p->T > 0)
      GrDrawImageFromFile(w, gc, 110, 45, 30, 30, IMG_UP, 0);
  else
      GrDrawImageFromFile(w, gc, 110, 45, 30, 30, IMG_DOWN, 0);
  if (p->P > 0)
      GrDrawImageFromFile(w, gc, 110, 95, 30, 30, IMG_UP, 0);
  else
      GrDrawImageFromFile(w, gc, 110, 95, 30, 30, IMG_DOWN, 0);
  if (p->RH > 0)
      GrDrawImageFromFile(w, gc, 110, 145, 30, 30, IMG_UP, 0);
  else
      GrDrawImageFromFile(w, gc, 110, 145, 30, 30, IMG_DOWN, 0);

  return EXIT_SUCCESS;

} // affichage_menu_03

int affichage_menu_04(GR_WINDOW_ID w, GR_GC_ID gc)
{
    GrText(w, gc, 40, 75, "Au revoir !",  -1, GR_TFASCII);

    return EXIT_SUCCESS;

} // affichage_menu_04
