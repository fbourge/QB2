/************************************************************
* Cubieboard 1 or 3 using I2C (Cubieboard 2 not tested, but should be working too)
* for displaying data on a 0.96" 128x64 Pixel OLED-Display with SSD1306 controller
*
* We use TWI-1 (I2C-1) on Cubieboard
*
* Wiring OLED with Cubieboard
* CB/CT-Port    OLED    Pullup
* PB18(SCK) --> CLK --> 2k2 --> 3V3
* PB19(SDA) --> DIN --> 2k2 --> 3V3
* 3V3       --> 3V3
* GND       --> GND
*               RES --> 3V3
*
* You need a kernel who support I2C (most of them do that)
* and also have to enable the TWI1-Port in script.bin
* [twi1_para]
* twi1_used = 1     <-- change 0 to 1
* twi1_scl = port:PB18<2><default><default><default>
* twi1_sda = port:PB19<2><default><default><default>
*
* copyrights: CC0
****************************************************/

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "ssd1306.h"


int main(int argc, char **argv)
{
	char buff1[12];
	char buff2[3];
//	char strs[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f, 0x00};  //"1234567890123456789012345678901234567890";
	unsigned char i;
//	char min = 0;

	struct tm *newtime;
	time_t timel;

	oled_open_device(I2C_BUS, OLED_SSD1306_ADDRESS);
	oled_Init();


// show Time on Oled-Display (yellow Lines)
//	for(n = 0; n < 1200; n++) {             // for testing
    while(0) {      // for ever
		time(&timel);
		newtime = localtime(&timel);
        i = 0;
        ChartoStr((char)newtime->tm_hour, buff2);
        buff1[i++] = buff2[0];
        buff1[i++] = buff2[1];
        buff1[i++] = ':';
        ChartoStr((char)newtime->tm_min, buff2);
        buff1[i++] = buff2[0];
        buff1[i++] = buff2[1];
        buff1[i++] = ':';
        ChartoStr((char)newtime->tm_sec, buff2);
        buff1[i++] = buff2[0];
        buff1[i++] = buff2[1];
        buff1[i] = '\0';
        oled_ShowString(16, 0, 12, buff1);
//        if(min != newtime->tm_min) {
//
//            // do something every minute
//
//            min = newtime->tm_min;          // set minute to current minute
//        }
        oled_Refresh_Gram();
        sleep(0.5);
	}

/*********************************************
* Examples
**********************************************/
// x = 0..127, y = 0..63, mode = 0 or 1, font_width = 6 or 12
//      oled_Fill(0,0,127,63,1);                // Filled rectangle x1,y1,x2,y2,mode
//      oled_Circle(50,30,20);                  // Circle x,y,radius
//      oled_Rectangle(20,10,30,40);            // Rectangle x1,y1,x2,y2
//      oled_Line(120,10,10,50);                // Line x1,y1,x2,y2
//        oled_DrawPixel(20,30,1);                // Set Pixel x,y,mode
//    oled_DrawLine(10,10,30,40,1);           // Draw Line x1,y1,x2,y2,mode
//      oled_ShowString(0, 0, 12, strs);        // write string x,y,font width, string
int x=0, y=0, n=0;

    if(argc > 1) {                          // check for commandline
        if(strcmp(argv[1], "-s") == 0) {    // commandline = ./oled -s x y strings
            x = (unsigned char)atoi(argv[2]);
            y = (unsigned char)atoi(argv[3]);
            for(n = 4; n < argc; n++) {
                oled_ShowString( x, y, 6, argv[n]);
                y += 16;
            }
        }
        if(strcmp(argv[1], "-r") == 0) {    // comandline = ./oled -r x1 y1 x2 y2
            oled_Rectangle((unsigned char)atoi(argv[2]), (unsigned char)atoi(argv[3]), (unsigned char)atoi(argv[4]), (unsigned char)atoi(argv[5]));
            }
          oled_Refresh_Gram();
        
	if(strcmp(argv[1], "-f") == 0) {    // comandline = ./oled -r x1 y1 x2 y2 mode
            oled_Fill((unsigned char)atoi(argv[2]), (unsigned char)atoi(argv[3]), (unsigned char)atoi(argv[4]), (unsigned char)atoi(argv[5]),(unsigned char)atoi(argv[6]));
            }
          oled_Refresh_Gram();
        }

	return 0;
}
