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
    SPDR = 'K';
    //SPDR = 0x06; //must write to register to start transmission, data being writtin in is junk
    while(!(SPSR & (1<<SPIF)));
    PORTB |= 1<<SS; //block slave off from transmitting
    char cData = SPDR; // read the register to get the data
    return cData;
}

void SPI_MasterTransmit(char cData)
{
    DDRB = DDRB | (1<<DDB1); //overwrites DDRB and sets PORTB5 to output
    PORTB &= ~(1<<SS); //SS low to start transmitting (SS is PB2)
    //while( (PORTB & (1<<0)); //don't think this is needed at all
    /* Start transmission */
    SPDR = cData; //get data in register to begin transmitting
    /* Wait for transmission complete */
    PORTB = PORTB | (1 << PORTB1); //pin D9 is on until transmission done
    while(!(SPSR & (1<<SPIF))); //page 136: after 8 bits shifted in SPDR (SPI data register_), SPIF is set to high and transmission is over
    PORTB |= 1<<SS; //pull SS to high
    PORTB = PORTB & ~(1 << PORTB1);
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
    //SPDR = 0x06; // junk data, not meaningful
    /* Wait for reception complete */
    SPDR = 'W';
    while(!(SPSR & (1<<SPIF)));
    /* Return Data Register */
    return SPDR;
}

void SPI_SlaveTransmit(char cData)
{
    //DDRB = DDRB | (1<<DDB1); //overwrites DDRB and sets PORTB1 to output

    SPDR = cData; //get data ready in register to transmit, SPDR register stays the same until master pulls SS low   

    //PORTB = PORTB | (1 << PORTB1); //led is on until transmission done

    //_delay_ms(2000);
    while (PORTB & 1<<SS);
    while(!(SPSR & (1<<SPIF))); //wait for transmission to end

    //PORTB = PORTB & ~(1 << PORTB1);
}