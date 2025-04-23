#include <avr/io.h>
#define MOSI 3
#define MISO 4
#define SCK 5
void Init_SPI (void)
{
	DDRB |= (1<<MOSI)|(1<<SCK); //SCK, MOSI as output
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPI2X); //Enable SPI, Master, set clock rate fclk/2 = 4MHz
}

uint8_t SPI_MasterTransmit(uint8_t transmit_data)
{
	SPDR = transmit_data;
	while(!(SPSR & (1<<SPIF))) {}
	return SPDR;	//take receive data
}