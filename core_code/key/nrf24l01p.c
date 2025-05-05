/*
 * nrf24l01p.c
 *
 * Created: 22/04/2025 22:32:48
 *  Author: Admin
 */ 
#define F_CPU 1000000

#define SS 2
#define IQR 1
#define CE 0

#define RF_PORT PORTB
#define RF_PIN PINB

#include <avr/io.h>
#include <util/delay.h>
#include "QoL.h"
#include "spi.h"
#include "nrf24l01-mnemonics.h"

// Settings
uint8_t rx_address[5] = { 0xe7, 0xe7, 0xe7, 0xe7, 0xe7 };	// Read pipe address
uint8_t tx_address[5] = { 0xe7, 0xe7, 0xe7, 0xe7, 0xe7 };	// Write pipe address
#define READ_PIPE		0									// Number of read pipe
//
// -AUTO_ACK can be disabled when running on 2MBPS @ <= 32 byte messages.
// -250KBPS and 1MBPS with AUTO_ACK disabled lost many packets
// if the packet size was bigger than 4 bytes.
// -If AUTO_ACK is enabled, tx_address = rx_address.
//
#define AUTO_ACK		1								// Auto acknowledgment
#define DATARATE		RF_DR_1MBPS							// 250kbps, 1mbps, 2mbps
#define POWER			POWER_MIN							// Set power (MAX 0dBm..HIGH -6dBm..LOW -12dBm.. MIN -18dBm)
#define CHANNEL			6								// 2.4GHz-2.5GHz channel selection (0x01 - 0x7C)
#define DATA_LENGTH		1									//in byte
#define ADDR_LENGTH		3
//
// ISR(INT0_vect) is triggered depending on config (only one can be true)
//
#define RX_INTERRUPT	1								// Interrupt when message is received (RX)
#define TX_INTERRUPT	0								// Interrupt when message is sent (TX)
#define RT_INTERRUPT	0								// Interrupt when maximum re-transmits are reached (MAX_RT)
//	States
#define POWERUP		1
#define POWERDOWN	2
#define RECEIVE		3
#define TRANSMIT	4
#define STANDBY1	5
#define STANDBY2	6

// Used to store SPI commands

uint8_t nrf24_send_spi(uint8_t register_address, void *data, unsigned int bytes)
{
	uint8_t status;
	clearbit(RF_PORT,SS);
	status = SPI_MasterTransmit(register_address);
	for (unsigned int i = 0; i < bytes; i++)
	((uint8_t*)data)[i] = SPI_MasterTransmit(((uint8_t*)data)[i]);
	setbit(RF_PORT,SS);
	return status;
}

uint8_t nrf24_write(uint8_t register_address, uint8_t *data, unsigned int bytes)
{
	return nrf24_send_spi(W_REGISTER | register_address, data, bytes);
}

uint8_t nrf24_read(uint8_t register_address, uint8_t *data, unsigned int bytes)
{
	return nrf24_send_spi(R_REGISTER | register_address, data, bytes);
}

void Init_RF_Port(void)
{
	//setup port
	DDRB |= (1<<SS)|(1<<CE);
	clearbit(DDRB,IQR);
	clearbit(PORTB,CE);
	RF_PORT = (1<<IQR)|(1<<SS);
}

void Init_RF (void)
{
	uint8_t data;
	Init_RF_Port();
	
	setbit(RF_PORT,SS);
	clearbit(RF_PORT,CE);
	_delay_ms(100);		//startup wait
	
	//activate reg
	data = 0x73;
	nrf24_write(ACTIVATE,&data,1);
	
	// Start nRF24L01+ config
	data =
	(!(RX_INTERRUPT) << MASK_RX_DR) |	// IRQ interrupt on RX (0 = enabled)
	(!(TX_INTERRUPT) << MASK_TX_DS) |	// IRQ interrupt on TX (0 = enabled)
	(!(RT_INTERRUPT) << MASK_MAX_RT) |	// IRQ interrupt on auto retransmit counter overflow (0 = enabled)
	(1 << EN_CRC) |						// CRC enable
	(0 << CRC0) |						// CRC scheme
	(0 << PWR_UP) |						// Power up
	(1 << PRIM_RX);						// RX select at start
	nrf24_write(CONFIG,&data,1);
	
	// Auto-acknowledge on all pipes
	data =
	(AUTO_ACK << ENAA_P5) |
	(AUTO_ACK << ENAA_P4) |
	(AUTO_ACK << ENAA_P3) |
	(AUTO_ACK << ENAA_P2) |
	(AUTO_ACK << ENAA_P1) |
	(AUTO_ACK << ENAA_P0);
	nrf24_write(EN_AA,&data,1);
	
	// Set retries
	data = 0x0A;				// Delay 250us with 5 re-try (will be added in settings)
	nrf24_write(SETUP_RETR,&data,1);
	
	// Disable RX addresses
	data = 0;
	nrf24_write(EN_RXADDR, &data, 1);
	
	// Set channel
	data = CHANNEL;
	nrf24_write(RF_CH,&data,1);
	
	// Setup
	data =
	(0 << CONT_WAVE) |					// Continuous carrier transmit
	(DATARATE) |	// Data rate
	(POWER);				// PA level
	nrf24_write(RF_SETUP,&data,1);
	
	// Status - clear TX/RX FIFO's and MAX_RT by writing 1 into them
	data =
	(1 << RX_DR) |								// RX FIFO
	(1 << TX_DS) |								// TX FIFO
	(1 << MAX_RT);								// MAX RT
	nrf24_write(STATUS,&data,1);
	
	//specify data length
	data = DATA_LENGTH;
	nrf24_write(RX_PW_P0,&data,1);
	
	//specify address length
	data = ADDR_LENGTH;
	nrf24_write(SETUP_AW,&data,1);
	
	// Flush TX/RX
	// Clear RX FIFO which will reset interrupt
	data = (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT);
	nrf24_write(FLUSH_RX,0,0);
	nrf24_write(FLUSH_TX,0,0);
	
	// Open pipes
	nrf24_write(RX_ADDR_P0 + READ_PIPE,rx_address,5);
	nrf24_write(TX_ADDR,tx_address,5);
	data = (1 << READ_PIPE);
	nrf24_write(EN_RXADDR,&data,1);
	return;
}

uint8_t nrf24_RX_data_available(void)
{
	if ((RF_PIN & 1<<IQR) == 0) return 1;
	return 0;
}

void nrf24_state(uint8_t state)
{
	uint8_t data;
	uint8_t config_register;
	nrf24_read(CONFIG,&config_register,1);
	
	switch (state)
	{
		case POWERUP:
		// Check if already powered up
		if (!(config_register & (1 << PWR_UP)))
		{
			data = config_register | (1 << PWR_UP);
			nrf24_write(CONFIG,&data,1);
			// 1.5ms from POWERDOWN to start up
			_delay_ms(2);
		}
		break;
		case POWERDOWN:
		data = config_register & ~(1 << PWR_UP);
		nrf24_write(CONFIG,&data,1);
		break;
		case RECEIVE:
		data = config_register | (1 << PRIM_RX);
		nrf24_write(CONFIG,&data,1);
		// Clear STATUS register
		data = (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT);
		nrf24_write(STATUS,&data,1);
		break;
		case TRANSMIT:
		data = config_register & ~(1 << PRIM_RX);
		nrf24_write(CONFIG,&data,1);
		break;
		case STANDBY1:
		clearbit(RF_PORT,CE);
		break;
		case STANDBY2:
		data = config_register & ~(1 << PRIM_RX);
		nrf24_write(CONFIG,&data,1);
		setbit(RF_PORT,CE);
		_delay_us(150);
		break;
	}
}

void nrf24_start_listening(void)
{
	nrf24_state(RECEIVE);				// Receive mode
	//if (AUTO_ACK) nrf24_write_ack();	// Write acknowledgment
	setbit(RF_PORT,CE);
	_delay_us(150);						// Settling time
}

uint8_t nrf24_send_message(uint8_t message)
{
	clearbit(RF_PORT,CE);
	uint8_t data;
	// Flush TX/RX and clear TX interrupt
	nrf24_write(FLUSH_RX,0,0);
	nrf24_write(FLUSH_TX,0,0);
	data = (1 << TX_DS)|(1<<MAX_RT);
	nrf24_write(STATUS,&data,1);
	
	// Start SPI, load message into TX_PAYLOAD
	clearbit(RF_PORT,SS);
	if (AUTO_ACK) SPI_MasterTransmit(W_TX_PAYLOAD);
	else SPI_MasterTransmit(W_TX_PAYLOAD_NOACK);
	SPI_MasterTransmit(message);
	setbit(RF_PORT,SS);
	
	// Transmit mode
	nrf24_state(TRANSMIT);
	
	// Send message by pulling CE high for more than 10us
	setbit(RF_PORT,CE);
	_delay_us(20);
	clearbit(RF_PORT,CE);
	
	// Wait for message to be sent (TX_DS flag raised)
	uint8_t SEND_SUSCESS = 0;
	uint8_t finish_send = 0;
	while(finish_send == 0)
	{
		nrf24_read(STATUS,&data,1);
		if (data & (1 << TX_DS)) {
			SEND_SUSCESS = 1;
			finish_send = 1;
		}
		
		else if (data & (1 << MAX_RT)) {
			SEND_SUSCESS = 0;
			finish_send = 1;
		}
		else {
			SEND_SUSCESS = 0;
			finish_send = 0;
		}
	}
	
	//Clear TX interrupt
	data = (1 << TX_DS)|(1<<MAX_RT);
	nrf24_write(STATUS,&data,1);
	
	return SEND_SUSCESS;

}

uint8_t nrf24_read_message()
{
	uint8_t data;
	uint8_t rx_message = 0;
	// Read message
	clearbit(RF_PORT,CE);
	nrf24_send_spi(R_RX_PAYLOAD,&rx_message,1);
	data = (1 << RX_DR);
	nrf24_write(STATUS,&data,1);
	//nrf24_write(FLUSH_RX,0,0);
	setbit(RF_PORT,CE);
	return rx_message;
}