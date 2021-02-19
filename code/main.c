/* Pulse oximeter 
 * Sensor: MAX30102
 * MCU: PIC16F1823
 * OLED: SSD1306
 * LCD: ST7565R / AQM1248
 * 2021 (C) Mizutani Lab
 */

#include <htc.h>
#include "lcd.h"
#include "i2c.h"
#include "oled.h"

//#define DISP_LCD
#define DISP_OLED

#define MAX30102_INTR_STAT1 0x00
#define MAX30102_INTR_STAT2 0x01
#define MAX30102_INTR_EN1 0x02
#define MAX30102_INTR_EN2 0x03
#define MAX30102_FIFO_WR_PTR 0x04
#define MAX30102_OVF_COUNTER 0x05
#define MAX30102_FIFO_RD_PTR 0x06
#define MAX30102_FIFO_DATA 0x07
#define MAX30102_FIFO_CONFIG 0x08
#define MAX30102_MODE_CONFIG 0x09
#define MAX30102_SPO2_CONFIG 0x0A
#define MAX30102_LED1_PA 0x0C
#define MAX30102_LED2_PA 0x0D
//#define MAX30105_PILOT_PA 0x10
#define MAX30102_MULTI_LED_CTRL1 0x11
#define MAX30102_MULTI_LED_CTRL2 0x12
#define MAX30102_TEMP_INT 0x1F
#define MAX30102_TEMP_FRAC 0x20
#define MAX30102_TEMP_CONFIG 0x21
//#define MAX30105_PROX_INT_THRESH 0x30
#define MAX30102_REV_ID 0xFE
#define MAX30102_PART_ID 0xFF

#define MAX30102_ID 0x15

#define MAX30102_I2C_ADDR 0xAE

char MAX30102getid() {
	return i2c_recv_byte(MAX30102_I2C_ADDR, MAX30102_PART_ID);
}

void MAX30102starttemp(void) {
	i2c_send_byte(MAX30102_I2C_ADDR, MAX30102_INTR_EN2,0x02); //enable temp interrupt
	i2c_send_byte(MAX30102_I2C_ADDR, MAX30102_TEMP_CONFIG,0x01); //start temp
}

void MAX30102init() {
	unsigned char uc_i;
	if (i2c_recv_byte(MAX30102_I2C_ADDR, MAX30102_PART_ID)!= MAX30102_ID) return;
	i2c_send_byte(MAX30102_I2C_ADDR, MAX30102_MODE_CONFIG,0x40); //reset
	for (uc_i=0; uc_i<255; uc_i++) wait2ms();
	i2c_send_byte(MAX30102_I2C_ADDR, MAX30102_FIFO_CONFIG,0x40); //sample avg=4, fifo rollover=false, fifo almost full=32
	i2c_send_byte(MAX30102_I2C_ADDR, MAX30102_MODE_CONFIG,0x03); //0x02 for Red only, 0x03 for SpO2 mode 0x07 for multimode LED
	i2c_send_byte(MAX30102_I2C_ADDR, MAX30102_SPO2_CONFIG,0x27); //SPO2_ADC=4096nA, SPO2 sample rate=100Hz, pulseWidth=411us
	i2c_send_byte(MAX30102_I2C_ADDR, MAX30102_LED1_PA,0x1F); //LED1(IR) 6.2 mA
	i2c_send_byte(MAX30102_I2C_ADDR, MAX30102_LED2_PA,0x1F); //LED2(red) 6.2 mA
	//MAX30102starttemp();
}

unsigned char MAX30102read(unsigned char* pcdata) {
	unsigned char ucDataReady = i2c_recv_byte(MAX30102_I2C_ADDR, MAX30102_FIFO_WR_PTR) - i2c_recv_byte(MAX30102_I2C_ADDR, MAX30102_FIFO_RD_PTR);
	if (ucDataReady) {
		i2c_recv_burst(MAX30102_I2C_ADDR, MAX30102_FIFO_DATA, pcdata, 6);
		pcdata[0] &= 0x03;
		pcdata[3] &= 0x03;
	} 
	return (ucDataReady);
}

unsigned char MAX30102gettemp(char* ctemp, char* ctfrac) {
	unsigned char cRdy = i2c_recv_byte(MAX30102_I2C_ADDR, MAX30102_INTR_STAT2) & 0x02;
	if (cRdy) {
		*ctemp = i2c_recv_byte(MAX30102_I2C_ADDR, MAX30102_TEMP_INT);
		*ctfrac = i2c_recv_byte(MAX30102_I2C_ADDR, MAX30102_TEMP_FRAC);
	}
	return cRdy;
}

#ifdef DISP_LCD
	#define DISP_INIT lcd_init
	#define DISP_CLEAR lcd_clear
	#define DISP_POS lcd_pos
	#define DISP_CHAR lcd_char
	#define DISP_WRITE lcd_write
	#define DISP_CHAR_SCALE lcd_char_scale
#endif
#ifdef DISP_OLED
	#define DISP_INIT oled_init
	#define DISP_CLEAR oled_clear
	#define DISP_POS oled_pos
	#define DISP_CHAR oled_char
	#define DISP_WRITE oled_write
	#define DISP_CHAR_SCALE oled_char_scale
#endif

void disp_string(char* pcstr, char cinv) {
    while (*pcstr) {DISP_CHAR(*pcstr++, cinv);}
}

void disp_string_scale(char* cx, char* cy, char* pcstr, char cscale) {
    while (*pcstr) {DISP_CHAR_SCALE(cx, cy, *pcstr++, cscale);}
}

void disp_hex(unsigned int iout, char idigit) {
	char buf;
	signed char i;
	for (i=3; i>=0; i--) {
		buf = (iout >> (i * 4)) & 0x000f;
		if (buf < 10) buf += 0x30; else buf += 0x37;
		if (i < idigit) DISP_CHAR(buf, 0);
	}
}

void disp_dec2(unsigned char cout, char cinv) {
	//char 8 bit ==> 2 decimal digits
	char buf0, buf1;
	buf0 = cout / 10;
	buf1 = cout - buf0 * 10;
	DISP_CHAR(buf0 + 0x30, cinv);
	DISP_CHAR(buf1 + 0x30, cinv);
}

void disp_dec3(unsigned char cout) {
	//char 8 bit ==> 3 decimal digits
	char buf0, buf1, buf2;
	buf0 = cout / 100; 
	buf1 = cout/10 - buf0 * 10; 
	buf2 = cout - buf1 * 10 - buf0 * 100;
	buf0 += 0x30; if (buf0 == 0x30) buf0 = 0x3a;
	buf1 += 0x30; if ((buf0 == 0x3a)&&(buf1 == 0x30)) buf1 = 0x3a;
	DISP_CHAR(buf0, 0);
	DISP_CHAR(buf1, 0);
	DISP_CHAR(buf2 + 0x30, 0);
}

void disp_dec2scale(char* cx, char* cy, unsigned char cout, unsigned char cscale) {
	//char 8 bit ==> 2 decimal digits
	char buf0, buf1;
	buf0 = cout / 10;
	buf1 = cout - buf0 * 10;
	DISP_CHAR_SCALE(cx, cy, buf0 + 0x30, cscale);
	DISP_CHAR_SCALE(cx, cy, buf1 + 0x30, cscale);
}


void disp_dec3scale(char* cx, char* cy, unsigned char cout, unsigned char cscale) {
	//char 8 bit ==> 3 decimal digits
	char buf0, buf1, buf2;
	buf0 = cout / 100; 
	buf1 = cout/10 - buf0 * 10; 
	buf2 = cout - buf1 * 10 - buf0 * 100;
	buf0 += 0x30; if (buf0 == 0x30) buf0 = 0x3a;
	buf1 += 0x30; if ((buf0 == 0x3a)&&(buf1 == 0x30)) buf1 = 0x3a;
	DISP_CHAR_SCALE(cx, cy, buf0, cscale);
	DISP_CHAR_SCALE(cx, cy, buf1, cscale);
	DISP_CHAR_SCALE(cx, cy, buf2 + 0x30, cscale);
}

void disp_dec5(unsigned int iout, int* pidigit, char csel) {
	//char 8 bit ==> 5 decimal digits
	int i;
	int idiv = 10000;
	int iomit = 1;
	*pidigit = 0;
	for (i=0; i<5; i++) {
		char buf0 = iout / idiv;
		char cinv  = (csel == i) ? 1 : 0;
		if (iomit && (i != 4)) {
			if (buf0) {DISP_CHAR(buf0 + 0x30, cinv); iomit = 0; (*pidigit)++;}
			else DISP_CHAR(' ', cinv);
		} else {
			DISP_CHAR(buf0 + 0x30, cinv);
			(*pidigit)++;
		}
		if (i==4) break;
		iout = iout - buf0 * idiv;
		idiv /= 10;
	}
}

void init(void){
	////ANSELs
	ANSELA = 0x00; //set as digital
	ANSELC = 0x00;
	////Weak pull ups
	WPUC = 0x00;//disable WPUC<5:0>
	WPUA = 0x00;//disable WPUA
	nWPUEN = 0;//enable indiv WPU bits
	////CPU clock oscillator
	//SCS1 = 1; SCS0 = 0; //SCS<1:0> = 1x (use internal clock)
	//set config bits FOSC<2:0> = 100 (use internal clock)
	//IRCF3 = 0; IRCF2 = 1; IRCF1 = 1; //default: IRCF<3:0> = 0111 (500 kHz)
	//IRCF3 = 1; IRCF2 = 1; IRCF1 = 1; IRCF0 = 1; // IRCF<3:0> = 1111 (16 MHz)
	SCS1 = 0; SCS0 = 0; //SCS<1:0> = 00 for PLL
	IRCF3 = 1; IRCF2 = 1; IRCF1 = 1; IRCF0 = 0; // IRCF<3:0> = 1110 (8 MHz)
	SPLLEN = 1;//enable 4x PLL => 8 MHx x 4
	////CCP1 (beep)
	TRISC5 = 0;//enable RC5/P1A output
//	TRISC4 = 0;//enable RC4/P1B output
	CCPTMRSbits.C1TSEL1 = 0; //use timer2 for CCP1
	CCPTMRSbits.C1TSEL0 = 0;
//	PR2 = 0x3D;//2.05 kHz
//	CCPR1L = 0x1F;//2.05 kHz, 50.0%duty
	PR2 = 0x19;//5.0 kHz
	CCPR1L = 0x0C;//5.0 kHz, 50.0%duty
	T2CONbits.T2CKPS1 = 1;//prescale 1/64
	T2CONbits.T2CKPS0 = 1;
	T2CONbits.TMR2ON = 1;
	P1M1 = 0;//PWM single output
	P1M0 = 0;
	CCP1M3 = 1;//enable CCP1 module in PWM mode
	CCP1M2 = 1;
	CCP1M1 = 0;
	CCP1M0 = 0;
	STR1A = 0;//output P1A (beep)
	STR1B = 0;//disable P1B
	STR1C = 0;//disable P1C
	STR1D = 0;//disable P1D
	//timer1 to stop beep
	T1GCONbits.TMR1GE = 0;//disable timer1 gating
	T1CONbits.TMR1ON = 0;//disable timer1
	T1CONbits.TMR1CS0 = 0;//timer1 clock = Fosc/4
	T1CONbits.TMR1CS1 = 0;
	T1CONbits.T1CKPS0 = 1;//timer1 1/8 prescaler
	T1CONbits.T1CKPS1 = 1;
	TMR1H = 0;//reset timer1
	TMR1L = 0;
	PIE1bits.TMR1IE = 1;//enable timer1 interrupt
	INTCONbits.PEIE = 1;//enable peripheral interrupt
	//MAX30102
	i2c_init();
	MAX30102init();
	//display
	DISP_INIT();
	DISP_CLEAR();
	//
//	oled_init();
//	oled_clear();
//	oled_pos(0, 0);
//	oled_char('A', 0);
//	oled_pos(20, 1);
//	oled_char('B', 0);
//	oled_pos(70, 3);
//	oled_char('C', 0);
//	unsigned char uc_x, uc_y;
//	uc_x = 80; uc_y = 0;
//	oled_char_scale(&uc_x, &uc_y, 'D', 3);
}

void interrupt isr(void){
	if (PIR1bits.TMR1IF) {
		PIR1bits.TMR1IF = 0;
		T1CONbits.TMR1ON = 0;
		STR1A = 0;
	}
}

unsigned short uc_prevwave;
void plotWave(unsigned char ucStart, unsigned char uc_wave) {
	unsigned char uc_y;
	unsigned char uc_dot;
	unsigned char uc_disp, uc_pos;
	unsigned char uc1, uc2;
	if (ucStart == 0) uc_prevwave = uc_wave;
	if (uc_wave > uc_prevwave) {uc1 = uc_wave; uc2 = uc_prevwave+1;}
	else if (uc_wave < uc_prevwave) {uc2 = uc_wave; uc1 = uc_prevwave-1;}
	else {uc2 = uc_wave; uc1 = uc_prevwave;}
	for (uc_y=0; uc_y<6; uc_y++) {
		uc_disp = 0;
		for (uc_dot=0; uc_dot<8; uc_dot++) {
			uc_pos = (5-uc_y)*8 + (7-uc_dot);
			if ((uc_pos >= uc2)&&(uc_pos <= uc1)) uc_disp |= (0x1 << uc_dot);
		}
		DISP_POS(64+ucStart, uc_y);
		DISP_WRITE(uc_disp, 1);
	}
	uc_prevwave = uc_wave;
}

#define SIG_ASCENT 1
#define SIG_DESCENT 2
#define SIG_NONE 0
#define DIM_SIG 2
#define WAVE_HEIGHT 48
#define WAVE_WIDTH 64
#define WAVE_MARGIN 8
#define MIN_SIG 0x04000
short long sigIR[DIM_SIG];
#define DIM_STORE 10
float fR[DIM_STORE];
unsigned int uiBeat[DIM_STORE];
void main(void){
	unsigned char uc_i, uc_j;
	unsigned char pcdata[6];
	short long sl_wave;
	unsigned char uc_wave;
	unsigned char uc_sig;
	unsigned int ui_time;
	short long slDiff;
	unsigned char uc_ascent;
	unsigned char uc_descent;
	short long slSig1;
	short long slIRmax;
	short long slIRmin;
	short long slSig2;
	short long slRedmax;
	short long slRedmin;
	unsigned long ulIR;
	unsigned long ulRed;
	unsigned char ucSum;
	short long slWaveMax;
	short long slWaveMin;
	float fIR;
	float fRed;
	unsigned int uiStore;
	unsigned char uc_prevsig;
	short long slSigBase;
	short long slSigScale;
	unsigned char ucCleared;

	for (uc_i=0; uc_i<DIM_SIG; uc_i++) {sigIR[uc_i] = 0;}
	for (uc_i=0; uc_i<DIM_STORE; uc_i++) {fR[uc_i] = 0; uiBeat[uc_i] = 0;}
	slIRmax = 0;
	slIRmin = 0x7fffff;
	slRedmax = 0;
	slRedmin = 0x7fffff;
	slWaveMax = 0;
	slWaveMin = 0x7fffff;
	ulIR = 0;
	ulRed = 0;
	ucSum = 0;
	uiStore = 0;
	slSigBase = 0x18000;
	slSigScale = 64;

	init();
//	LED blink
//	TRISCbits.TRISC2 = 0;
//	while (1) {
//		LATCbits.LATC2 = 1;
//		for (uc_i=0; uc_i<255; uc_i++) wait2ms();
//		LATCbits.LATC2 = 0;
//		for (uc_i=0; uc_i<255; uc_i++) wait2ms();
//	}
	unsigned char uc_x, uc_y;
	ui_time = 0;
	uc_sig = SIG_NONE;
	uc_prevsig = SIG_NONE;
	uc_x = 0; uc_y = 0;
	DISP_CHAR_SCALE(&uc_x, &uc_y, 0x3B, 3);
	uc_x = 0; uc_y = 3;
	DISP_CHAR_SCALE(&uc_x, &uc_y, 'O', 3);
	DISP_POS(10,5);
	DISP_CHAR(' ', 0);
	DISP_POS(11,5);
	DISP_CHAR('2', 0);
	uc_x = 0; uc_y = 6;
	disp_string_scale(&uc_x, &uc_y, "TEMP", 2);
	DISP_POS(100,6);
	DISP_CHAR('O', 0);
	uc_x = 106; uc_y = 6;
	DISP_CHAR_SCALE(&uc_x, &uc_y, 'C', 2);
	ucCleared = 1;
	MAX30102starttemp();
	while (1) {
		//data acquiisition
		uc_i = MAX30102read(pcdata);
		//DISP_POS(0,0);
		//disp_dec2(uc_i, 0);
		if (!uc_i) continue;
		//if (!MAX30102read(pcdata)) continue;
		asm("CLRWDT");
		//disp raw read
		//DISP_POS(0,6);
		//for (uc_i=0; uc_i<3; uc_i++) disp_hex(pcdata[uc_i], 2);//IR
		//DISP_POS(0,7);
		//for (uc_i=0; uc_i<3; uc_i++) disp_hex(pcdata[uc_i+3], 2);//Red
		slSig1 = (((short long)pcdata[0]) << 16) | (((short long)pcdata[1]) << 8) | pcdata[2];
		slSig2 = (((short long)pcdata[3]) << 16) | (((short long)pcdata[4]) << 8) | pcdata[5];
		//disp wave form
		sl_wave = (slSig1 - slSigBase) / slSigScale + WAVE_MARGIN;
		if (sl_wave < 0) uc_wave = 0; else if (sl_wave >= WAVE_HEIGHT) uc_wave = WAVE_HEIGHT-1; else uc_wave = sl_wave;
		uc_j = ui_time % WAVE_WIDTH;
		plotWave(uc_j, uc_wave);
		if (slSig1 > slWaveMax) slWaveMax = slSig1;
		if (slSig1 < slWaveMin) slWaveMin = slSig1;
		//detect peaks
		if ((slSig1 > MIN_SIG)&&(slSig2 > MIN_SIG)) {
			uc_prevsig = uc_sig;
			uc_ascent = 0;
			uc_descent = 0;
			for (uc_i=0; uc_i<DIM_SIG; uc_i++) {
				if (uc_i) slDiff = sigIR[(ui_time - uc_i) % DIM_SIG] - sigIR[(ui_time - uc_i-1) % DIM_SIG];
				else slDiff = slSig1 - sigIR[(ui_time - 1) % DIM_SIG];
				if (slDiff > 10) uc_ascent++;
				else if (slDiff < -30) uc_descent++;
			}
			if (uc_ascent == DIM_SIG) uc_sig = SIG_ASCENT;
			else if (uc_descent == DIM_SIG) uc_sig = SIG_DESCENT;
			ulIR += slSig1;
			if (slSig1 > slIRmax) slIRmax = slSig1;
			if (slSig1 < slIRmin) slIRmin = slSig1;
			ulRed += slSig2;
			if (slSig2 > slRedmax) slRedmax = slSig2;
			if (slSig2 < slRedmin) slRedmin = slSig2;
			ucSum++;
		} else {
			uiStore = 0;
			if (!ucCleared) {
				uc_x = 16; uc_y = 0;
				disp_string_scale(&uc_x, &uc_y, "   ", 3);
				uc_x = 16; uc_y = 3;
				disp_string_scale(&uc_x, &uc_y, "   ", 3);
				ucCleared = 1;
			}
			uc_prevsig = SIG_NONE;
		}
		sigIR[(ui_time) % DIM_SIG] = slSig1;
		//calc heart rate and spo2
		if ((uc_sig == SIG_DESCENT)&&(uc_prevsig == SIG_ASCENT)) {
			//beep
			STR1A = 1; //output P1A (beep)
			T1CONbits.TMR1ON = 1;// timer1 on
			INTCONbits.GIE = 1;//enable interrupt
			//blink heart icon
			uc_x = 0; uc_y = 0;
			DISP_CHAR_SCALE(&uc_x, &uc_y, 0x3A, 3);
			//calc beta
			fIR = ulIR / ucSum;
			fIR = (slIRmax - slIRmin) / fIR;
			fRed = ulRed / ucSum;
			fRed = (slRedmax - slRedmin) / fRed;
			fR[uiStore % DIM_STORE] = fRed / fIR;
			uiBeat[uiStore % DIM_STORE] = ui_time;
			uiStore++;
			if (uiStore >= DIM_STORE+5) {
				ucSum = (unsigned char)(60. * DIM_STORE / (0.05 * ((uiBeat[(uiStore-1)%DIM_STORE] - uiBeat[(uiStore-DIM_STORE)%DIM_STORE]) % 65536)));
				uc_x = 16; uc_y = 0;
				disp_dec3scale(&uc_x, &uc_y, ucSum, 3);
				fRed = 0;
				for (uc_i=0; uc_i<DIM_STORE; uc_i++) {fRed += fR[uc_i];}
				fRed /= DIM_STORE;
				//ucSum = (unsigned char)(-33.437 * fRed + 114.9);//linear approx of Reynolds et al. (1991) Br J Anaesthesia 67, 638-643.
				//ucSum = (unsigned char)(-17. * fRed + 104.);//Recommended Configurations and Operating Profiles for MAX30101/MAX30102 EV Kits
				ucSum = (unsigned char)(1.5958422 * fRed * fRed -34.6596622 * fRed + 112.6898759);//Guidelines for SpO2 Measurement Using the MaximR MAX32664 Sensor Hub
				//ucSum = (unsigned char)(-45.060 * fRed * fRed + 30.354 * fRed + 94.845);// https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library/blob/master/src/spo2_algorithm.cpp
				uc_x = 16; uc_y = 3;
				disp_dec3scale(&uc_x, &uc_y, ucSum, 3);
				ucCleared = 0;
			} else {
				if (!ucCleared) {
					uc_x = 16; uc_y = 0;
					disp_string_scale(&uc_x, &uc_y, "   ", 3);
					uc_x = 16; uc_y = 3;
					disp_string_scale(&uc_x, &uc_y, "   ", 3);
					ucCleared = 1;
				}
			}
			//temperature
			if (MAX30102gettemp(&uc_i, &uc_j)) {
				uc_x = 50; uc_y = 6;
				disp_dec2scale(&uc_x, &uc_y, uc_i, 2);
				DISP_CHAR_SCALE(&uc_x, &uc_y, '@', 2);//period
				DISP_CHAR_SCALE(&uc_x, &uc_y, (char)(uc_j * 0.625 + 0x30), 2);
				MAX30102starttemp();
			}
			//blink heart icon
			uc_x = 0; uc_y = 0;
			DISP_CHAR_SCALE(&uc_x, &uc_y, 0x3B, 3);
			//
			ulIR = 0;
			ulRed = 0;
			ucSum = 0;
			slIRmax = 0;
			slIRmin = 0x7fffff;
			slRedmax = 0;
			slRedmin = 0x7fffff;
		}
		//update cycle
		ui_time++;
		if (ui_time % WAVE_WIDTH == 0) {
			slSigBase = slWaveMin;
			slSigScale = (slWaveMax - slWaveMin) / WAVE_HEIGHT * (WAVE_HEIGHT + WAVE_MARGIN*2) / WAVE_HEIGHT;
			slWaveMax = 0;
			slWaveMin = 0x7fffff;
		}
	}
}
