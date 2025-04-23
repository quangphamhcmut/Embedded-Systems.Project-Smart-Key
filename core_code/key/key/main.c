/*
 * main.c
 *
 * Created: 1/14/2025 8:29:33 PM
 *  Author: Admin
 */ 

#define F_CPU 1000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <util/delay.h>
#include <stdbool.h>
#include "nrf24l01-mnemonics.h"
#include "QoL.h"
#include "nrf24l01p.h"
#include "spi.h"
#include "button.h"

volatile uint8_t rx_data;	
volatile uint8_t tx_data;
uint8_t button1;

void watchdogSetup(void){
	wdt_reset();
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR = (1<<WDIE) | (0<<WDE) | (1<<WDP2) | (1<<WDP0) | (1<<WDP0);  // 0.5s interrupt, no system reset
}
	

void Clear_Port (void)
{
	DDRB = 0;
	DDRC = 0;
	DDRD = 0;
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;
	return;
}

ISR(WDT_vect,ISR_BLOCK)
{
	Reopen_Button_Port(&button1);
	power_spi_enable();
	Init_RF_Port();
	Init_SPI();
	if (SamplingButton(&button1)) {
		nrf24_state(POWERUP);
		nrf24_send_message (0xA5);
		nrf24_state(POWERDOWN);
	}
	Clear_Port();
}

int main(void)
{
	cli();
	button1 = Init_Button(2);
	Init_SPI();
	Init_RF();
	watchdogSetup();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    while(1)
    {
		cli();
		power_all_disable ();
        sleep_enable();
		sleep_bod_disable();
		sei();
        sleep_cpu();
        sleep_disable();
    }
}
