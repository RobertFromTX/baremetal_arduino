#include "spi.h" 
//#include "./../spi_lib/spi.h"


#include <avr/io.h> //not sure if this ruins it since already included in spi.h
#include <util/delay.h>

//for uart
#define FOSC 16000000UL//1843200 // Clock Speed
#define BAUD 115200
#define MYUBRR ((FOSC/(16UL*BAUD))-1)



#define DDR_SPI DDRB//to find out which data direction register, look up pinout to see where the SPI pins are (MISO, MOSI, SCK)
#define DD_MOSI DDB3
#define DD_SCK DDB5
#define DD_MISO DDB4
#define SS DDB2

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
    USART_Init(MYUBRR); //for debugging purposes only
    DDRB = DDRB & (~((1<<DDB2)|(1<<DDB3)|(1<<DDB4)|(1<<DDB5))); //set all of these pins to read mode/ input

    while(1)
    {
        if((PIND & 1<<PORTD2))
        {

            USART_Transmit('W');
            USART_Transmit('\n');
        }else if(PIND & (1 << PORTD3))
        {
            USART_Transmit('A');
            USART_Transmit('\n');
        }else if(PIND & (1 << PORTD4))
        {
            USART_Transmit('S');
            USART_Transmit('\n');
        }else if(PIND & (1 << PORTD5))
        {
            USART_Transmit('D');
            USART_Transmit('\n');
        }
        else
        {
            USART_Transmit('E');
            USART_Transmit('\n');
        }


        _delay_ms(400);
    }

}











