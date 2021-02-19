#ifndef _OLED_H_
#define _OLED_H_

extern void oled_init(void);
extern void oled_clear(void);
extern void oled_char(char cdata, char cinv);
extern void oled_pos(char xpos, char ypos);
extern void oled_write(unsigned char idata, unsigned char irs);
extern void oled_char_scale(char* cx, char* cy, char cdata, char cscale);


#endif
