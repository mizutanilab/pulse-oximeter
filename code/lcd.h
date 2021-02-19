#ifndef _LCD_H_
#define _LCD_H_

extern void wait2ms(void);
extern void lcd_init();
extern void lcd_write(unsigned char idata, unsigned char irs);
extern void lcd_char(char cdata, char cinv);
extern void lcd_char_scale(char* cx, char* cy, char cdata, char cscale);
//extern void lcd_string(char* pcstr, char cinv);
//extern void lcd_string_scale(char* cx, char* cy, char* pcstr, char cscale);
//extern void lcd_printHex(unsigned int iout, char idigit);
//extern void lcd_printDec2(unsigned char cout, char cinv);
//extern void lcd_printDec3(unsigned char cout);
//extern void lcd_printDec5(unsigned int iout, int* pidigit, char csel);
//extern void lcd_printDec3scale(char* cx, char* cy, unsigned char cout, unsigned char cscale);
extern void lcd_clear(void);
extern void lcd_pos(unsigned char xpos, unsigned char ypos);

#endif

