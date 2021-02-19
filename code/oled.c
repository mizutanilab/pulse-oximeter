/*
 *	OLED interface
 *	SSD1306
 *  2021 (C) Mizutani Lab
 */

#include	<htc.h>
#include	"oled.h"
#include	"i2c.h"

const unsigned char cchar[43][5] = {
	{0x7C,0xA2,0x92,0x8A,0x7C},		// 30	0
	{0x00,0x84,0xFE,0x80,0x00},		// 31	1
	{0xE4,0x92,0x92,0x92,0x8C},		// 32	2
	{0x44,0x82,0x92,0x92,0x6C},		// 33	3
	{0x30,0x28,0x24,0xFE,0x20},		// 34	4
	{0x4E,0x8A,0x8A,0x8A,0x72},		// 35	5
	{0x78,0x94,0x92,0x92,0x60},		// 36	6
	{0x02,0xE2,0x12,0x0A,0x06},		// 37	7
	{0x6C,0x92,0x92,0x92,0x6C},		// 38	8
	{0x0C,0x92,0x92,0x52,0x3C},		// 39	9
	{0x00,0x00,0x00,0x00,0x00},		// 3A	blank(20)
//	{0xB8,0x64,0x5C,0x44,0x3A},		// 3B	phi	//instead of ";"
	{0x1C,0x7E,0xFC,0x7E,0x1C},		// 3B	heart	//instead of ";"
	{0x10,0x28,0x44,0x82,0x00},		// 3C	<
	{0x28,0x28,0x28,0x28,0x28},		// 3D	=
	{0x82,0x44,0x28,0x10,0x00},		// 3E	>
	{0x40,0x20,0x10,0x08,0x04},		// 3F	/	//instead of "?"
	{0x00,0xC0,0xC0,0x00,0x00},		// 40	.	//instaed of "@"
	{0xF8,0x24,0x22,0x24,0xF8},		// 41	A 
	{0x82,0xFE,0x92,0x92,0x6C},		// 42	B 
	{0x7C,0x82,0x82,0x82,0x44},		// 43	C 
	{0x82,0xFE,0x82,0x82,0x7C},		// 44	D 
	{0xFE,0x92,0x92,0x82,0x82},		// 45	E 
	{0xFE,0x12,0x12,0x02,0x02},		// 46	F 
	{0x7C,0x82,0x92,0x92,0x74},		// 37	G 
	{0xFE,0x10,0x10,0x10,0xFE},		// 48	H 
	{0x00,0x82,0xFE,0x82,0x00},		// 49	I
	{0x40,0x80,0x82,0x7E,0x02},		// 4A	J 
	{0xFE,0x10,0x28,0x44,0x82},		// 4B	K 
	{0xFE,0x80,0x80,0x80,0x80},		// 4C	L 
	{0xFE,0x04,0x18,0x04,0xFE},		// 4D	M 
	{0xFE,0x04,0x08,0x10,0xFE},		// 4E	N 
	{0x7C,0x82,0x82,0x82,0x7C},		// 4F	O 
	{0xFE,0x12,0x12,0x12,0x0C},		// 50	P 
	{0x7C,0x82,0xA2,0x42,0xBC},		// 51	Q
	{0xFE,0x12,0x32,0x52,0x8C},		// 52	R 
	{0x4C,0x92,0x92,0x92,0x64},		// 53	S 
	{0x02,0x02,0xFE,0x02,0x02},		// 54	T 
	{0x7E,0x80,0x80,0x80,0x7E},		// 55	U 
	{0x0E,0x30,0xC0,0x30,0x0E},		// 56	V 
	{0xFE,0x40,0x30,0x40,0xFE},		// 57	W 
	{0xC6,0x28,0x10,0x28,0xC6},		// 58	X 
	{0x06,0x08,0xF0,0x08,0x06},		// 59	Y 
	{0xC2,0xA2,0x92,0x8A,0x86}		// 5A	Z 
};

//void wait2ms(void) {//approx 2 msec at 32 MHz / PIC16F1825
//	unsigned int i;
//	for (i=0; i<1185; i++) asm("NOP");
//}

#define SSD1306_I2C_ADDR 0x78

void oled_init(void) {//see SSD1306 Application note
	i2c_start();
	i2c_write(SSD1306_I2C_ADDR);
	i2c_write(0x00);//Co=0 (continue), D/C#=0 (command)
	i2c_write(0xA8);//Sequence of SSD1306 App Note; Set MUX Ratio
	i2c_write(0x3F);
	i2c_write(0xD3);//Set Display Offset
	i2c_write(0x00);
	i2c_write(0x40);//Set Display Start Line
	i2c_write(0xA1);//upsode down
	i2c_write(0xC8);
	i2c_write(0xDA);//Set COM Pins hardware configuration
	//i2c_write(0x02);
	i2c_write(0x12);//same as reset
	i2c_write(0x81);//Set Contrast Control
	i2c_write(0x7F);
	i2c_write(0xA4);//Disable Entire Display On
	i2c_write(0xA6);//Set Normal Display
	i2c_write(0xD5);//Set Osc Frequency
	i2c_write(0x80);
	i2c_write(0x8D);//Enable charge pump regulator
	i2c_write(0x14);
	i2c_write(0xAF);//Display On
	i2c_stop();
}

void oled_clear(void) {
	unsigned int ui_i;

	i2c_start();
	i2c_write(SSD1306_I2C_ADDR);
	i2c_write(0x00);//Co=0 (continue), D/C#=0 (command)
	i2c_write(0x20);//Set Memory Addressing Mode
	i2c_write(0x00);// Horizontal Addressing Mode
	i2c_write(0x21);//Set Column Address
	i2c_write(0x00);// Column start address = 0
	i2c_write(0x7F);// Column end address = 7F
	i2c_write(0x22);//Set Page Address
	i2c_write(0x00);// Page start Address = 0
	i2c_write(0x07);// Page end Address = 7
	i2c_stop();

	i2c_start();
	i2c_write(SSD1306_I2C_ADDR);
	i2c_write(0x40);//Co=0 (continue), D/C#=1 (stored at the GDDRAM)
	for(ui_i=0; ui_i<1024; ui_i++) i2c_write(0x00);// 128 columns x 8 pages
	i2c_stop();
}

void oled_char(char cdata, char cinv){
	i2c_start();
	i2c_write(SSD1306_I2C_ADDR);
	i2c_write(0x40);//Co=0 (continue), D/C#=1 (stored at the GDDRAM)

	unsigned char i;
	if (cdata == 0x20) cdata = 0x3a;
	cdata -= 0x30;
	if (cinv) {
		for(i=0; i<5; i++) {i2c_write(~(cchar[cdata][i]));}
		i2c_write(0xff);
	} else {
		for(i=0; i<5; i++) {i2c_write(cchar[cdata][i]);}
		i2c_write(0x00);
	}
	i2c_stop();
}

void oled_pos(unsigned char xpos, unsigned char ypos){
	i2c_start();
	i2c_write(SSD1306_I2C_ADDR);
	i2c_write(0x00);//Co=0 (continue), D/C#=0 (command)
	i2c_write(0x20);//Set Memory Addressing Mode
	i2c_write(0x00);// Page Addressing Mode
	i2c_write(0xB0 | ypos);//Set GDDRAM Page Start Address

	i2c_write(0x10 | (xpos >> 4));
	i2c_write(xpos & 0x0F);
	i2c_stop();
}

void oled_write(unsigned char idata, unsigned char irs) {
	i2c_start();
	i2c_write(SSD1306_I2C_ADDR);
	if (irs) i2c_write(0x40);//Co=0 (continue), D/C#=1 (stored at the GDDRAM)
	else i2c_write(0x00);//Co=0 (continue), D/C#=1 (command)
	i2c_write(idata);
	i2c_stop();
}

void oled_char_scale(char* cx, char* cy, char cdata, char cscale){
	unsigned char uc_i, uc_j, uc_k, uc_dot, uc_out, uc_x, uc_y;
	if (cdata == 0x20) cdata = 0x3a;
	cdata -= 0x30;
	//
	uc_x = *cx; uc_y = *cy;
	for (uc_k=0; uc_k<5; uc_k++) {
		uc_dot = cchar[cdata][uc_k];
		for (uc_i=0; uc_i<cscale; uc_i++) {
			uc_out = 0;
			oled_pos(uc_x, uc_y);
			for (uc_j=0; uc_j<8; uc_j++) {
				if ((0x01 << ((uc_i*8 + uc_j)/cscale)) & uc_dot) {
					uc_out |= (0x01 << uc_j);
				}
			}
			i2c_start();
			i2c_write(SSD1306_I2C_ADDR);
			i2c_write(0x40);//Co=0 (continue), D/C#=1 (stored at the GDDRAM)
			i2c_write(uc_out);
			i2c_write(uc_out);
			i2c_write(uc_out);
			i2c_stop();
			uc_y++;
		}
		uc_y = *cy;
		uc_x += cscale;
	}
	for (uc_i=0; uc_i<cscale; uc_i++) {
		oled_pos(uc_x, *cy + uc_i);
		oled_write(0, 1);
	}
	*cx += (cscale * 5 + 1);
}
