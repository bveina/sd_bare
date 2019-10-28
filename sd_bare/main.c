/*
 * sd_bare.c
 *
 * Created: 10/28/2019 3:25:10 PM
 * Author : u_bviall
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>

#include "mmc_avr.h"

void initSPI();
void initTimer();

int main(void)
{
	initSPI();
	initTimer();

	
    while (1) 
    {
    }
}

void initSPI()
{
	DDRB |= (1<< CS) | (1<< SCK) | (1<< MOSI);
	DDRB &= ~(1<<MISO);
	PORTB |=(1<<CS);
	SPCR0 |= (1<<SPE) | (1<< MSTR) | (0b10 <<SPR0);
	SPSR0 |= 1<<SPI2X;
}

void initTimer()
{
	OCR0A = F_CPU / 1024 / 100 - 1;
	TCCR0A = _BV(WGM01);
	TCCR0B = 0b101;
	TIMSK0 = _BV(OCIE0A);
}

ISR(TIMER0_COMPA_vect)
{
	disk_timerproc();	/* Drive timer procedure of low level disk I/O module */
}
