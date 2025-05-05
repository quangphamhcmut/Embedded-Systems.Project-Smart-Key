/*
 * nrf24l01p.h
 *
 * Created: 22/04/2025 22:33:16
 *  Author: Admin
 */ 


#ifndef NRF24L01P_H_
#define NRF24L01P_H_

#include "nrf24l01p.h"

uint8_t nrf24_send_spi(uint8_t register_address, void *data, unsigned int bytes);
uint8_t nrf24_write(uint8_t register_address, uint8_t *data, unsigned int bytes);
uint8_t nrf24_read(uint8_t register_address, uint8_t *data, unsigned int bytes);
void Init_RF_Port(void);
void Init_RF (void);
uint8_t nrf24_RX_data_available(void);
void nrf24_state(uint8_t state);
void nrf24_start_listening(void);
uint8_t nrf24_send_message(uint8_t message);
uint8_t nrf24_read_message();
uint8_t nrf24_RX_power(); 

//define output
#define SS 2
#define IQR 1
#define CE 0

#define RF_PORT PORTB
#define RF_PIN PINB

//	States
#define POWERUP		1
#define POWERDOWN	2
#define RECEIVE		3
#define TRANSMIT	4
#define STANDBY1	5
#define STANDBY2	6

#endif /* NRF24L01P_H_ */