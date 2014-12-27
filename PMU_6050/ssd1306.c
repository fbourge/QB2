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
