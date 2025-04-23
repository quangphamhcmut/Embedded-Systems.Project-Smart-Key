/*
 * button.h
 *
 * Created: 22/04/2025 23:01:55
 *  Author: Admin
 */ 


#ifndef BUTTON_H_
#define BUTTON_H_

uint8_t Init_Button (uint8_t button_num); 

void Reopen_Button_Port (volatile uint8_t *button_state_num);

uint8_t SamplingButton(volatile uint8_t *button_state_num);

#endif /* BUTTON_H_ */