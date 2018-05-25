
#include "capteurs.h"
#include "menus.h"

int ord_pix_1, ord_pix_2, ord_min, ord_max, ord_i, debut_nb_archives=0;

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
    GrText(w, gc, 10, 170," - Humidite rel :", 17, GR_TFASCII);
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
    GrText(w, gc, 10, 170," - Humidite abs :", 17, GR_TFASCII);
    GrText(w, gc, 10, 190,sRH, 14, GR_TFASCII);
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
  GrText(w, gc, 10, 170," - Humidite rel :", 17, GR_TFASCII);
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
  GrText(w, gc, 10, 170," - Humidite abs :", 17, GR_TFASCII);
  GrText(w, gc, 10, 190,sRH, 14, GR_TFASCII);
  GrLine(w, gc, 10, 220, 130, 220);

  return EXIT_SUCCESS;

} // affichage_menu_02

int affichage_menu_03(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_tendances p)
{
  char sT[64], sP[64], sRH[64];

  sprintf(sT, "%d celsius", p->T);
  sprintf(sP, "%d bar", p->P);
  sprintf(sRH, "%d %%", p->RH);

  GrText(w, gc, 25, 20,"---Tendances---", 16, GR_TFASCII);
  GrLine(w, gc, 10, 40, 130, 40);
  GrText(w, gc, 10, 70," - Temperature :", 17, GR_TFASCII);
  GrText(w, gc, 10, 120," - Pression :", 14, GR_TFASCII);
  GrText(w, gc, 10, 170," - Humidite :", 14, GR_TFASCII);
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

/*********************================= Nouveau***************/
//=================== à tester pour les courbes **************/
/********* affiche la courbes des valeurs pour la temperature***/
int affichage_menu_courbes_temperature_celsius(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data g_archives_donnees, int g_nb_archives)
{
  char index_min[10], index_max[10];
  int i, abscisse=10;
  // ==== titre de l'écran
    GrText(w, gc, 35, 12,"---Courbe tmp---", 16, GR_TFASCII);
  //valeur sur l'axe Ox

  //=====axe Ox
  GrLine(w, gc, 10, 220, 140, 220);
  GrDrawImageFromFile(w, gc, 140, 215, 10, 10, IMG_RIGHT, 0);
  GrText(w, gc, 100, 235,"temps", 5, GR_TFASCII);

  //===== axe Oy
  GrLine(w, gc, 10, 220, 10, 30);
  GrDrawImageFromFile(w, gc, 5, 30, 10, 10, IMG_UP, 0);

//========================Pour la temperature en degreés Celsius
  printf("Nb de valeurs dans Archive : %d \n", g_nb_archives);

  ord_min=(int)g_archives_donnees[debut_nb_archives].Tc-1;
  for (i = debut_nb_archives; i < g_nb_archives ; ++i){
    ord_i=(int)g_archives_donnees[i].Tc-1;
    if (ord_i>ord_min) {
      ord_min=(int)g_archives_donnees[i].Tc-1;
    }
  }

  ord_max=(int)g_archives_donnees[debut_nb_archives].Tc+1;
  for (i = debut_nb_archives; i < g_nb_archives ; ++i){
    ord_i=(int)g_archives_donnees[i].Tc+1;
    if (ord_i>ord_max) {
      ord_max=(int)g_archives_donnees[i].Tc+1;
    }
  }

  sprintf(index_min, "%dC", ord_min);
  sprintf(index_max, "%dC", ord_max);
  // =================mettre les valeurs sur les axes
  //la valeur minimum
  GrText(w, gc, 5, 235, index_min, 3, GR_TFASCII);
  //la valeur max en haut
  GrText(w, gc, 5, 25, index_max, 3, GR_TFASCII);

//================chercher le minimum de Temperature en Celsius
  ord_min=g_archives_donnees[debut_nb_archives].Tc-1;
  for (i = debut_nb_archives +1 ; i < g_nb_archives ; ++i){
    if ((g_archives_donnees[i].Tc-1)>ord_min )
      ord_min=g_archives_donnees[i].Tc-1;
  }
//================chercher le maximum de Temperature en Celsius
  ord_max=g_archives_donnees[debut_nb_archives].Tc+1;
  for (i = debut_nb_archives + 1; i < g_nb_archives ; ++i){
    if ((g_archives_donnees[i].Tc+1)>ord_max )
      ord_max=g_archives_donnees[i].Tc+1;
  }

  for (i = debut_nb_archives; i < g_nb_archives-1 ; ++i)
  {
  //la valeur de tempterature en degree Celsius
      ord_pix_1=(int)(220-(g_archives_donnees[i].Tc-ord_min)*(190/(ord_max-ord_min)));
      ord_pix_2=(int)(220-(g_archives_donnees[i+1].Tc-ord_min)*(190/(ord_max-ord_min)));
      //on trace la courbe
      GrLine(w, gc, abscisse, ord_pix_1, (abscisse+20), ord_pix_2);
      //on chage d'abscisse
      abscisse = abscisse + 20;
  }
  //si ça depasse
  if (abscisse>=140)
    debut_nb_archives=g_nb_archives;
  return EXIT_SUCCESS;
}


int affichage_menu_courbes_pression_hPa(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data g_archives_donnees, int g_nb_archives)
{
  char index_min[10], index_max[10];
  int i, abscisse=10;
  // ==== titre de l'écran
    GrText(w, gc, 30, 12,"---Courbe press---", 18, GR_TFASCII);
  //valeur sur l'axe Ox

  //=====axe Ox
  GrLine(w, gc, 10, 220, 140, 220);
  GrDrawImageFromFile(w, gc, 140, 215, 10, 10, IMG_RIGHT, 0);
  GrText(w, gc, 100, 235,"temps", 5, GR_TFASCII);

  //===== axe Oy
  GrLine(w, gc, 10, 220, 10, 30);
  GrDrawImageFromFile(w, gc, 5, 30, 10, 10, IMG_UP, 0);

//========================Pour la pression en hPa
  printf("Nb de valeurs dans Archive : %d \n", g_nb_archives);

  ord_min=(int)g_archives_donnees[debut_nb_archives].Ph-2;
  for (i = debut_nb_archives; i < g_nb_archives ; ++i){
    ord_i=(int)g_archives_donnees[i].Ph-2;
    if (ord_i>ord_min) {
      ord_min=(int)g_archives_donnees[i].Ph-2;
    }
  }

  ord_max=(int)g_archives_donnees[debut_nb_archives].Ph+2;
  for (i = debut_nb_archives; i < g_nb_archives ; ++i){
    ord_i=(int)g_archives_donnees[i].Ph+2;
    if (ord_i>ord_max) {
      ord_max=(int)g_archives_donnees[i].Ph+2;
    }
  }

  sprintf(index_min, "%dhPa", ord_min);
  sprintf(index_max, "%dhPa", ord_max);
  // =================mettre les valeurs sur les axes
  //la valeur minimum
  GrText(w, gc, 5, 235, index_min, 7, GR_TFASCII);
  //la valeur max en haut
  GrText(w, gc, 5, 25, index_max, 6, GR_TFASCII);

//================chercher le minimum de pression en hPa
  ord_min=g_archives_donnees[debut_nb_archives].Ph-2;
  for (i = debut_nb_archives +1 ; i < g_nb_archives ; ++i){
    if ((g_archives_donnees[i].Ph-2)>ord_min )
      ord_min=g_archives_donnees[i].Ph-2;
  }
//================chercher le maximum de pression en hPa
  ord_max=g_archives_donnees[debut_nb_archives].Ph+2;
  for (i = debut_nb_archives + 1; i < g_nb_archives ; ++i){
    if ((g_archives_donnees[i].Ph+2)>ord_max )
      ord_max=g_archives_donnees[i].Ph+2;
  }

  for (i = debut_nb_archives; i < g_nb_archives-1 ; ++i)
  {
  //la valeur de pression en hPa
      ord_pix_1=(int)(220-(g_archives_donnees[i].Ph-ord_min)*(190/(ord_max-ord_min)));
      ord_pix_2=(int)(220-(g_archives_donnees[i+1].Ph-ord_min)*(190/(ord_max-ord_min)));

      //on trace la courbe
      GrLine(w, gc, abscisse, ord_pix_1, (abscisse+20), ord_pix_2);
      //on chage d'abscisse
      abscisse = abscisse + 20;
  }
  //si ça depasse
  if (abscisse>=140)
    debut_nb_archives=g_nb_archives;
  return EXIT_SUCCESS;
}

int affichage_menu_courbes_humidite_gm3(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data g_archives_donnees, int g_nb_archives)
{
  char index_min[10], index_max[10];
  int i, abscisse=10;
  // ==== titre de l'écran
    GrText(w, gc, 35, 12,"---Courbe hum---", 16, GR_TFASCII);
  //valeur sur l'axe Ox

  //=====axe Ox
  GrLine(w, gc, 10, 220, 140, 220);
  GrDrawImageFromFile(w, gc, 140, 215, 10, 10, IMG_RIGHT, 0);
  GrText(w, gc, 100, 235,"temps", 5, GR_TFASCII);

  //===== axe Oy
  GrLine(w, gc, 10, 220, 10, 30);
  GrDrawImageFromFile(w, gc, 5, 30, 10, 10, IMG_UP, 0);

//========================Pour la pression en hPa
  printf("Nb de valeurs dans Archive : %d \n", g_nb_archives);

  ord_min=(int)g_archives_donnees[debut_nb_archives].Ha-1;
  for (i = debut_nb_archives; i < g_nb_archives ; ++i){
    ord_i=(int)g_archives_donnees[i].Ha-1;
    if (ord_i>ord_min) {
      ord_min=(int)g_archives_donnees[i].Ha-1;
    }
  }

  ord_max=(int)g_archives_donnees[debut_nb_archives].Ha+1;
  for (i = debut_nb_archives; i < g_nb_archives ; ++i){
    ord_i=(int)g_archives_donnees[i].Ha+1;
    if (ord_i>ord_max) {
      ord_max=(int)g_archives_donnees[i].Ha+1;
    }
  }

  sprintf(index_min, "%dg/m3", ord_min);
  sprintf(index_max, "%dg/m3", ord_max);
  // =================mettre les valeurs sur les axes
  //la valeur minimum
  GrText(w, gc, 5, 235, index_min, 6, GR_TFASCII);
  //la valeur max en haut
  GrText(w, gc, 5, 25, index_max, 6, GR_TFASCII);

//================chercher le minimum de pression en hPa
  ord_min=g_archives_donnees[debut_nb_archives].Ha-1;
  for (i = debut_nb_archives +1 ; i < g_nb_archives ; ++i){
    if ((g_archives_donnees[i].Ha-1)>ord_min )
      ord_min=g_archives_donnees[i].Ha-1;
  }
//================chercher le maximum de pression en hPa
  ord_max=g_archives_donnees[debut_nb_archives].Ha+1;
  for (i = debut_nb_archives + 1; i < g_nb_archives ; ++i){
    if ((g_archives_donnees[i].Ha+1)>ord_max )
      ord_max=g_archives_donnees[i].Ha+1;
  }

  for (i = debut_nb_archives; i < g_nb_archives-1 ; ++i)
  {
  //la valeur de pression en hPa
      ord_pix_1=(int)(220-(g_archives_donnees[i].Ha-ord_min)*(190/(ord_max-ord_min)));
      ord_pix_2=(int)(220-(g_archives_donnees[i+1].Ha-ord_min)*(190/(ord_max-ord_min)));
      //on trace la courbe
      GrLine(w, gc, abscisse, ord_pix_1, (abscisse+20), ord_pix_2);
      //on chage d'abscisse
      abscisse = abscisse + 20;
  }
  //si ça depasse
  if (abscisse>=140)
    debut_nb_archives=g_nb_archives;
  return EXIT_SUCCESS;
}
