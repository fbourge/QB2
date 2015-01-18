/************************************************************
* Cubieboard 1 or 3 using I2C (Cubieboard 2 not tested, but should be working too)
* Debug and test PCA9685 module on i2c
*
* We use TWI-1 (I2C-1) on Cubieboard
*
*
* You need a kernel who support I2C (most of them do that)
* and also have to enable the TWI1-Port in script.bin
* [twi1_para]
* twi1_used = 1     <-- change 0 to 1
* twi1_scl = port:PB18<2><default><default><default>
* twi1_sda = port:PB19<2><default><default><default>
*
* Name        : main.cpp
* Author      : Florentin Bourge
* Version     :
* Created on  : Janv 17, 2015
*
* Copyright CC0 © 2015 Florentin Bourge  <honor2@hotmail.com>
****************************************************/

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h> //usr/include/linux
#include <syslog.h>		/* Syslog functionallity */
#include <inttypes.h>
#include <errno.h>



// Register Definitions

#define MODE1 0x00			//Mode  register  1
#define MODE2 0x01			//Mode  register  2
#define SUBADR1 0x02		//I2C-bus subaddress 1
#define SUBADR2 0x03		//I2C-bus subaddress 2
#define SUBADR3 0x04		//I2C-bus subaddress 3
#define ALLCALLADR 0x05     //LED All Call I2C-bus address
#define LED0 0x6			//LED0 start register
#define LED0_ON_L 0x6		//LED0 output and brightness control byte 0
#define LED0_ON_H 0x7		//LED0 output and brightness control byte 1
#define LED0_OFF_L 0x8		//LED0 output and brightness control byte 2
#define LED0_OFF_H 0x9		//LED0 output and brightness control byte 3
#define LED_MULTIPLYER 4	// For the other 15 channels
#define ALLLED_ON_L 0xFA    //load all the LEDn_ON registers, byte 0 (turn 0-7 channels on)
#define ALLLED_ON_H 0xFB	//load all the LEDn_ON registers, byte 1 (turn 8-15 channels on)
#define ALLLED_OFF_L 0xFC	//load all the LEDn_OFF registers, byte 0 (turn 0-7 channels off)
#define ALLLED_OFF_H 0xFD	//load all the LEDn_OFF registers, byte 1 (turn 8-15 channels off)
#define PRE_SCALE 0xFE		//prescaler for output frequency
#define CLOCK_FREQ 25000000.0 //25MHz default osc clock

#define I2C_BUS "/dev/i2c-1"
#define PCA9685_ADDRESS 0x40


	void open_device(char* device, unsigned char address);
	void reset(void);
	void setPWMFreq(int freq);
	void setPWM(uint8_t led, int on_value, int off_value);
	void setPWM2(uint8_t led, int value);
	int  getPWM(uint8_t led);

	int device_fd;
	


int main(int argc, char **argv)
{
    open_device(I2C_BUS, PCA9685_ADDRESS);
	
	printf("Open Device.  .......... done \n");
	reset();
	printf("Reset Device  .......... done \n");
	setPWMFreq(40);
	printf("setPWMFreq ................. done \n");
	
	setPWM(1, 1, 415);
	sleep(5);
	
	int i = 0;
	
	while(i<100) { 
		setPWM(1, 1, i*5);
		i++;
		//printf(".+1");
		usleep(300000);
	}
	
	setPWM(1, 1, 200);
	
return 0;
}


//! Initialise a connection
/*!
 \param device Name the I2C used, for example "/dev/i2c-1"
 \param address Slave device i2c address
 */
void open_device(char* device, unsigned char address)
{
	// Open i2c-device for reading and writing
	device_fd = open(device, O_RDWR);
	if (device_fd < 0 ) {
		perror("Failed to open I2C-Bus.\n");
		exit(1);
	}
	// Set the port options and set the address of the device
	if (ioctl(device_fd, I2C_SLAVE, address) != 0) {
		perror("Failed to open I2C-Device.\n");
		close(device_fd);
		exit(1);
	}
	else{
	printf("Connection .......... done \n");
	}
}


void reset(void){

		i2c_smbus_write_byte_data(device_fd,  MODE1, 0x00);//Normal mode
	//	i2c_smbus_write_byte_data(device_fd,  MODE2, 0x04);// totem pole

}

//! Set the frequency of PWM
/*!
 \param freq desired frequency. 40Hz to 1000Hz using internal 25MHz oscillator.
 */
void setPWMFreq(int freq){
		uint8_t prescale_val = (CLOCK_FREQ / 4096 / freq)  - 1;
		i2c_smbus_write_byte_data(device_fd, MODE1, 0x10); //sleep
		i2c_smbus_write_byte_data(device_fd, PRE_SCALE, prescale_val); // multiplyer for PWM frequency
		i2c_smbus_write_byte_data(device_fd, MODE1, 0x80); //restart
		i2c_smbus_write_byte_data(device_fd, MODE2, 0x04); //totem pole (default)
}
	
void setPWM(uint8_t led, int on_value, int off_value){
		i2c_smbus_write_byte_data(device_fd, LED0_ON_L + LED_MULTIPLYER * (led - 1), on_value & 0xFF);
		i2c_smbus_write_byte_data(device_fd, LED0_ON_H + LED_MULTIPLYER * (led - 1), on_value >> 8);
		i2c_smbus_write_byte_data(device_fd, LED0_OFF_L + LED_MULTIPLYER * (led - 1), off_value & 0xFF);
		i2c_smbus_write_byte_data(device_fd, LED0_OFF_H + LED_MULTIPLYER * (led - 1), off_value >> 8);
}

void setPWM2(uint8_t led, int value){
	// setPWM(led, 0, value);
}

int getPWM(uint8_t led){
	int ledval = 0;
	// ledval = i2c->read_byte(LED0_OFF_H + LED_MULTIPLYER * (led-1));
	// ledval = ledval & 0xf;
	// ledval <<= 8;
	// ledval += i2c->read_byte(LED0_OFF_L + LED_MULTIPLYER * (led-1));
	return ledval;
}


