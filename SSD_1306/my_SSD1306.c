#include "my_SSD1306.h"
#include "i2c_sunxi.h"

/*SSD1306::SSD1306(int8_t SCLK, int8_t DC, int8_t RST, int8_t CS) {
  cs = CS;
  rst = RST;
  dc = DC;
  sclk = SCLK;
}

 initializer for I2C - we only indicate the reset pin!

  SSD1306::SSD1306(int8_t reset) {
  sclk = dc = cs = -1;
  rst = reset;
}
*/

void begin(uint8_t vccstate, uint8_t i2caddr) {
  _i2caddr = i2caddr;

#if defined SSD1306_128_64
    // Init sequence for 128x64 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80
    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(0x3F);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC)
      { ssd1306_command(0x10); }
    else
      { ssd1306_command(0x14); }
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x0 act like ks0108
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x12);
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    if (vccstate == SSD1306_EXTERNALVCC)
      { ssd1306_command(0x9F); }
    else
      { ssd1306_command(0xCF); }
    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC)
      { ssd1306_command(0x22); }
    else
      { ssd1306_command(0xF1); }
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
  #endif

  ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel

 
}


void ssd1306_command(char value) {
    // I2C

char buf[10];
buf[0]=value;

    uint8_t control = 0x00;   // Co = 0, D/C = 0

    //Wire.beginTransmission(_i2caddr);
    //Wire.write(control);
    //Wire.write(c);
    //Wire.endTransmission();



i2c_write(control,1,buf,_i2caddr);


}