/*
 *	LCD interface
 *	LCD: ST7565R / AQM1248
 */

#include	<htc.h>
#include	"lcd.h"
 
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

#define LCD_CS_IO LATAbits.LATA5
#define LCD_RS_IO LATAbits.LATA4
#define LCD_SDO_IO LATCbits.LATC3
#define LCD_SCK_IO LATCbits.LATC4

#define LCD_CS_TRIS TRISAbits.TRISA5
#define LCD_RS_TRIS TRISAbits.TRISA4
#define LCD_SDO_TRIS TRISCbits.TRISC3
#define LCD_SCK_TRIS TRISCbits.TRISC4

void lcd_init(void) {
//	ANSELA = 0x00; //set as digital
//	ANSELC = 0x00;
//	AD1PCFGL=0xFFFF;	// Set as digital
	LCD_RS_IO = 0;
	LCD_SDO_IO = 1;
	LCD_SCK_IO = 1;
	LCD_CS_IO = 1;
//config RA4 as output
	LCD_RS_TRIS = 0;//set as output
//config RB3 as output
	LCD_SDO_TRIS = 0;
//config RB4 as output
	LCD_SCK_TRIS = 0;
//config RB5 as output
	LCD_CS_TRIS = 0;
}

void wait2ms(void) {//approx 2 msec at 32 MHz / PIC16F1825
	unsigned int i;
	for (i=0; i<1185; i++) asm("NOP");
}

void lcd_write(unsigned char idata, unsigned char irs) {
    LCD_CS_IO = 0;
	if (irs) LCD_RS_IO = 1; else LCD_RS_IO = 0;//RS

	unsigned char i;
    for(i=0; i<8; i++) {
		asm("nop");
		LCD_SDO_IO = ((idata & 0x80) != 0);
		asm("nop");
		LCD_SCK_IO = 0;
		//asm("nop");
        LCD_SCK_IO = 1;
		asm("nop");
        idata <<= 1;
    }
    LCD_CS_IO = 1;
	return;
}

void lcd_pos(unsigned char xpos, unsigned char ypos){
    lcd_write(0xB0 | ypos, 0);
    lcd_write(0x10 | (xpos >> 4), 0);
    lcd_write(xpos & 0x0F, 0);
}

void lcd_char(char cdata, char cinv){
	int i;
	if (cdata == 0x20) cdata = 0x3a;
	cdata -= 0x30;
	if (cinv) {
		for(i=0; i<5; i++) {lcd_write(~(cchar[cdata][i]), 1);}
		lcd_write(0xff, 1);
	} else {
		for(i=0; i<5; i++) {lcd_write(cchar[cdata][i], 1);}
		lcd_write(0x00, 1);
	}
}

void lcd_char_scale(char* cx, char* cy, char cdata, char cscale){
	unsigned char uc_i, uc_j, uc_k, uc_dot, uc_out, uc_x, uc_y;
	if (cdata == 0x20) cdata = 0x3a;
	cdata -= 0x30;
	//
	uc_x = *cx; uc_y = *cy;
	for (uc_k=0; uc_k<5; uc_k++) {
		uc_dot = cchar[cdata][uc_k];
		for (uc_i=0; uc_i<cscale; uc_i++) {
			uc_out = 0;
			lcd_pos(uc_x, uc_y);
			for (uc_j=0; uc_j<8; uc_j++) {
				if ((0x01 << ((uc_i*8 + uc_j)/cscale)) & uc_dot) {
					uc_out |= (0x01 << uc_j);
				}
			}
			lcd_write(uc_out, 1);
			lcd_write(uc_out, 1);
			lcd_write(uc_out, 1);
			uc_y++;
		}
		uc_y = *cy;
		uc_x += cscale;
	}
	for (uc_i=0; uc_i<cscale; uc_i++) {
		lcd_pos(uc_x, *cy + uc_i);
		lcd_write(0, 1);
	}
	*cx += (cscale * 5 + 1);
}

void lcd_string(char* pcstr, char cinv) {
    while (*pcstr) {lcd_char(*pcstr++, cinv);}
}

void lcd_string_scale(char* cx, char* cy, char* pcstr, char cscale) {
    while (*pcstr) {lcd_char_scale(cx, cy, *pcstr++, cscale);}
}

void lcd_printHex(unsigned int iout, char idigit) {
	char buf;
	signed char i;
	for (i=3; i>=0; i--) {
		buf = (iout >> (i * 4)) & 0x000f;
		if (buf < 10) buf += 0x30; else buf += 0x37;
		if (i < idigit) lcd_char(buf, 0);
	}
}

void lcd_printDec2(unsigned char cout, char cinv) {
	//char 8 bit ==> 2 decimal digits
	char buf0, buf1;
	buf0 = cout / 10;
	buf1 = cout - buf0 * 10;
	lcd_char(buf0 + 0x30, cinv);
	lcd_char(buf1 + 0x30, cinv);
}

void lcd_printDec3(unsigned char cout) {
	//char 8 bit ==> 3 decimal digits
	char buf0, buf1, buf2;
	buf0 = cout / 100; 
	buf1 = cout/10 - buf0 * 10; 
	buf2 = cout - buf1 * 10 - buf0 * 100;
	buf0 += 0x30; if (buf0 == 0x30) buf0 = 0x3a;
	buf1 += 0x30; if ((buf0 == 0x3a)&&(buf1 == 0x30)) buf1 = 0x3a;
	lcd_char(buf0, 0);
	lcd_char(buf1, 0);
	lcd_char(buf2 + 0x30, 0);
}

void lcd_printDec3scale(char* cx, char* cy, unsigned char cout, unsigned char cscale) {
	//char 8 bit ==> 3 decimal digits
	char buf0, buf1, buf2;
	buf0 = cout / 100; 
	buf1 = cout/10 - buf0 * 10; 
	buf2 = cout - buf1 * 10 - buf0 * 100;
	buf0 += 0x30; if (buf0 == 0x30) buf0 = 0x3a;
	buf1 += 0x30; if ((buf0 == 0x3a)&&(buf1 == 0x30)) buf1 = 0x3a;
	lcd_char_scale(cx, cy, buf0, cscale);
	lcd_char_scale(cx, cy, buf1, cscale);
	lcd_char_scale(cx, cy, buf2 + 0x30, cscale);
}

void lcd_printDec5(unsigned int iout, int* pidigit, char csel) {
	//char 8 bit ==> 5 decimal digits
	int i;
	int idiv = 10000;
	int iomit = 1;
	*pidigit = 0;
	for (i=0; i<5; i++) {
		char buf0 = iout / idiv;
		char cinv  = (csel == i) ? 1 : 0;
		if (iomit && (i != 4)) {
			if (buf0) {lcd_char(buf0 + 0x30, cinv); iomit = 0; (*pidigit)++;}
			else lcd_char(' ', cinv);
		} else {
			lcd_char(buf0 + 0x30, cinv);
			(*pidigit)++;
		}
		if (i==4) break;
		iout = iout - buf0 * idiv;
		idiv /= 10;
	}
}

void lcd_clear(void){
	unsigned char i,j;
	wait2ms();
	for(j=0; j<8; j++) {
		lcd_pos(0, j);
		for(i=0; i<128; i++) {
			lcd_write(0, 1); 
			//wait(100); //uses much memories
			//wait2ms();
		}
	}
}


