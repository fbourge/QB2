#include"i2c_sunxi.h"

//////////
// Init I2Cdevice
//////////
int i2c_init(char filename[40], int addr)
        {
        int file;
 
        if ((file = open(filename,O_RDWR)) < 0)
                {
                printf("Failed to open the bus.");
                /* ERROR HANDLING; you can check errno to see what went wrong */
                com_serial=0;
                exit(1);
                }
else printf("\n %s opened ! \n",filename);
 
    if (ioctl(file,I2C_SLAVE,addr) < 0)
                {
                printf("Failed to acquire bus access and/or talk to slave.\n");
                /* ERROR HANDLING; you can check errno to see what went wrong */
                com_serial=0;
                exit(1);
                }
 else printf("\n Succed to acquire bus access ! \n\n");

        return file;
        }

 
//////////
// Set pointer address
//////////
void i2c_set_pointer(int address,int value,int file)
        {
        /*
        //printf("end: 0x%x%x\n", address,value);
        if (i2c_smbus_write_byte_data(file, address, value)<0)
                {
                fprintf(stderr, "Warning - write failed\n");
                }
        */
        char buf[10];
        buf[0]=address;
        buf[1]=value;
 
        if (write(file, buf, 2) != 2)
                {
                fprintf(stderr, "Error setting pointer\n");
                com_serial=0;
                failcount++;
                }
        else
                {
                //printf("w_0x%0*x\n", 2, buf[0]);
                //printf("w_0x%0*x\n", 2, buf[1]);
                com_serial=1;
                failcount=0;
                }
 
        }
 
//////////
// Read n bytes
//////////
 
char * i2c_read(int add1, int add2, int nbytes,int file)
        {
        int n;
 
        i2c_set_pointer(add1,add2,file);
 
        if (read(file, buf, nbytes) != nbytes)
                {
                fprintf(stderr, "Error reading %i bytes\n",nbytes);
                com_serial=0;
                failcount++;
                }
        else
                {
                for (n=0;n<nbytes;n++)
                        {
                        //printf("r_0x%0*x\n", 2, buf[n]);
                        }
                com_serial=1;
                failcount=0;
                return buf;
                }
        }
 
//////////
// Write n bytes
//////////
void i2c_write(int add1, int nbytes,char value[10],int file)
        {
        int n;
        unsigned char buf[10];
        buf[0] = add1;

        if (nbytes>=1) buf[2] = value[0];
        if (nbytes>=2) buf[3] = value[1];
        if (nbytes>=3) buf[4] = value[2];
        if (nbytes>=4) buf[5] = value[3];
 
        if (write(file, buf, nbytes) != nbytes)
                {
                fprintf(stderr, "Error writing %i bytes\n",nbytes);
                com_serial=0;
                failcount++;
                }
        else
                {
                for (n=0;n<(nbytes+2);n++)
                        {
                        //printf("w_0x%0*x\n", 2, buf[n]);
                        }
                com_serial=1;
                failcount=0;
                }
        }
