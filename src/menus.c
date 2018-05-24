
#include "capteurs.h"
#include "menus.h"
int ord_pix_1, ord_pix_2, ord_min, ord_max, debut_nb_archives=0;

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

  sprintf(sT, "%lf celsius", p->T);
  sprintf(sP, "%lf bar", p->P);
  sprintf(sRH, "%lf %%", p->RH);

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
  printf("debut_nb_archives= %d \n",debut_nb_archives);
  char sT[64], sP[64], sRH[64];
  //valeur sur l'axe Ox
  int i, abscisse=10;
  GrDrawImageFromFile(w, gc, 140, 215, 10, 10, IMG_RIGHT, 0);
  GrLine(w, gc, 10, 220, 140, 220);
  GrDrawImageFromFile(w, gc, 5, 30, 10, 10, IMG_UP, 0);
  GrLine(w, gc, 10, 220, 10, 30);
  GrText(w, gc, 100, 235,"temps", 5, GR_TFASCII);
  GrText(w, gc, 5, 235,"24C", 3, GR_TFASCII);
  GrText(w, gc, 5, 25,"27C", 3, GR_TFASCII);

  GrText(w, gc, 35, 20,"---Courbe tmp---", 16, GR_TFASCII);

//========================Pour la temperature en degreés Celsius
  printf("Nb de valeurs dans Archive : %d \n", g_nb_archives);

  ord_min=g_archives_donnees[debut_nb_archives]-1;
  for (i = debut_nb_archives; i < g_nb_archives ; ++i){
    if (g_archives_donnees[i]-1)>ord_min {
      ord_min=g_archives_donnees[i]-1;
    }
  }

  ord_max=g_archives_donnees[debut_nb_archives]+1;
  for (i = debut_nb_archives; i < g_nb_archives ; ++i){
    if (g_archives_donnees[i]+1)>ord_max {
      ord_max=g_archives_donnees[i]+1;
    }
  }
  char index_min[3];
  sprintf(index_min, "%dC", ord_min);
  char index_max[3];
  sprintf(index_max, "%dC", ord_max);
  GrText(w, gc, 5, 235, index_min, 3, GR_TFASCII);
  GrText(w, gc, 5, 25, index_max, 3, GR_TFASCII);

  for (i = debut_nb_archives; i < g_nb_archives-1 ; ++i)
  {
  //le point faire un simbole joli :)

//  GrDrawImageFromFile(w, gc, abscisse,  g_archives_donnees[i].Tc, 5, 5, IMG_DOWN, 0);

  //la valeur de tempterature en degree Celsius
      ord_pix_1=(int)((g_archives_donnees[i].Tc-ord_min)*(190/(ord_max-ord_min))+30);
      ord_pix_2=(int)((g_archives_donnees[i+1].Tc-ord_min)*(190/(ord_max-ord_min))+30);

    //  printf("Valeur de Archive temp : %lf \n",g_archives_donnees[i].Tc);
    //  printf("Valeur de Archive temp +1 : %lf \n",g_archives_donnees[i+1].Tc);
    //  printf("Valeur de ord temp : %d \n", ord_pix_1);
    //  printf("Valeur de ord temp +1 : %d \n", ord_pix_2);



      GrLine(w, gc, abscisse, ord_pix_1, (abscisse+20), ord_pix_2);
      abscisse = abscisse + 20;

  }
  printf("abcisse= %d",abscisse);
  if (abscisse>=220){
    debut_nb_archives=g_nb_archives;
  }
//le dernier point
//GrDrawImageFromFile(w, gc, abscisse,  g_archives_donnees[i].Tc, 5, 5, IMG_DOWN, 0);

  return EXIT_SUCCESS;
}


int affichage_menu_courbes_pression_hPa(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data g_archives_donnees, int g_nb_archives)
{
  char sT[64], sP[64], sRH[64];
  //valeur sur l'axe Ox
  int abscisse = 0,i;

  GrText(w, gc, 25, 20,"---Courbes---", 16, GR_TFASCII);
//========================Pour la pression en hPa
for (i = 0; i < g_nb_archives -1 ; ++i)
{
  //le point faire un simbole joli :)

  GrDrawImageFromFile(w, gc, abscisse,  g_archives_donnees[i].Ph, 5, 5, IMG_DOWN, 0);

  //la valeur de tempterature en degree Celsius
  GrLine(w, gc, abscisse, g_archives_donnees[i].Ph, abscisse+10, g_archives_donnees[i+1].Tc);
  abscisse = abscisse + 10;
}
//le dernier point
GrDrawImageFromFile(w, gc, abscisse,  g_archives_donnees[i].Ph, 5, 5, IMG_DOWN, 0);

  return EXIT_SUCCESS;
}

int affichage_menu_courbes_humidite_absolue(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data g_archives_donnees, int g_nb_archives)
{
  char sT[64], sP[64], sRH[64];
  //valeur sur l'axe Ox
  int abscisse = 0,i;

  GrText(w, gc, 25, 20,"---Courbes---", 16, GR_TFASCII);
//========================Pour l'humidité absolue
for (i = 0; i < g_nb_archives -1 ; ++i)
{
  //le point faire un simbole joli :)

  GrDrawImageFromFile(w, gc, abscisse,  g_archives_donnees[i].Ha, 5, 5, IMG_DOWN, 0);

  //la valeur de  l'humidité absolue
  GrLine(w, gc, abscisse, g_archives_donnees[i].Ha, abscisse+10, g_archives_donnees[i+1].Tc);
  abscisse = abscisse + 10;
}
//le dernier point
GrDrawImageFromFile(w, gc, abscisse,  g_archives_donnees[i].Ha, 5, 5, IMG_DOWN, 0);

  return EXIT_SUCCESS;
}
