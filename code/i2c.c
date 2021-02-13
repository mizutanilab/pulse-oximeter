/*
 *	I2C interface
 *	Example Si5351A
 */

#include	<htc.h>
#include	"i2c.h"
//#include	"lcd.h"

//#define I2C_SCL_OUT LATCbits.LATC0
//#define I2C_SCL_IN PORTCbits.RC0
#define I2C_SCL_TRIS TRISCbits.TRISC0
#define I2C_SCL_PULLUP WPUCbits.WPUC0
//#define I2C_SDA_OUT LATCbits.LATC1
//#define I2C_SDA_IN PORTCbits.RC1
#define I2C_SDA_TRIS TRISCbits.TRISC1
#define I2C_SDA_PULLUP WPUCbits.WPUC1

//SI5351A address
//#define I2C_ADDR 0xC0
//MAX30102 address
#define I2C_ADDR 0xAE

void i2c_init(void) {
	//ANSELC = 0x00; //set as digital
	I2C_SCL_PULLUP = 1;
	I2C_SDA_PULLUP = 1;
	I2C_SCL_TRIS = 1;//disable output
	I2C_SDA_TRIS = 1;//disable output
	SSP1ADD = 0x4F;//Baud rate 100 kHz
	SSP1STAT = 0x00;
	SSP1CON1 = 0x28;//SSP1EN=1, SSP1M<3:0>=1000
}

void i2c_start(void) {
	SSP1CON2bits.SEN = 1;
	while(SSP1CON2bits.SEN);
}

void i2c_restart(void) {
	PIR1bits.SSP1IF = 0;
	SSP1CON2bits.RSEN = 1;
	while(SSP1CON2bits.RSEN);
}

void i2c_stop(void) {
	SSP1CON2bits.PEN = 1;
	while(SSP1CON2bits.PEN);
}

void i2c_write(unsigned char byte) {
	PIR1bits.SSP1IF = 0;
	SSP1BUF = byte;
	while(SSP1STATbits.R_nW);
	//while(!PIR1bits.SSP1IF);
}

char i2c_read(char bLastRx) {
	PIR1bits.SSP1IF = 0;
	SSP1CON2bits.RCEN = 1;
	while(SSP1CON2bits.RCEN);
	//ack sequence
	if (bLastRx) SSP1CON2bits.ACKDT = 1; else SSP1CON2bits.ACKDT = 0;
	SSP1CON2bits.ACKEN = 1; while (SSP1CON2bits.ACKEN);
	//
	return SSP1BUF;
}

void i2c_send_byte(unsigned char reg, unsigned char data) {
	i2c_start();
	i2c_write(I2C_ADDR);
	i2c_write(reg);
	i2c_write(data);
	i2c_stop();
}

void i2c_send_burst(unsigned char reg, unsigned char* pcdata, unsigned char ndata){
	i2c_start();

	i2c_write(I2C_ADDR);
	i2c_write(reg);
	unsigned char i;
	for (i=0; i<ndata; i++) {i2c_write(pcdata[i]);}

	i2c_stop();
}

unsigned char i2c_recv_byte(unsigned char reg) {
	unsigned char ucRtn;
	i2c_start();
	i2c_write(I2C_ADDR);// I2C slave address
	i2c_write(reg);// Send register address
	i2c_restart();
	i2c_write(I2C_ADDR | 0x01);// I2C slave address + read flag
	ucRtn = i2c_read(1);// receive data
	i2c_stop();
	return ucRtn;
}

void i2c_recv_burst(unsigned char reg, unsigned char* pcdata, unsigned char ndata){
	i2c_start();
	i2c_write(I2C_ADDR);//address, write
	i2c_write(reg);
	i2c_restart();

	i2c_write(I2C_ADDR | 0x01);// I2C slave address + read flag
	unsigned char i;
	for (i=0; i<ndata; i++) {pcdata[i] = i2c_read(i == ndata-1);}
	i2c_stop();
}
