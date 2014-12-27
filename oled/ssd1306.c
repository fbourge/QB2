/***********************************************************************
* SSD1306 driver for 128x64 0.96" OLED-Displays on Cubieboard/Cubietruck
*
* we use the I2C-Bus on CB/CT
* GPIO-Pin PB18 --> SCK with Pullup 2k2 --> 3V3
* GPIO-Pin PB19 --> SDA with Pullup 2k2 --> 3V3
*
* wiring OLED with Cubieboard:
* OLED    CB/CT
* VCC --> 3V3
* GND --> GND
* NC
* DIN --> SDA <-- 2k2 Ohm --> 3V3
* CLK --> SCK <-- 2k2 Ohm --> 3V3
* CS
* D/C
* RES --> 3V3
*
* copyrights: CC0
********************/



#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "font_12x16.h"
#include "font_6x8.h"
#include "ssd1306.h"

///////////////////////////////////////////////////////////
// Initialise a connection
///////////////////////////////////////////////////////////
void oled_open_device(char* device, unsigned char address)
{
	// Open i2c-device for reading and writing
	oled_fd = open(device, O_RDWR);
	if (oled_fd < 0 ) {
		perror("Failed to open I2C-Bus.\n");
		exit(1);
	}
	// Set the port options and set the address of the device
	if (ioctl(oled_fd, I2C_SLAVE, address) < 0) {
		perror("Failed to open I2C-Device.\n");
		close(oled_fd);
		exit(1);
	}
}


///////////////////////////////////////////////////////////
// Write byte to Oled 0 = command 1 = value
///////////////////////////////////////////////////////////
void oled_WriteByte(unsigned char value, unsigned char cmd)
{
	if(cmd) {
		// write data byte
		if (i2c_smbus_write_byte_data(oled_fd, DATA, value) < 0) {
			perror("Failed to write Byte to Device.\n");
			close(oled_fd);
			exit(1);
		}
	}
	else {
		// write command byte
		if (i2c_smbus_write_byte_data(oled_fd, COMMAND, value) < 0) {
			perror("Failed to write Byte to Device.\n");
			close(oled_fd);
			exit(1);
		}
	}
}

///////////////////////////////////////////////////////////
// Set cursor position
///////////////////////////////////////////////////////////
void oled_SetPos(unsigned char x, unsigned char y)
{
	oled_WriteByte(0xb0 + y, OLED_CMD);
	oled_WriteByte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	oled_WriteByte((x & 0x0f) | 0x01, OLED_CMD);
}

///////////////////////////////////////////////////////////
// Oled display on
///////////////////////////////////////////////////////////
void oled_DisplayOn(void)
{
	oled_WriteByte(0X8D, OLED_CMD);		// charge pump
	oled_WriteByte(0X14, OLED_CMD);		// enable
	oled_WriteByte(DISPLAY_ON, OLED_CMD);
}

///////////////////////////////////////////////////////////
// Oled display off
///////////////////////////////////////////////////////////
void oled_DisplayOff(void)
{
	oled_WriteByte(0X8D, OLED_CMD);		// charge pump
	oled_WriteByte(0X10, OLED_CMD);		// disable
	oled_WriteByte(DISPLAY_OFF, OLED_CMD);
}

///////////////////////////////////////////////////////////
// Refresh GRAM Oled
///////////////////////////////////////////////////////////
void oled_Refresh_Gram(void)
{
	unsigned char i, n, x, z;
	unsigned char bl[32];

	for(i = 0; i < 8; i++) {	// we have 8 pages with 128 bytes see page 25 datasheet
		z = 0;
		oled_WriteByte(0xb0 + i, OLED_CMD);	// set page address
		oled_WriteByte(0x00, OLED_CMD);
		oled_WriteByte(0x10, OLED_CMD);
		// we can only write 32 Byte at once (i2c_smbus_write_block)
		for(n = 0; n < 4; n++) {
			for(x = 0; x < 32; x++) {
				bl[x] = oled_GRAM[z][i];
				z++;
			}
			if(i2c_smbus_write_i2c_block_data(oled_fd, DATA, 32, bl) < 0) {
				perror("Failed to write block to Device.\n");
				close(oled_fd);
				exit(1);
			}
		}
	}
}

void oled_Refresh_Yellow(void)
{
	unsigned char i, n, x, z;
	unsigned char bl[32];

	for(i = 0; i < 3; i++) {	// we have 2 pages yellow
		x = 0;
		oled_WriteByte(0xb0 + i, OLED_CMD);	// set page address
		oled_WriteByte(0x00, OLED_CMD);
		oled_WriteByte(0x10, OLED_CMD);
		// we can only write 32 Byte at once (i2c_smbus_write_block)
		for(n = 0; n < 4; n++) {
			for(z = 0; z < 32; z++) {
				bl[z] = oled_GRAM[z][i];
				x++;
			}
			if(i2c_smbus_write_i2c_block_data(oled_fd, DATA, 32, bl) < 0) {
				perror("Failed to write block to Device.\n");
				close(oled_fd);
				exit(1);
			}
		}
	}
}

void oled_Refresh_Blue(void)
{
	unsigned char i, n, x, z;
	unsigned char bl[32];

	for(i = 2; i < 8; i++) {	// we have 6 pages blue
		z = 0;
		oled_WriteByte(0xb0 + i, OLED_CMD);	// set page address
		oled_WriteByte(0x00, OLED_CMD);
		oled_WriteByte(0x10, OLED_CMD);
		// we can only write 32 Byte at once (i2c_smbus_write_block)
		for(n = 0; n < 4; n++) {
			for(x = 0; x < 32; x++) {
				bl[x] = oled_GRAM[z][i];
				z++;
			}
			if(i2c_smbus_write_i2c_block_data(oled_fd, DATA, 32, bl) < 0) {
				perror("Failed to write block to Device.\n");
				close(oled_fd);
				exit(1);
			}
		}
	}
}

///////////////////////////////////////////////////////////
// Clear Oled display
///////////////////////////////////////////////////////////
void oled_ClearDisplay(void)
{
	unsigned char i, n;

	for(i = 0; i < 8; i++)
	{
		for(n = 0; n < 128; n++)
		{
			oled_GRAM[n][i] = 0x00;
		}
	}
	oled_Refresh_Gram();
}

///////////////////////////////////////////////////////////
// Oled Set Pixel
///////////////////////////////////////////////////////////
void oled_SetPixel(unsigned char x, unsigned char y, unsigned char mode)
{
	unsigned char temp, y1;

	if(x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
		return;
	y1 = y % 8;
	temp = oled_GRAM[x][y1];		//get old state

	if(mode)
		temp |= (1 << (y1 % 8));	// show Pixel
	else
		temp &= (1 << ~(y1 % 8));	// clear Pixel
	oled_GRAM[x][y1] = temp;
}

///////////////////////////////////////////////////////////
// Write Point to Oled
///////////////////////////////////////////////////////////
void oled_DrawPixel(unsigned char x, unsigned char y, unsigned char mode)
{
	unsigned char pos, bx, temp = 0;

	if(x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
		return;
	pos = 7 - y / 8;
	bx = y % 8;
	temp = 1 << (7 - bx);
	if(mode)
	{
		oled_GRAM[x][pos] |= temp;
	}
	else
	{
		oled_GRAM[x][pos] &= ~temp;
	}
}

///////////////////////////////////////////////////////////
// Write filled Rectangle to Oled
///////////////////////////////////////////////////////////
void oled_Fill(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char dot)
{
	unsigned char x, y;

	for(x = x1; x <= x2; x++)
	{
		for(y = y1; y <= y2; y++)
			oled_DrawPixel(x, y, dot);
	}
}

///////////////////////////////////////////////////////////
// Write Line to Oled
///////////////////////////////////////////////////////////
void oled_Line(unsigned int X1,unsigned int Y1,unsigned int X2,unsigned int Y2)
{
	int CurrentX, CurrentY, Xinc, Yinc, Dx, Dy, TwoDx, TwoDy, TwoDxAccumulatedError, TwoDyAccumulatedError;
	unsigned char color = 1;

	Dx = (X2-X1);
	Dy = (Y2-Y1);

	TwoDx = Dx + Dx;
	TwoDy = Dy + Dy;

	CurrentX = X1;
	CurrentY = Y1;

	Xinc = 1;
	Yinc = 1;

	if(Dx < 0) {
		Xinc = -1;
		Dx = -Dx;
		TwoDx = -TwoDx;
	}

	if (Dy < 0) {
		Yinc = -1;
		Dy = -Dy;
		TwoDy = -TwoDy;
	}

	oled_DrawPixel(X1,Y1, color);

	if ((Dx != 0) || (Dy != 0)) {
		if (Dy <= Dx) {
			TwoDxAccumulatedError = 0;
			do {
				CurrentX += Xinc;
				TwoDxAccumulatedError += TwoDy;
				if(TwoDxAccumulatedError > Dx) {
					CurrentY += Yinc;
					TwoDxAccumulatedError -= TwoDx;
				}
			oled_DrawPixel(CurrentX,CurrentY, color);
			} while (CurrentX != X2);
		}
		else {
			TwoDyAccumulatedError = 0;
			do {
				CurrentY += Yinc;
				TwoDyAccumulatedError += TwoDx;
			if(TwoDyAccumulatedError>Dy) {
				CurrentX += Xinc;
				TwoDyAccumulatedError -= TwoDy;
			}
			oled_DrawPixel(CurrentX,CurrentY, color);
			} while (CurrentY != Y2);
		}
	}
}

///////////////////////////////////////////////////////////
// Write Circle to Oled
///////////////////////////////////////////////////////////
void oled_Circle(unsigned char cx, unsigned char cy ,unsigned char radius)
{
	int x, y, xchange, ychange, radiusError;
	unsigned char color = 1;

	x = radius;
	y = 0;
	xchange = 1 - 2 * radius;
	ychange = 1;
	radiusError = 0;

	while(x >= y) {
		oled_DrawPixel(cx+x, cy+y, color);
		oled_DrawPixel(cx-x, cy+y, color);
		oled_DrawPixel(cx-x, cy-y, color);
		oled_DrawPixel(cx+x, cy-y, color);
		oled_DrawPixel(cx+y, cy+x, color);
		oled_DrawPixel(cx-y, cy+x, color);
		oled_DrawPixel(cx-y, cy-x, color);
		oled_DrawPixel(cx+y, cy-x, color);
		y++;
		radiusError += ychange;
		ychange += 2;
		if ( 2*radiusError + xchange > 0 ) {
			x--;
			radiusError += xchange;
			xchange += 2;
		}
	}
}

///////////////////////////////////////////////////////////
// Write Rectangle to Oled
///////////////////////////////////////////////////////////
void oled_Rectangle(unsigned char x, unsigned char y, unsigned char b, unsigned char a)
{
  unsigned char j, color = 1;

  for (j = 0; j < a; j++) {
		oled_DrawPixel(x, y + j, color);
		oled_DrawPixel(x + b - 1, y + j, color);
	}

  for (j = 0; j < b; j++)	{
		oled_DrawPixel(x + j, y, color);
		oled_DrawPixel(x + j, y + a - 1, color);
	}
}

void oled_DrawChar(unsigned char x, unsigned char y, unsigned char c)
{
  unsigned char x1, y1, w1, x2 = 0, y2 = 0;

	for (y1 = 0; y1 < 8; y1 += 1) {
        w1 = font_6x8[c][y1];		// Fontsize 12x16 Bits
        w1 >>= 2;
        for (x1 = 0; x1 < 8; x1++) {
			if((x + x2) > MAX_CHAR_POSX || (y + y2) > MAX_CHAR_POSY)
				return;
			if (w1 & 1)
				oled_DrawPixel(x + x2, y + y2, 1);
			else
				oled_DrawPixel(x + x2, y + y2, 0);
			w1 >>= 1;
			x2++;
		}
	y2++;
	x2 = 0;
	}
}

///////////////////////////////////////////////////////////
// Large String to Oled font 12x16
///////////////////////////////////////////////////////////
void oled_DrawCharLarge(unsigned char x, unsigned char y, unsigned char c)
{
   unsigned char x1, y1, w1, w2, x2 = 0, y2 = 0;

	for (y1 = 0; y1 < 32; y1 += 2) {
		w1 = font_12x16[c][y1];		// Fontsize 12x16 Bits
		w2 = font_12x16[c][y1+1];
		w1 >>= 4; 			        // only 12 Bits
		for (x1 = 0; x1 < 4; x1++) {
			if((x + x2) > MAX_CHAR_POSX || (y + y2) > MAX_CHAR_POSY)
				return;
			if (w1 & 1)
				oled_DrawPixel(x + x2, y + y2, 1);
			else
				oled_DrawPixel(x + x2, y + y2, 0);
			w1 >>= 1;
	 		x2++;
		}
		for (x1 = 0; x1 < 8; x1++) {
			if((x + x2) > MAX_CHAR_POSX || (y + y2) > MAX_CHAR_POSY)
				return;
			if (w2 & 1)
				oled_DrawPixel(x + x2, y + y2, 1);
			else
				oled_DrawPixel(x + x2, y + y2, 0);
			w2 >>= 1;
			x2++;
		}
	y2++;
	x2 = 0;
	}
}

///////////////////////////////////////////////////////////
// Write String to Oled x, y, font_size_width, string
///////////////////////////////////////////////////////////
void oled_ShowString(unsigned char x, unsigned char y, unsigned char font_width, char *str)
{
	int i;

    if(font_width == 12) {
        for(i = 0; str[i]; i++) {
            if((x + font_width) > MAX_CHAR_POSX) {
                x = 0;
                y += 16;                // next column
            }
            if(y > MAX_CHAR_POSY) {
                y = x = 0;              // from beginning
            }
            oled_DrawCharLarge(x, y, str[i]);
            x += font_width;
        }
    }
    if(font_width == 6) {
        for(i = 0; str[i]; i++) {
            if((x + font_width) > MAX_CHAR_POSX) {
                x = 0;
                y += 8;                // next column
            }
            if(y > MAX_CHAR_POSY) {
                y = x = 0;              // from beginning
            }
            oled_DrawChar(x, y, str[i]);
            x += font_width;
        }
	}
}

///////////////////////////////////////////////////////////
// Init Oled
///////////////////////////////////////////////////////////
void oled_Init(void)
{
	oled_WriteByte(0xA8, OLED_CMD);		// Set MUX ratio
	oled_WriteByte(0x3F, OLED_CMD);		//
	oled_WriteByte(0xD3, OLED_CMD);		// Set display offset
	oled_WriteByte(0x00, OLED_CMD);		//
	oled_WriteByte(0x40, OLED_CMD);		// Set display start line
	oled_WriteByte(0xA1, OLED_CMD);		// Set segment remap
	oled_WriteByte(0xC0, OLED_CMD);		// Set COM output scan direction
	oled_WriteByte(0xDA, OLED_CMD);		// Set COM pins hardware configuration
	oled_WriteByte(0x12, OLED_CMD);		//
	oled_WriteByte(0x81, OLED_CMD);		// Set contrast control
	oled_WriteByte(0x3F, OLED_CMD);		//
	oled_WriteByte(0xA4, OLED_CMD);		// Disable entire display on
	oled_WriteByte(0xA6, OLED_CMD);		// Set normal display 0xA7 = invers
	oled_WriteByte(0xD5, OLED_CMD);		// Set osc-frequency
	oled_WriteByte(0x80, OLED_CMD);		//
	oled_WriteByte(0x8D, OLED_CMD);		// Enable charge pump regulator
	oled_WriteByte(0x14, OLED_CMD);		//
	oled_WriteByte(0x20, OLED_CMD);		// set addressing mode
	oled_WriteByte(0x02, OLED_CMD);		// page addressing
	oled_WriteByte(0x00, OLED_CMD);		// set lower column start address
	oled_WriteByte(0x10, OLED_CMD);		// set higher column start address
	oled_WriteByte(0xAF, OLED_CMD);		// Display on
}


///////////////////////////////////////////////////////////
// convert char to string with leading 0 for values < 10
///////////////////////////////////////////////////////////
void ChartoStr( char z, char* Buffer )
{
  unsigned char u, c1,c2;

	u = z;
	c2 = '0' + u % 10;
	u /= 10;
	c1 = '0' + u;
    if(z <= 9)
        c1 = '0';
	Buffer[0] = c1;
	Buffer[1] = c2;
	Buffer[2] = '\0';
}
