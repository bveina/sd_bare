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
#include <stdlib.h>

void initSPI();
void initTimer();
void initUART0_57600();
static int uart_putchar(char c, FILE * stream);


static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,
_FDEV_SETUP_WRITE);
char line[64];

int main(void)
{
	static const char* const fst[] = {"", "FAT12", "FAT16", "FAT32", "exFAT"};
	FATFS *fs;
	fs = malloc(sizeof(FATFS));
	FRESULT fr;
	DSTATUS t;
	DWORD p2;
	
	initSPI();
	initTimer();
	initUART0_57600();
	stdout = &mystdout;
	
	sei();
	
	printf("hello World\n");
		
	t=f_mount(fs, "", 0);
	printf_P(PSTR("mount:%d\n"),t);
	
	fr = f_getfree("", &p2, &fs);
	
	printf_P(PSTR("FAT type = %s\nBytes/Cluster = %lu\nNumber of FATs = %u\n"
	"Root DIR entries = %u\nSectors/FAT = %lu\nNumber of clusters = %lu\n"
	"Volume start (lba) = %lu\nFAT start (lba) = %lu\nDIR start (lba,clustor) = %lu\nData start (lba) = %lu\n\n"),
	fst[fs->fs_type], (DWORD)fs->csize * 512, fs->n_fats,
	fs->n_rootdir, fs->fsize, fs->n_fatent - 2,
	(DWORD)fs->volbase, (DWORD)fs->fatbase, (DWORD)fs->dirbase, (DWORD)fs->database
	);
	
	
	fr = f_getlabel("", line, &p2);
	printf_P(line[0] ? PSTR("Volume name is %s\n") : PSTR("No volume label\n"), line);
	printf_P(PSTR("Volume S/N is %04X-%04X\n"), (WORD)((DWORD)p2 >> 16), (WORD)(p2 & 0xFFFF));
		
		
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