#include "./../spi_lib/spi.h"


#include <avr/io.h> //not sure if this ruins it since already included in spi.h
#include <util/delay.h>

int main(void)
{
    SPI_MasterInit();

    while(1)
    {
        SPI_MasterTransmit('A');
        SPI_MasterTransmit('\n');
        _delay_ms(2000);
    }

}