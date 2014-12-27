/************************************************************************************************
    *                                                                                           *
    * Author:  Florentin, Bourge                                                                *
    *                                                                                           *
    * Inspired by : likeaferrari.wordpress.com/2013/12/02/simple-gpio-example-with-cubieboard2/ *
    *                                                                                           *
    *                                                                                           *
    * Purpose:  Generate PWM on a Cubieboard v2 GPIO (gpio1 pin3 default)                       *
    *                                                                                           *
    * Usage:                                                                                    *
    *      run the programm with frequency and duty cycle passed in arguments                   *
    ********************************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

FILE *fp;

int main(int argc, char* argv[])
{
char set_value[4];
int toggle = 0, freq, period, timeOn, timeOff;
float dutyCycle;

if(argc==3)
	{
		sscanf(argv[1],"%i",&freq);
		sscanf(argv[2],"%f",&dutyCycle);
	}
else exit(1);

period = 1000000/freq;
timeOn = (dutyCycle/100)*period;
timeOff = period - timeOn;
if(timeOn<0 || timeOff<0 || timeOn>period || timeOff>period) exit(1);
printf("Frequency = %d, Period = %d µs and Duty Cycle = %f,  Time ON = %d, Time OFF = %d \n",freq, period, dutyCycle, timeOn, timeOff); 


if((fp = fopen("/sys/class/gpio/export","ab"))==NULL)
	{
	printf("Cannot open export file \n");
	exit(1);
	}
	rewind(fp);

	strcpy(set_value,"1");
	fwrite(&set_value, sizeof(char), 1, fp);
	fclose(fp);

if ((fp = fopen("/sys/class/gpio/gpio1_pg3/direction","rb+"))==NULL)
	{
	printf("Cannot open direction file \n");
	exit(1);
	}
	rewind(fp);

	strcpy(set_value,"out");
	fwrite(&set_value, sizeof(char), 3, fp);
	fclose(fp);

if ((fp = fopen("/sys/class/gpio/gpio1_pg3/value","rb+"))==NULL)
	{
	printf("Cannot open value file \n");
	exit(1);
	}
	rewind(fp);

	strcpy(set_value,"1");
	fwrite(&set_value, sizeof(char), 1, fp);
	fclose(fp);

	


while(1)
{
if((fp = fopen("/sys/class/gpio/gpio1_pg3/value", "rb+")) == NULL)
	{
		printf("Cannot open value file \n");
		exit(1);
	}
		rewind(fp);
		
		strcpy(set_value,"1");
		fwrite(&set_value, sizeof(char), 1,fp);
		fclose(fp);
		usleep(timeOn);


		if((fp = fopen("/sys/class/gpio/gpio1_pg3/value", "rb+")) == NULL)
	{
		printf("Cannot open value file \n");
		exit(1);
	}
		rewind(fp);
		
		strcpy(set_value,"0");
		fwrite(&set_value, sizeof(char), 1,fp);
		fclose(fp);
		usleep(timeOff);
		
	}
	return 0;

}
