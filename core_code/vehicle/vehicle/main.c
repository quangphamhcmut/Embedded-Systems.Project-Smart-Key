/*
 * main.c
 *
 * Created: 1/14/2025 8:29:33 PM
 *  Author: Admin
 */ 

#define F_CPU 8000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/delay.h>
#include <stdbool.h>
#include "nrf24l01-mnemonics.h"
#include "QoL.h"
#include "nrf24l01p.h"
#include "spi.h"


#define LED 2
#define LED_PORT PORTD

void flashled(void) {
	LED_PORT = LED_PORT | (1<<LED);
	_delay_ms(500);
	LED_PORT = LED_PORT & ~(1<<LED);
	_delay_ms(500);
	LED_PORT = LED_PORT | (1<<LED);
	_delay_ms(500);
	LED_PORT = LED_PORT & ~(1<<LED);
}




int main(void)
{
	uint8_t message;
	cli();
	Init_SPI();
	DDRD |= 1<<LED; 
	Init_RF();
	nrf24_state(POWERUP);
	nrf24_start_listening();
	while(1) 
	{
		_delay_ms(100);
		message = nrf24_read_message();
		if (message==0xA5) {
			flashled();
			message = 0;
		}
		
	}
}
