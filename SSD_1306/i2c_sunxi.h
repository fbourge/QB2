/*
A user-space program to get data from an I2C device.
                                Gustavo Zamboni
*/
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
 
char buf[10];
int com_serial;
int failcount;
 
//////////
// Init I2Cdevice
//////////
int i2c_init(char filename[40], int addr);

//////////
// Set pointer address
//////////
void i2c_set_pointer(int address,int value,int file);

//////////
// Read n bytes
////////// 
char * i2c_read(int add1, int add2, int nbytes,int file);

//////////
// Write n bytes
//////////
void i2c_write(int add1,int nbytes,char value[10],int file);


