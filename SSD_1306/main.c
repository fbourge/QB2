

#include "i2c_sunxi.h"
#include "my_SSD1306.h"



int main(){

char *buffer;
char buf[10];
 
int file=i2c_init("/dev/i2c-1",0x3c); //dev,slavei2caddr
 
//buf[0] = 0x41;
//buf[1] = 0xFF;
//i2c_write(0xE6,0x0E,2,buf,file); //this will write value 0x41FF to the address 0xE60E
 
//buffer=(char *)i2c_read(0xE6,0x0E,4,file); //reading a 4 bytes value at the address 0xE60E



begin(SSD1306_EXTERNALVCC, SSD1306_I2C_ADDRESS);



 
close(file);

return 0;
}