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


#define MPU6050_ADDRESS 0x68

//const int MPU=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
float Tmp_corrected, AcX_corr, AcY_corr, AcZ_corr,GyX_corr, GyY_corr, GyZ_corr ;

int main(int argc, char **argv)
{
    oled_open_device(I2C_BUS, MPU6050_ADDRESS);
	
	i2c_smbus_write_byte_data(oled_fd,  0x6B , 0); // PWR_MGMT_1 register // set to zero (wakes up the MPU-6050)


    while(1){
       AcX= i2c_smbus_read_byte_data(oled_fd, 0x3B)<<8|i2c_smbus_read_byte_data(oled_fd, 0x3C); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
       AcY= i2c_smbus_read_byte_data(oled_fd, 0x3D)<<8|i2c_smbus_read_byte_data(oled_fd, 0x3E); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
       AcZ= i2c_smbus_read_byte_data(oled_fd, 0x3F)<<8|i2c_smbus_read_byte_data(oled_fd, 0x40); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
       Tmp= i2c_smbus_read_byte_data(oled_fd, 0x41)<<8|i2c_smbus_read_byte_data(oled_fd, 0x42); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
       GyX= i2c_smbus_read_byte_data(oled_fd, 0x43)<<8|i2c_smbus_read_byte_data(oled_fd, 0x44); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
       GyY= i2c_smbus_read_byte_data(oled_fd, 0x45)<<8|i2c_smbus_read_byte_data(oled_fd, 0x46); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
       GyZ= i2c_smbus_read_byte_data(oled_fd, 0x47)<<8|i2c_smbus_read_byte_data(oled_fd, 0x48); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)



       AcX_corr = (float)AcX/131;
       AcY_corr = (float)AcY/131;
       AcZ_corr = (float)AcZ/131;

       
       GyX_corr = (float)GyX/16384.0;
       GyY_corr = (float)GyY/16384.0;
       GyZ_corr = (float)GyZ/16384.0;


       // Temperature sensor from -40 to +85 degrees Celsius : 340 per degrees, -512 at 35 degrees.
       // Regarding the equation : y = ax + b with y for raw data, x for degrees, a is correction/degree and b is offset
       // Initial data : b = yo - a.xo = -512 - 35.340
       // Reversed equation to convert RAW to Degree is : x = ( y - b ) / a       =>       x = (y - yo)/a  + xo        
       
       Tmp_corrected = ((float)Tmp+512)/340 + 35; 
       

       printf("AcX =  %.3f \n",AcX_corr);
       printf("AcY =  %.3f \n",AcY_corr);
       printf("AcZ =  %.3f \n",AcZ_corr);
       printf("Tmp =  %.2f \n",Tmp_corrected);
       printf("GyX =  %.3f \n",GyX_corr);
       printf("GyY =  %.3f \n",GyY_corr);
       printf("GyZ =  %.3f \n",GyZ_corr);


               sleep(1);
    }

return 0;
}

