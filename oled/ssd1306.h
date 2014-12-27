/***********************************************************************
* SSD1306 driver for 128x64 0.96" OLED-Displays on Cubieboard/Cubietruck
*
* we use the I2C-Bus (TWI-1) on CB/CT
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

#ifndef SSD1306_H_INCLUDED
#define SSD1306_H_INCLUDED

#define I2C_BUS "/dev/i2c-1"
#define OLED_SSD1306_ADDRESS 0x3c

// i2c bus transfer definition
// SLAVE_ADDRESS = 0b0011 1100 = write
// SLAVE_ADDRESS = 0b0011 1101 = read, not availlable on serial
// commandregister is 0x00 + command
// dataregister is 0x40 + data it writes data to GDDRAM,
// in most addressing modes the GDDRAM pointer will be increased automaticaly
// so we can use the i2c write block command (max. 32 bytes)

#define MAX_CHAR_POSX		127	// 0...127
#define MAX_CHAR_POSY		63	// 0...63
#define COMMAND			    0x00
#define DATA			    0x40
#define OLED_CMD		    0
#define OLED_DAT		    1

// Oled commands for more details read datasheet
#define CONTRAST 		    0x81	// + 1...256
#define DISPLAY_ENTIRE_ON	0xA4	// or 0xA5
#define DISPLAY_NORMAL		0xA6	// normal display
#define DISPLAY_INVERS		0xA7	// invers display
#define DISPLAY_ON		    0xAF	// display on
#define DISPLAY_OFF		    0xAE	// display off
#define SET_LOW_COLUMN		0x00	// set lower column address (00..0F)
#define SET_HIGH_COLUMN		0x10	// set higher column address (10..1F)
#define SET_START_LINE		0x40	// set display start line 0x40...0x7F
#define SET_BRIGHTNESS		0xCF	// set SEG Output Current Brightness
#define SET_SEG_COLUMN_MAPPING	0xA1	// set SEG remapping
#define SET_COM_ROW_SCAN_DIR	0xC0	// set COM/Row Scan Direction
#define SET_MULTIPLEX_RATIO	0xA8	// set multiplex ratio(1 to 64)
#define SET_MEMORY_MODE		0x20	// set memory addressing mode + 0=horizontal or 1=vertikal or 2=page
#define SET_COLUMN_ADDR		0x21	// set column address + col. start address (0..127) + col. end address (0..127)
#define SET_PAGE_ADDR		0x22	// set page address + page start address (0..7) + page end address (0..7)


int oled_fd;
unsigned char oled_GRAM[128][8];

void oled_open_device(char* device, unsigned char address);
void oled_WriteByte(unsigned char value, unsigned char cmd);
void oled_SetPos(unsigned char x, unsigned char y);
void oled_DisplayOn(void);
void oled_DisplayOff(void);
void oled_Refresh_Gram(void);
void oled_Refresh_Yellow(void);
void oled_Refresh_Blue(void);
void oled_ClearDisplay(void);
void oled_SetPixel(unsigned char x, unsigned char y, unsigned char mode);
void oled_DrawPixel(unsigned char x, unsigned char y, unsigned char mode);
void oled_Fill(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char dot);
void oled_Line(unsigned int X1,unsigned int Y1,unsigned int X2,unsigned int Y2);
void oled_Circle(unsigned char cx, unsigned char cy ,unsigned char radius);
void oled_Rectangle(unsigned char x, unsigned char y, unsigned char b, unsigned char a);
void oled_DrawChar(unsigned char x, unsigned char y, unsigned char chr);
void oled_DrawCharLarge(unsigned char x, unsigned char y, unsigned char c);
void oled_ShowString(unsigned char x, unsigned char y, unsigned char font_width, char *str);
void oled_Init(void);
void ChartoStr(char z, char* Buffer);


#endif // SSD1306_H_INCLUDED
