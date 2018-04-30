/* ADC  driver for S3C44B0X - uClinux 2.4 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


#define ADC_VERSION           0x400
#define ADC_SAMPLING          0x800
#define ADC_CHANNEL           0x500

/* accessADC /dev/adc 1 for the first channel */
int main(int argc, char**argv)
{
	short data;
	int fd, ret, n;
	char adc_device[20] ;

	if (argc < 3) 
	{
		printf("Usage : accessADC [DEVICE]  [channel]\n");
		printf("Ex    : accessADC /dev/adc 1\n");
		return (-1);
	}

	strcpy(adc_device, argv[1]) ;
	if ((fd = open(adc_device,O_RDWR)) < 0 ) 
	{
		perror("open");
		printf("Error opening %s\n", adc_device);
		return (-1);
	}

	sscanf(argv[2], "%x", &n);
	printf("\n--- Test de lecture ADC channel %d ---\n\n", n);
	ret = ioctl(fd, ADC_CHANNEL, n);
        if(ret <0)
        {
             perror("ADC_CHANNEL ioctl cmd");
             close(fd);
             return (-1);
	}

	read(fd, &data, 2);
	printf("val[%d] = %d\n", n, (int)data);
	close(fd);
	return 0;
}


