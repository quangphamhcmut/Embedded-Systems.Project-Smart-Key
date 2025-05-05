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
#include <avr/eeprom.h>
#include "nrf24l01-mnemonics.h"
#include "QoL.h"
#include "nrf24l01p.h"
#include "spi.h"
#include "button.h"

//define state
#define LOCKED 0
#define UNLOCK 1
#define ENGINE_ON 2

//define command
#define LOCATE_VEHICLE 0xFE //default
#define HANDSHAKE_VEHICLE 0xFF //default
#define LOCK_VEHICLE 0xB6  //debug only
#define UNLOCK_VEHICLE 0xC6  //debug only


//testing message
#define UNLOCK_CODE 0xE6
#define LOCK_CODE 0xF6

//Timeout parameter
#define HANDSHAKE_TIMEOUT 3
#define MESSAGE_TIMEOUT 200

//Timer for handshake
#define ISR_CYCLE 6


volatile uint8_t STATE;
uint8_t send_flag;
uint8_t handshake_timer;
uint8_t locate_button;
uint8_t lock_button;

uint8_t lock_code;
uint8_t *code_address;

void watchdogSetup(void){
	wdt_reset();
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR = (1<<WDIE) | (0<<WDE) | (1<<WDP2) | (1<<WDP0) | (1<<WDP0);  // 0.5s interrupt, no system reset
}
	
void Change_Code (uint8_t *code, uint8_t **address)
{
	*address = *address + 1;
	*code = eeprom_read_byte(*address);
}

void Clear_Port (void)
{
	DDRC = 0;
	DDRD = 0;
	PORTC = 0;
	PORTD = 0;
	return;
}

ISR(WDT_vect,ISR_BLOCK) //Happen every 0.5s
{
	Reopen_Button_Port(&locate_button);
	Reopen_Button_Port(&lock_button);
	power_spi_enable();
	Init_RF_Port();
	Init_SPI();
	volatile uint8_t message = 0;
	uint16_t message_countdown;
	if (STATE == LOCKED) {
		if (SamplingButton(&lock_button)) {
			nrf24_state(POWERUP);
			nrf24_send_message (lock_code);
			nrf24_start_listening();
			message_countdown = MESSAGE_TIMEOUT;
			while (message_countdown > 1)
			{
				message_countdown = message_countdown - 1 ;
				if (nrf24_RX_data_available())
				{
					message_countdown = 0;
				}
				_delay_us(100);
			}
			if (message_countdown == 0)
			{
				message = nrf24_read_message();
				if (message == HANDSHAKE_VEHICLE)
				{
					STATE = UNLOCK;
					Change_Code(&lock_code, &code_address);
					handshake_timer = ISR_CYCLE;
					send_flag = HANDSHAKE_TIMEOUT;
				}
			}
			nrf24_state(POWERDOWN);
		}
		else if (SamplingButton(&locate_button)) 
		{
			nrf24_state(POWERUP);
			nrf24_send_message (LOCATE_VEHICLE);
			nrf24_state(POWERDOWN);
		}
	}
	else if (STATE == UNLOCK) {
		if (SamplingButton(&lock_button))
		{
			nrf24_state(POWERUP);
			nrf24_send_message (lock_code);
			nrf24_start_listening();
			message_countdown = MESSAGE_TIMEOUT;
			while (message_countdown > 1)
			{
				message_countdown = message_countdown - 1 ;
				if (nrf24_RX_data_available())
				{
					message_countdown = 0;
				}
				_delay_us(100);
			}
			if (message_countdown == 0)
			{
				message = nrf24_read_message();
				if (message == HANDSHAKE_VEHICLE)
				{
					STATE = LOCKED;
					Change_Code(&lock_code, &code_address);
				}
			}
			nrf24_state(POWERDOWN);
		}
		else if (handshake_timer == 1) 
		{
			nrf24_state(POWERUP);
			nrf24_send_message (HANDSHAKE_VEHICLE); 
			nrf24_start_listening();
			message_countdown = MESSAGE_TIMEOUT;
			while (message_countdown > 1) 
			{
				message_countdown = message_countdown - 1 ;
				if (nrf24_RX_data_available())
				{
					message_countdown = 0;
				}
				_delay_us(100);
			}
			if (message_countdown == 0)
			{
				
				message = nrf24_read_message();
				nrf24_state(POWERDOWN);
				if (message == HANDSHAKE_VEHICLE)
				{
					send_flag = HANDSHAKE_TIMEOUT;
					handshake_timer = ISR_CYCLE;
				}
				else
				{
					send_flag = send_flag - 1;
					handshake_timer = 1;
					if (send_flag <= 0)
					{
						STATE = LOCKED;
						Change_Code(&lock_code, &code_address);
					}
				}
			}
			
			else
			{
				nrf24_state(POWERDOWN);
				send_flag = send_flag - 1;
				handshake_timer = 1;
				if (send_flag <= 0)
				{
					STATE = LOCKED;
					Change_Code(&lock_code, &code_address);
				}
			}
		}
		else handshake_timer = handshake_timer - 1;
	}
	Clear_Port();
}

int main(void)
{
	cli();
	STATE = LOCKED;
	locate_button = Init_Button(2);
	lock_button = Init_Button(3);
	Init_SPI();
	Init_RF();
	lock_code = 0;
	code_address = 0;
	Change_Code(&lock_code, &code_address);
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
