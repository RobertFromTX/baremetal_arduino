#include "spi.h" 
//#include "./../spi_lib/spi.h"


#include <avr/io.h> //not sure if this ruins it since already included in spi.h
#include <util/delay.h>

//for uart
#define FOSC 16000000UL//1843200 // Clock Speed
#define BAUD 115200
#define MYUBRR ((FOSC/(16UL*BAUD))-1)

void USART_Init(unsigned int ubrr)//page 149
{
/*Set baud rate */
UBRR0H = (unsigned char)(ubrr>>8);
UBRR0L = (unsigned char)ubrr; //this is the line that breaks it
/*Enable receiver and transmitter */
UCSR0B = (1<<RXEN0)|(1<<TXEN0);
/* Set frame format: 8data, 2stop bit */
UCSR0C = (1<<USBS0)|(3<<UCSZ00);
UBRR0L=0x67;
}

void USART_Transmit(unsigned char data)//page 150
{
/* Wait for empty transmit buffer */
while (!(UCSR0A & (1<<UDRE0)))
;
/* Put data into buffer, sends the data */
UDR0 = data;
}




int main(void)
{
    USART_Init(MYUBRR);
    SPI_MasterInit();

    while(1)
    {
        USART_Transmit('M');  
        USART_Transmit('T');
        USART_Transmit('B');
        USART_Transmit('\n');
        SPI_MasterTransmit('M');
        USART_Transmit('M'); 
        USART_Transmit('T');
        USART_Transmit('E');
        USART_Transmit('\n');
        USART_Transmit(SPDR);  
        USART_Transmit('\n');

        _delay_ms(100);

        USART_Transmit('M');  
        USART_Transmit('R');
        USART_Transmit('B');
        USART_Transmit('\n');
        char data = SPI_MasterReceive();
        USART_Transmit(data);
        USART_Transmit('\n');
        USART_Transmit('M'); 
        USART_Transmit('R');
        USART_Transmit('E');
        USART_Transmit('\n');


        _delay_ms(100);
    }

}