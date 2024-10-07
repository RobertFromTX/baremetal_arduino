#include "spi.h" 
//#include "./../spi_lib/spi.h"

#include "uart.h"

#include <avr/io.h> //not sure if this ruins it since already included in spi.h
#include <util/delay.h>

//for uart
#define FOSC 16000000UL//1843200 // Clock Speed
#define BAUD 115200
#define MYUBRR ((FOSC/(16UL*BAUD))-1)


int main(void)
{
    
    DDRB = DDRB & (~((1<<DDB2)|(1<<DDB3)|(1<<DDB4)|(1<<DDB5))); //set all of these pins to read mode/ input
    SPI_SlaveInit();
    USART_Init(MYUBRR); //for debugging purposes only


    while(1)
    {

        if((PIND & 1<<PORTD2))
        {

            USART_Transmit('W');
            USART_Transmit('\n');
            SPI_SlaveTransmit('W');
        }else if(PIND & (1 << PORTD3))
        {
            USART_Transmit('A');
            USART_Transmit('\n');
            SPI_SlaveTransmit('A');
        }else if(PIND & (1 << PORTD4))
        {
            USART_Transmit('S');
            USART_Transmit('\n');
            SPI_SlaveTransmit('S');
        }else if(PIND & (1 << PORTD5))
        {
            USART_Transmit('D');
            USART_Transmit('\n');
            SPI_SlaveTransmit('D');
        }
        else
        {
            USART_Transmit('E');
            USART_Transmit('\n');
            SPI_SlaveTransmit('E');
        }


        // _delay_ms(100);
    }

}











