/*
 * sd_bare.c
 *
 * Created: 10/28/2019 3:25:10 PM
 * Author : u_bviall
 */ 

#include <avr/io.h>

#include "mmc_avr.h"

void initSPI();

int main(void)
{
	initSPI();
    /* Replace with your application code */
	
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
