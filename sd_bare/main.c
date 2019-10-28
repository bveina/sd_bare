/*
 * sd_bare.c
 *
 * Created: 10/28/2019 3:25:10 PM
 * Author : u_bviall
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <avr/pgmspace.h>

#include "mmc_avr.h"

void initSPI();
void initTimer();
void initUART0_57600();
static int uart_putchar(char c, FILE * stream);


static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,
_FDEV_SETUP_WRITE);

int main(void)
{
	initSPI();
	initTimer();
	initUART0_57600();
	stdout = &mystdout;
	
	sei();
	
	printf("hello World\n");
		
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

void initUART0_57600()
{
	#define UCSZ_VALUE 0b011
	UCSR0B |= (0 << RXCIE0) | (0 << TXCIE0) | (0 << UDRIE0) |
	(0 << RXEN0) | (1 << TXEN0) | ((((UCSZ_VALUE)>>2)&1) << UCSZ02);
	// UMSEL = uart, UPM = No Parity, USB = one stop bit
	UCSR0C |= (0b00 <<UMSEL00) | (0b00 << UPM00) | (0 << USBS0)	|
	((UCSZ_VALUE&0x3) << UCSZ00);
	
	#define BAUD 57600
	#include <util/setbaud.h>
	UBRR0 = UBRR_VALUE;
	//UBRR0L = UBRRL_VALUE;
	#if USE_2X
	UCSR0A |= (1<< U2X0);
	#else
	UCSR0A &= ~(1<< U2X0);
	#endif
}

static int uart_putchar(char c, FILE * stream)
{
	if (c == '\n')
	uart_putchar('\r',stream);
	
	loop_until_bit_is_set(UCSR0A,UDRE0);
	UDR0 = c;
	return 0;
}