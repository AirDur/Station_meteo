
#include "capteurs.h"
#include "menus.h"


int affichage_menu_01(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data p)
{
    char sT[64], sP[64], sRH[64];

    sprintf(sT,  "T = %.2f C\n",   p->T);
    sprintf(sP,  "P = %.2f hPa\n", p->P);
    sprintf(sRH, "H = %.2f %%\n",  p->RH);

    GrText(w, gc, 20, 30, "TEST Temps reel",  -1, GR_TFASCII);
    GrText(w, gc, 35, 80,  sT,  -1, GR_TFASCII);
    GrText(w, gc, 35, 110, sP,  -1, GR_TFASCII);
    GrText(w, gc, 35, 140, sRH, -1, GR_TFASCII);

    return EXIT_SUCCESS;

} // affichage_menu_01

int affichage_menu_02(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data p)
{
    char sT[64], sP[64], sRH[64];

    sprintf(sT,  "T = %.2f C\n",   p->T);
    sprintf(sP,  "P = %.2f hPa\n", p->P);
    sprintf(sRH, "H = %.2f %%\n",  p->RH);

    GrText(w, gc, 20, 30, "TEST Moyennes",  -1, GR_TFASCII);
    GrText(w, gc, 35, 80,  sT,  -1, GR_TFASCII);
    GrText(w, gc, 35, 110, sP,  -1, GR_TFASCII);
    GrText(w, gc, 35, 140, sRH, -1, GR_TFASCII);

    return EXIT_SUCCESS;

} // affichage_menu_02

int affichage_menu_03(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_tendances p)
{
    GrText(w, gc, 20, 30, "TEST Tendances", -1, GR_TFASCII);

    GrText(w, gc, 35, 80,  "T =", -1, GR_TFASCII);
    GrText(w, gc, 35, 110, "P =", -1, GR_TFASCII);
    GrText(w, gc, 35, 140, "H =", -1, GR_TFASCII);

    if (p->T > 0)
        GrDrawImageFromFile(w, gc, 60, 60, 30, 30, IMG_UP, 0);
    else
        GrDrawImageFromFile(w, gc, 60, 60, 30, 30, IMG_DOWN, 0);
    if (p->P > 0)
        GrDrawImageFromFile(w, gc, 60, 90, 30, 30, IMG_UP, 0);
    else
        GrDrawImageFromFile(w, gc, 60, 90, 30, 30, IMG_DOWN, 0);
    if (p->RH > 0)
        GrDrawImageFromFile(w, gc, 60, 120, 30, 30, IMG_UP, 0);
    else
        GrDrawImageFromFile(w, gc, 60, 120, 30, 30, IMG_DOWN, 0);

    return EXIT_SUCCESS;

} // affichage_menu_03

int affichage_menu_04(GR_WINDOW_ID w, GR_GC_ID gc)
{
    GrText(w, gc, 40, 75, "Bye bye !",  -1, GR_TFASCII);

    return EXIT_SUCCESS;

} // affichage_menu_04
