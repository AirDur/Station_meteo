/* i2c */

#include "44b.h"
#include<time.h>
#include<signal.h>

#define MWINCLUDECOLORS
#include "nano-X.h"
#include <graphics.h>

#include <stdio.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>



#define ADC_VERSION           0x400
#define ADC_SAMPLING          0x800
#define ADC_CHANNEL           0x500
#define I2C_SLAVE             0x0703
#define ADC_CHANNEL_PRESSURE	4
#define ADC_CHANNEL_HUMIDITY	5


unsigned int KEYBOARD_STATUS(void)
{
   rPCONG=0x0;	//IN7~0
   rPUPG=0x0;   	//pull up enable
   return (~((rPDATG&0xf0)>>4)&0xF);
}

//deux fonctions d'attentes pour les boutons
int __nsleep(const struct timespec *req, struct timespec *rem)
{
    struct timespec temp_rem;
    if(nanosleep(req,rem)==-1)
        __nsleep(rem,&temp_rem);
    else
        return 1;
}

int msleep(unsigned long milisec)
{
    struct timespec req={0},rem={0};
    time_t sec=(int)(milisec/1000);
    milisec=milisec-(sec*1000);
    req.tv_sec=sec;
    req.tv_nsec=milisec*1000000L;
    __nsleep(&req,&rem);
    return 1;
}




int main(int argc, char**argv)
{	  
	unsigned int KbStatus;
   	int delay_val = 200;
	GR_WINDOW_ID w;
        GR_GC_ID gc;
        GR_EVENT event;
	short data_H,data_P;
	double temp,RH,P,VOUT_H,VS_H,VOUT_P,VS_P; 
	char valeur1[256];
	char valeur2[256];
	char valeur3[256];
	int fd, ret;
	unsigned char val[2];
	
	VS_H=5;
	VS_P=5.1;
	val[0]=0x01;
	val[1]=0x60;

	//exécution des modules et de nano-X
	system("insmod adc_s3c44.o");
	system("insmod i2c_s3c44.o");
	system("./nano-X &");
	 
 
	//calcul de la temperature

	if ((fd = open("/dev/i2c0",O_RDWR)) < 0 ) //on ouvre le fichier i2c0 en lecture et ecriture
	{	//si le fichier n'existe pas on affiche un message d'erreur
		perror("open");
		printf("Error opening /dev/i2c0");
		return (-1);
	}

	ret = ioctl(fd, I2C_SLAVE, 0x92);
        if(ret <0)
        {	//si le fichier n'existe pas on affiche un message d'erreur
             perror("ADC_CHANNEL ioctl cmd");
             close(fd);
             return (-1);
	}
	
	write(fd, val ,2);//ecriture de deux valeurs sur fd
	read(fd, val, 1);//lecture d'une valeur sur fd
	printf("val[%d] = %x\n", 0x92, val[0]);
	
	val[0]=0x00;
	write(fd, val ,1);
	read(fd, val, 2);
	//calcul
	temp=(val[0] << 4) | (val[1] >> 4);//décalage 
	temp=temp*0.0625;
	

	// calcul de l'humidity
	
	if ((fd = open("/dev/adc",O_RDWR)) < 0 )//on ouvre le fichier adc en lecture et ecriture 
	{
		perror("open");
		printf("Error opening /dev/adc");
		return (-1);
	}
	
	ret = ioctl(fd, ADC_CHANNEL, ADC_CHANNEL_HUMIDITY);
        if(ret <0)
        {
             perror("ADC_CHANNEL_HUMIDITY ioctl cmd");
             close(fd);
             return (-1);
	}
	
	read(fd, &data_H, 2);//lecture
	//calcul
	VOUT_H=(2.5/1024)*data_H*2;
	RH=(VOUT_H - VS_H * 0.16)/(VS_H*0.0062);
	RH=RH/(1.0546-(0.00216*temp));

 
	
	//calcul de la pression
 
	ret = ioctl(fd, ADC_CHANNEL, ADC_CHANNEL_PRESSURE);
        if(ret <0)
        {    //si le fichier n'existe pas on affiche un message d'erreur
             perror("ADC_CHANNEL_PRESSURE ioctl cmd");
             close(fd);
             return (-1);
	}
	
	read(fd, &data_P, 2);
	VOUT_P=(2.5/1024)*data_P*2;
	P=(VOUT_P + (VS_P * 0.1518))/(VS_P * 0.01059);
	P=P*10;
	 
	//stockage des valeurs sous forme de chaine de caractére
	
	sprintf(valeur1,"T= %.1f C \n",temp);//stockage de la valeur temp dans valeur1
	sprintf(valeur2,"H= %.1f %%\n" ,RH);//stockage de la valeur RH dans valeur2 
	sprintf(valeur3,"P= %.1f kPa \n",P);//stockage de la valeur P dans valeur3


	//configuration pour les préférences d'afichage   	
	if (GrOpen() < 0) 
	{
        	printf("Can't open graphics\n");
        	exit(1);
    	}

    	w = GrNewWindow(GR_ROOT_WINDOW_ID, 20, 100, 120, 120,
        5, WHITE, BLACK);
    	gc = GrNewGC();
    	GrSetGCForeground(gc, BLACK);
    	GrSetGCUseBackground(gc, GR_FALSE);
    	GrSelectEvents(w, GR_EVENT_MASK_EXPOSURE);
    	GrMapWindow(w);

  	//utilisation des boutons
   	while(1)
    	{  
	          KbStatus = KEYBOARD_STATUS();
    		  if((KbStatus & 0x1)==1) 
    		 {
                   printf("Button Ox1, S2 pressed\n") ;
    	 		while( KEYBOARD_STATUS() == KbStatus) ;
    	    		msleep(delay_val) ;
       		}	        
    	
    		if((KbStatus & 0x2)==2) //bouton 3
    		{
			//affichage et positionnement des valeurs(température, humidité et pression)sur l'ecran LCD
	  		for (;;) {
        		GrGetNextEvent(&event);
        		switch (event.type) {
       	 		case GR_EVENT_TYPE_EXPOSURE:
        		GrText(w, gc, 0, 20,valeur1 , -1, GR_TFASCII);
	  		GrText(w, gc, 0, 60,valeur2 , -1, GR_TFASCII);
	  		GrText(w, gc, 0, 100,valeur3 , -1, GR_TFASCII);
			while( KEYBOARD_STATUS() == KbStatus) ;
    		}	        
    	
    		if((KbStatus & 0x4)==4) 
    		{
    			printf("Button 0x4, S4 pressed\n") ;
    			while( KEYBOARD_STATUS() == KbStatus) ;
    		}	        
    	
    		if((KbStatus & 0x8)==8) 
    		{
    			printf("Button 0x8, S5 pressed\n") ;
    			while( KEYBOARD_STATUS() == KbStatus) ;
    		}
  
    	}


        break;
    }
    }
    GrClose();

	
	close(fd);
	return 0;
}

 

