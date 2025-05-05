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


#define LOCATE_LED 2
#define LOCK_LED 3
#define LED_DATA DDRD
#define LED_PORT PORTD

//define MainSWitch
#define SWITCH_DATA DDRD
#define SWITCH 4
#define SWITCH_PORT PORTD
#define SWITCH_PIN PIND

//define state
#define LOCKED 0
#define UNLOCK 1
#define ENGINE_ON 2

//define command
#define LOCATE_VEHICLE 0xA6
#define LOCK_VEHICLE 0xB6
#define UNLOCK_VEHICLE 0xC6
#define HANDSHAKE_VEHICLE 0xD6
#define HANDSHAKE_KEY 0xE6 

//Timeout parameter
#define HANDSHAKE_TIMEOUT 32000
#define MESSAGE_TIMEOUT 0xFFFF

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

void Init_Switch (void) {
	clearbit(SWITCH_DATA, SWITCH);
	setbit(SWITCH_PORT, SWITCH);
}

uint8_t Sampling_Switch (void) {
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
			else if (message == UNLOCK_VEHICLE)
			{
				_delay_ms(3);
				nrf24_send_message(HANDSHAKE_VEHICLE);
				nrf24_start_listening();
				*STATE = UNLOCK;
				return;
			}
			message = 0;
		}
	}
}

void Unlock_state_handle (uint8_t *STATE) {
	uint8_t message;
	uint16_t handshake_countdown = HANDSHAKE_TIMEOUT;
	while (1) 
	{
		if (nrf24_RX_data_available())
		{
			message = nrf24_read_message();
			if (message == HANDSHAKE_KEY)
			{
				_delay_ms(3);
				nrf24_send_message(HANDSHAKE_VEHICLE);
				nrf24_start_listening();
				handshake_countdown = HANDSHAKE_TIMEOUT;
				turn_on_led(LOCK_LED);
				_delay_ms(10);
				turn_off_led(LOCK_LED);
			}
			else if (message == LOCK_VEHICLE)
			{
				_delay_ms(3);
				nrf24_send_message(HANDSHAKE_VEHICLE);
				nrf24_start_listening();
				*STATE = LOCKED;
				return;	
			}
			message = 0;
		}
		else
		{
			handshake_countdown = handshake_countdown - 1;
		}
		if (handshake_countdown == 0) {
			*STATE = LOCKED;
			return;
		}
		_delay_us(150);
	}
	*STATE = ENGINE_ON;
}

void EngineOn_state_handle(uint8_t *STATE)
{
	uint8_t message;
	uint8_t lock_request;
	uint16_t handshake_countdown = HANDSHAKE_TIMEOUT;
	while (Sampling_Switch()==1)
	{
		if (nrf24_RX_data_available())
		{
			message = nrf24_read_message();
			if (message == HANDSHAKE_VEHICLE)
			{
				handshake_countdown = HANDSHAKE_TIMEOUT;
			}
			message = 0;
		}
		else
		{
			handshake_countdown--;
		}
		if (handshake_countdown == 0) {
			lock_request = 1;
		}
		_delay_us(150);
	}
	if (lock_request == 1) {
		*STATE = LOCKED;
	}
	else *STATE = UNLOCK;
}
int main(void)
{
	uint8_t STATE = LOCKED;
	cli();
	Init_SPI();
	Init_Switch();
	Init_LED(LOCATE_LED);
	Init_LED(LOCK_LED);
	Init_RF();
	nrf24_state(POWERUP);
	nrf24_start_listening();
	while(1) 
	{
		if (STATE == LOCKED) {
			turn_on_led(LOCK_LED);
			Locked_state_handle(&STATE);
		}
		else if (STATE == UNLOCK) {
			turn_off_led(LOCK_LED);
			Unlock_state_handle(&STATE);
		}
		else 
		{
			EngineOn_state_handle(&STATE);
		}
	}
}
