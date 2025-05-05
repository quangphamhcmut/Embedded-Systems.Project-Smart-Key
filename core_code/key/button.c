/*
 * button.c
 *
 * Created: 22/04/2025 22:48:37
 *  Author: Admin
 */  
#include<avr/io.h>

#define BUTTON_DATA DDRD
#define BUTTON_PORT PORTD
#define BUTTON_PIN PIND

uint8_t Init_Button (uint8_t button_num) 
{
	BUTTON_DATA &= ~(1<<button_num);
	BUTTON_PORT |= 1<<(button_num);
	return button_num;
}

void Reopen_Button_Port (uint8_t *button_state_num) 
{
	BUTTON_DATA &= ~(1<<((*button_state_num) & 0x3F));
	BUTTON_PORT |= 1<<((*button_state_num) & 0x3F);
}

uint8_t SamplingButton(uint8_t *button_state_num)
{
	uint8_t button = 0;
	uint8_t button_num = (*button_state_num) & 0x3F;
	uint8_t button_state = ((*button_state_num)>>6)& 0x03;
	if (BUTTON_PIN & (1<<(button_num))){
		if (button_state == 0) {button_state = 1; button = 0;}
		else if (button_state == 1) {button_state = 2; button = 1;}
		else if (button_state == 2) {button_state = 2; button = 0;}
	}
	else {
		if (button_state == 0) {button_state = 0; button = 0;}
		else if (button_state == 1) {button_state = 0; button = 0;}
		else if (button_state == 2) {button_state = 0; button = 0;}
	}
	*button_state_num = (button_state << 6) | button_num;
	return button;
}