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
#include <avr/eeprom.h>
#include "nrf24l01-mnemonics.h"
#include "QoL.h"
#include "nrf24l01p.h"
#include "spi.h"


#define LOCATE_LED 2
#define LOCK_LED 3
#define ENGINE_LED 4
#define LED_DATA DDRD
#define LED_PORT PORTD

//define MainSWitch
#define SWITCH_DATA DDRD
#define SWITCH 6
#define SWITCH_PORT PORTD
#define SWITCH_PIN PIND

//define state
#define LOCKED 0
#define UNLOCK 1

//define command
#define LOCATE_VEHICLE 0xFE //default
#define HANDSHAKE_VEHICLE 0xFF //default
#define LOCK_VEHICLE 0xB6
#define UNLOCK_VEHICLE 0xC6

//Timeout parameter
#define HANDSHAKE_TIMEOUT 32000
#define MESSAGE_TIMEOUT 0xFFFF

uint8_t lock_code;
uint8_t *code_address;

void Change_Code (uint8_t *code, uint8_t **address)
{
	*address = *address+1;
	*code = eeprom_read_byte(*address);
}

void flashled(uint8_t lednum) {
	setbit(LED_PORT,lednum);
	_delay_ms(500);
	clearbit(LED_PORT,lednum);
	_delay_ms(500);
	setbit(LED_PORT,lednum);
	_delay_ms(500);
	clearbit(LED_PORT,lednum);
}

void turn_on_led (uint8_t lednum) {
	setbit(LED_PORT,lednum);
}

void turn_off_led (uint8_t lednum) {
	clearbit(LED_PORT,lednum);
}

void Init_MainSwitch (void) {
	clearbit(SWITCH_DATA, SWITCH);
	setbit(SWITCH_PORT, SWITCH);
}

uint8_t Sampling_MainSwitch (void) {
	if (SWITCH_PIN & 1<<SWITCH) {
		return 1;
	}
	else return 0;
}

void Init_LED (uint8_t led_num) {
	setbit(LED_DATA,led_num);
}

void Locked_state_handle (uint8_t *STATE) {
	uint8_t message;
	while (1)
	{
		if (nrf24_RX_data_available())
		{
			message = nrf24_read_message();
			if (message == LOCATE_VEHICLE)
			{
				flashled(LOCATE_LED);
			}
			else if (message == lock_code)
			{
				_delay_ms(3);
				nrf24_send_message(HANDSHAKE_VEHICLE);
				nrf24_start_listening();
				*STATE = UNLOCK;
				Change_Code(&lock_code, &code_address);
				return;
			}
			message = 0;
		}
	}
}

void Unlock_state_handle (uint8_t *STATE) {
	uint8_t message;
	uint8_t MainSwitch = 0;
	uint8_t power;
	uint8_t lock_request = 0;
	uint16_t handshake_countdown = HANDSHAKE_TIMEOUT;
	while (1) 
	{
		MainSwitch = Sampling_MainSwitch();
		if(MainSwitch == 1)
		{
			turn_on_led(ENGINE_LED);
		}
		else turn_off_led(ENGINE_LED);
		
		if (nrf24_RX_data_available())
		 {
			message = nrf24_read_message();
			power = nrf24_RX_power();
			if (power == 1) 
			{
				if (message == HANDSHAKE_VEHICLE)
				{
					_delay_ms(3);
					nrf24_send_message(HANDSHAKE_VEHICLE);
					nrf24_start_listening();
					lock_request = 0;
					handshake_countdown = HANDSHAKE_TIMEOUT;
					turn_off_led(LOCK_LED);
					_delay_ms(10);
					turn_on_led(LOCK_LED);
				}
				else if ((message == lock_code) && (MainSwitch == 0))
				{
					_delay_ms(3);
					nrf24_send_message(HANDSHAKE_VEHICLE);
					nrf24_start_listening();
					lock_request = 1;
				}
				message = 0;
			}
		}
		else
		{
			handshake_countdown = handshake_countdown - 1;
		}
		if (handshake_countdown == 0) {
			lock_request = 1;
		}
		
		if (lock_request == 1)
		{
			if(MainSwitch == 0) 
			{
				*STATE = LOCKED;
				Change_Code(&lock_code, &code_address);
				return;
			}
		}
		_delay_us(150);
	}
}
int main(void)
{
	uint8_t STATE = LOCKED;
	cli();
	Init_SPI();
	Init_MainSwitch();
	Init_LED(LOCATE_LED);
	Init_LED(LOCK_LED);
	Init_LED(ENGINE_LED);
	Init_RF();
	lock_code = 0;
	code_address = 0;
	Change_Code(&lock_code, &code_address);
	nrf24_state(POWERUP);
	nrf24_start_listening();
	while(1) 
	{
		if (STATE == LOCKED) 
		{
			turn_off_led(LOCK_LED);
			Locked_state_handle(&STATE);
		}
		else
		{
			turn_on_led(LOCK_LED);
			Unlock_state_handle(&STATE);
		}
	}
}
