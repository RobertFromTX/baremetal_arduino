#include "spi.h"

#include <avr/io.h>
#include <util/delay.h>

#define DDR_SPI DDRB//to find out which data direction register, look up pinout to see where the SPI pins are (MISO, MOSI, SCK)
#define DD_MOSI DDB3
#define DD_SCK DDB5
#define DD_MISO DDB4
#define SS DDB2


void SPI_MasterInit(void) //set in master mode (check data sheet)
{
/* Set MOSI and SCK and SS output, all others input */
DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK) | (1<<SS); // added | (1<<SS)
/* Enable SPI, Master, set clock rate fck/16 */
SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

char SPI_MasterReceive(void)
{
    PORTB &= ~(1<<SS); //SS low to allow slave to transmit
    while(!(SPSR & (1<<SPIF)));
    PORTB |= 1<<SS; //pull SS to high to let slave know it is no longer transmitting
    return SPDR
}

void SPI_MasterTransmit(char cData)
{
PORTB &= ~(1<<SS); //SS low to start transmitting (SS is PB2)
//while( (PORTB & (1<<0)); //don't think this is needed at all
/* Start transmission */
SPDR = cData;
/* Wait for transmission complete */
while(!(SPSR & (1<<SPIF))); //page 136: after 8 bits shifted in SPDR (SPI data register_), SPIF is set to high and transmission is over
PORTB |= 1<<SS; //pull SS to high
}

void SPI_SlaveInit(void) //set in slave mode
{
/* Set MISO output, all others input */
DDR_SPI = (1<<DD_MISO);
/* Enable SPI */
SPCR = (1<<SPE);
}

char SPI_SlaveReceive(void)
{
/* Wait for reception complete */
while(!(SPSR & (1<<SPIF)));
/* Return Data Register */
return SPDR;
}

void SPI_SlaveTransmit(char cData)
{
    while(!(PORTB & 1<<SS)); //waiting for SS to be set to low from master
    //while( (PORTB & (1<<0)) ); //don't think this is needed at all
    SPDR = cData;
    while(!(SPSR & (1<<SPIF)));

}