#include "spi.h" 
//#include "./../spi_lib/spi.h"
#include <RF24/RF24.h>
#include "uart.h"

#include <avr/io.h> //not sure if this ruins it since already included in spi.h
#include <util/delay.h>

//for uart
#define FOSC 16000000UL//1843200 // Clock Speed
#define BAUD 115200
#define MYUBRR ((FOSC/(16UL*BAUD))-1)

struct position

int main(void)
{
    USART_Init(MYUBRR);
    SPI_MasterInit();

    while(1)
    {


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

        if(data=='W')
        {
            continue;
        }else if(data=='A')
        {
            continue;
        }else if(data=='S')
        {
            continue;
        }else if(data=='D')
        {
            continue;
        }else
        {
            continue;
        }
    }

}









