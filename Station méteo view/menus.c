
#include "capteurs.h"
#include "menus.h"


//extern volatile int current_screen = 0;
extern volatile int current_orientation = 0;
extern volatile int current_data = 0;
extern volatile int current_format_temp = 0;
extern volatile int current_format_pressure = 0;


int affichage_current_data(GR_WINDOW_ID w, GR_GC_ID gc, t_ptr_captors_data p){

  char sT[64], sP[64], sRH[64];

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

  return EXIT_SUCCESS;

}


/**
 * Affichage de l'historique de la semaine
 */
int affichage_menu_02(GR_WINDOW_ID w, GR_GC_ID gc, t_captors_data data [],int nbArchives){

  char title[64], minC[64], maxC[64];
  int i,height;
  double val, ecartMinMax, ecartMinVal;
  double min, max;
  GrFillRect(w,gc,0,0,160,4);
  GrFillRect(w,gc,0,4,4,240);
  GrFillRect(w,gc,0,236,160,4);
  GrFillRect(w,gc,156,4,4,240);
  GrFillRect(w,gc,0,40,160,2);
  /*GrFillRect(w,gc,10, 70, 2,130);
  //absscisse temp
  GrFillRect(w,gc,10, 200, 120,2);*/


  switch(current_data){
    // CASE ---------------------------------------------------TEMP
    case DATA_TEMP:

      // Calcul du min et du max
      min = data[0].T;
      max = data[0].T;
      for(i = 1; i<nbArchives && i<120; i++){
        if(data[i].T < min){
          min = data[i].T;
        }
        if(data[i].T > max){
          max = data[i].T;
        }
        printf("TEMP[%d]=%0.1f\n",i,data[i].T);
      }

      if(min!=max){
        //dessine les axes
        GrFillRect(w,gc,10, 70, 2,130);
        GrFillRect(w,gc,10, 200, 120,2);

        //dessine les points de la courbe
        for(i = 0; i<nbArchives && i<120; i++){
          val = data[i].T;
          ecartMinMax = max - min;
          ecartMinVal = val - min;
          height = 200 - ((ecartMinVal/ecartMinMax)*130);
          GrPoint(w,gc,(120-i)+10, height);
        }


        //convertit min et max, et le titre, selon l'unité
        switch(current_format_temp){
          case FORMAT_TEMP_CELSIUS :
            GrText(w, gc, 50, 30, "Historique - C", -1, GR_TFASCII);
            break;
          case FORMAT_TEMP_FAHRENHEIT:
            min = min*1.8+32;
            max = max*1.8+32;
            GrText(w, gc, 50, 30, "Historique - F", -1, GR_TFASCII);
            break;
          case FORMAT_TEMP_KELVIN :
            min = min+273.15;
            max = max+273.15;
            GrText(w, gc, 50, 30, "Historique - K", -1, GR_TFASCII);
            break;
        }
        //ecrit min et max
        sprintf(minC, "%.1f\n",  min);
        sprintf(maxC, "%.1f\n",  max);
        GrText(w, gc, 10, 220, minC, -1, GR_TFASCII);
        GrText(w, gc, 10, 60, maxC, -1, GR_TFASCII);

      }else{
        GrText(w, gc, 30, 30, "Historique - Temp", -1, GR_TFASCII);
        GrText(w, gc, 50, 90, "Donnees", -1, GR_TFASCII);
        GrText(w, gc, 45, 120, "monotones !", -1, GR_TFASCII);
      }
    break;

    // CASE ---------------------------------------------------PRESSURE
    case DATA_PRESSURE:
      // Calcul du min et du max
      min = data[0].P;
      max = data[0].P;
      for(i = 1; i<nbArchives && i<120; i++){
        if(data[i].P < min){
          min = data[i].P;
        }
        if(data[i].P > max){
          max = data[i].P;
        }
      }

      if(min!=max){
        //dessine les axes
        GrFillRect(w,gc,10, 70, 2,130);
        GrFillRect(w,gc,10, 200, 120,2);

        //dessine les points de la courbe
        for(i = 0; i<nbArchives && i<120; i++){
          val = data[i].P;
          ecartMinMax = max - min;
          ecartMinVal = val - min;
          height = 200 - ((ecartMinVal/ecartMinMax)*130);
          GrPoint(w,gc,(120-i)+10, height);
        }


        //convertit min et max, et le titre, selon l'unité
        switch(current_format_pressure){
          case FORMAT_PRESSURE_PASCAL :
            GrText(w, gc, 50, 30, "Historique - hPa", -1, GR_TFASCII);
            break;
          case FORMAT_PRESSURE_BAR:
            min = min*0.001;
            max = max*0.001;
            GrText(w, gc, 50, 30, "Historique - bar", -1, GR_TFASCII);
            break;
        }
        //ecrit min et max
        sprintf(minC, "%.1f\n",  min);
        sprintf(maxC, "%.1f\n",  max);
        GrText(w, gc, 10, 220, minC, -1, GR_TFASCII);
        GrText(w, gc, 10, 60, maxC, -1, GR_TFASCII);

      }else{
        GrText(w, gc, 20, 30, "Historique - Pression", -1, GR_TFASCII);
        GrText(w, gc, 50, 90, "Donnees", -1, GR_TFASCII);
        GrText(w, gc, 45, 120, "monotones !", -1, GR_TFASCII);
      }
    break;

    //-CASE --------------------------------------------------- HUMIDITY
    case DATA_HUMIDITY:
      // Calcul du min et du max
      min = 0;
      max = 100;

      //dessine les axes
      GrFillRect(w,gc,10, 70, 2,130);
      GrFillRect(w,gc,10, 200, 120,2);

      //dessine les points de la courbe
      for(i = 0; i<nbArchives && i<120; i++){
        val = data[i].RH;
        ecartMinMax = max - min;
        ecartMinVal = val - min;
        height = 200 - ((ecartMinVal/ecartMinMax)*130);
        GrPoint(w,gc,(120-i)+10, height);
      }
      //ecrit min et max
      sprintf(minC, "%.1f\n",  min);
      sprintf(maxC, "%.1f\n",  max);
      GrText(w, gc, 10, 220, minC, -1, GR_TFASCII);
      GrText(w, gc, 10, 60, maxC, -1, GR_TFASCII);
      GrText(w, gc, 50, 30, "Historique - %", -1, GR_TFASCII);
    break;
  }

}

int affichage_menu_03(GR_WINDOW_ID w, GR_GC_ID gc, t_captors_data data[], int nbArchives){

  char title[64], minC[64], maxC[64];
  int i,height;
  double val, ecartMinMax, ecartMinVal;
  double min, max;
  GrFillRect(w,gc,0,0,160,4);
  GrFillRect(w,gc,0,4,4,240);
  GrFillRect(w,gc,0,236,160,4);
  GrFillRect(w,gc,156,4,4,240);
  GrFillRect(w,gc,0,40,160,2);


  switch(current_data){
    // CASE ---------------------------------------------------TEMP
    case DATA_TEMP:

      // Calcul du min et du max
      min = data[0].T;
      max = data[0].T;
      for(i = 1; i<nbArchives && i<120; i++){
        if(data[i].T < min){
          min = data[i].T;
        }
        if(data[i].T > max){
          max = data[i].T;
        }
      }

      if(min!=max){
        //dessine les axes
        GrFillRect(w,gc,10, 70, 2,130);
        //dessine l'axe des abscisses
        if(0<=min){
          GrFillRect(w,gc,10, 200, 120,2);
        }else if(max<=0){
          GrFillRect(w,gc,10, 70, 120,2);
        }else{
          ecartMinMax = max - min;
          ecartMinVal = 0 - min;
          height = 200 - ((ecartMinVal/ecartMinMax)*130);
          GrFillRect(w,gc,10, height, 120,2);
        }

        //dessine les points de la courbe
        for(i = 0; i<nbArchives && i<120; i++){
          val = data[i].T;
          ecartMinMax = max - min;
          ecartMinVal = val - min;
          height = 200 - ((ecartMinVal/ecartMinMax)*130);
          GrPoint(w,gc,(120-i)+10, height);
        }


        //convertit min et max, et le titre, selon l'unité
        switch(current_format_temp){
          case FORMAT_TEMP_CELSIUS :
            GrText(w, gc, 50, 30, "Historique - C", -1, GR_TFASCII);
            break;
          case FORMAT_TEMP_FAHRENHEIT:
            min = min*1.8+32;
            max = max*1.8+32;
            GrText(w, gc, 50, 30, "Historique - F", -1, GR_TFASCII);
            break;
          case FORMAT_TEMP_KELVIN :
            min = min+273.15;
            max = max+273.15;
            GrText(w, gc, 50, 30, "Historique - K", -1, GR_TFASCII);
            break;
        }
        //ecrit min et max
        sprintf(minC, "%.2f\n",  min);
        sprintf(maxC, "%.2f\n",  max);
        GrText(w, gc, 10, 220, minC, -1, GR_TFASCII);
        GrText(w, gc, 10, 60, maxC, -1, GR_TFASCII);

      }else{
        GrText(w, gc, 20, 30, "Historique - Temp", -1, GR_TFASCII);
        GrText(w, gc, 50, 90, "Donnees", -1, GR_TFASCII);
        GrText(w, gc, 45, 120, "monotones !", -1, GR_TFASCII);
      }
    break;

    // CASE ---------------------------------------------------PRESSURE
    case DATA_PRESSURE:
      // Calcul du min et du max
      min = data[0].P;
      max = data[0].P;
      for(i = 1; i<nbArchives && i<120; i++){
        if(data[i].P < min){
          min = data[i].P;
        }
        if(data[i].P > max){
          max = data[i].P;
        }
      }

      if(min!=max){
        //dessine les axes
        GrFillRect(w,gc,10, 70, 2,130);
        //dessine l'axe des abscisses
        if(0<=min){
          GrFillRect(w,gc,10, 200, 120,2);
        }else if(max<=0){
          GrFillRect(w,gc,10, 70, 120,2);
        }else{
          ecartMinMax = max - min;
          ecartMinVal = 0 - min;
          height = 200 - ((ecartMinVal/ecartMinMax)*130);
          GrFillRect(w,gc,10, height, 120,2);
        }

        //dessine les points de la courbe
        for(i = 0; i<nbArchives && i<120; i++){
          val = data[i].P;
          ecartMinMax = max - min;
          ecartMinVal = val - min;
          height = 200 - ((ecartMinVal/ecartMinMax)*130);
          GrPoint(w,gc,(120-i)+10, height);
        }


        //convertit min et max, et le titre, selon l'unité
        switch(current_format_pressure){
          case FORMAT_PRESSURE_PASCAL :
            GrText(w, gc, 50, 30, "Historique - hPa", -1, GR_TFASCII);
            break;
          case FORMAT_PRESSURE_BAR:
            min = min*0.001;
            max = max*0.001;
            GrText(w, gc, 50, 30, "Historique - bar", -1, GR_TFASCII);
            break;
        }
        //ecrit min et max
        sprintf(minC, "%.2f\n",  min);
        sprintf(maxC, "%.2f\n",  max);
        GrText(w, gc, 10, 220, minC, -1, GR_TFASCII);
        GrText(w, gc, 10, 60, maxC, -1, GR_TFASCII);

      }else{
        GrText(w, gc, 10, 30, "Historique - Pression", -1, GR_TFASCII);
        GrText(w, gc, 50, 90, "Donnees", -1, GR_TFASCII);
        GrText(w, gc, 45, 120, "monotones !", -1, GR_TFASCII);
      }
    break;

    //-CASE --------------------------------------------------- HUMIDITY
    case DATA_HUMIDITY:
      // Calcul du min et du max
      min = data[0].RH;
      max = data[0].RH;
      for(i = 1; i<nbArchives && i<120; i++){
        if(data[i].RH < min){
          min = data[i].RH;
        }
        if(data[i].RH > max){
          max = data[i].RH;
        }
      }

      if(min!=max){
        //dessine les axes
        GrFillRect(w,gc,10, 70, 2,130);
        //dessine l'axe des abscisses
        if(0<=min){
          GrFillRect(w,gc,10, 200, 120,2);
        }else if(max<=0){
          GrFillRect(w,gc,10, 70, 120,2);
        }else{
          ecartMinMax = max - min;
          ecartMinVal = 0 - min;
          height = 200 - ((ecartMinVal/ecartMinMax)*130);
          GrFillRect(w,gc,10, height, 120,2);
        }

        //dessine les points de la courbe
        for(i = 0; i<nbArchives && i<120; i++){
          val = data[i].RH;
          ecartMinMax = max - min;
          ecartMinVal = val - min;
          height = 200 - ((ecartMinVal/ecartMinMax)*130);
          GrPoint(w,gc,(120-i)+10, height);
        }
        //ecrit min et max
        sprintf(minC, "%.2f\n",  min);
        sprintf(maxC, "%.2f\n",  max);
        GrText(w, gc, 10, 220, minC, -1, GR_TFASCII);
        GrText(w, gc, 10, 60, maxC, -1, GR_TFASCII);
        GrText(w, gc, 50, 30, "Historique - %", -1, GR_TFASCII);
      }else{
        GrText(w, gc, 10, 30, "Tendances - Humidite", -1, GR_TFASCII);
        GrText(w, gc, 50, 90, "Donnees", -1, GR_TFASCII);
        GrText(w, gc, 45, 120, "monotones !", -1, GR_TFASCII);
      }

    break;
  }
}


void get_Pressure(char data[64], t_ptr_captors_data p){

  float val = p->P; char t[64];int i = 0;
  if(current_format_pressure == FORMAT_PRESSURE_BAR){
    val *= 0.001;
    sprintf(t, "%.1f bar\n",val);

  }else{
    sprintf(t, "%.1f hPa\n",val);
  }
  for(i = 0;i<64;i++){
    data[i] = t[i];
  }

}

void get_Temp(char data[64], t_ptr_captors_data p){

  float val = p->T;
  char t[64];
  int i = 0;
  if(current_format_temp == FORMAT_TEMP_CELSIUS){
    sprintf(t, "%.1f C\n",val);
  }else if(current_format_temp == FORMAT_TEMP_FAHRENHEIT){
    val = val*1.8+32;
    sprintf(t, "%.1f F\n",val);
  }else{
    val = val + 273.15;
    sprintf(t, "%.1f K\n",val);
  }

  for(i = 0;i<64;i++){
    data[i] = t[i];
  }
}


int next(int current, int max){
  if(current == max){
    return 0;
  }else{
    return current + 1;
  }
}
