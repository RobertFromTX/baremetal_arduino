//code from page 150, 168 and 169 of atmega398 datasheet, look at sections with 5-8 bit data
#include <avr/io.h>
#include <util/delay.h>

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

unsigned char USART_Receive(void)//page 152
{
/* Wait for data to be received */
while (!(UCSR0A & (1<<RXC0)))
;
/* Get and return received data from buffer */
return UDR0; 
}


void USART_Transmit(unsigned char data)//page 150
{
/* Wait for empty transmit buffer */
while (!(UCSR0A & (1<<UDRE0)))
;
/* Put data into buffer, sends the data */
UDR0 = data;
}

void USART_Flush(void)
{
unsigned char dummy;
while (UCSR0A & (1<<RXC0)) dummy = UDR0;
}

void main(void)
{
USART_Init(MYUBRR);
while(1)
{
    USART_Transmit('G');
    USART_Transmit('I');
    USART_Transmit('V');
    USART_Transmit('E');
    USART_Transmit(' ');
    USART_Transmit('M');
    USART_Transmit('E');
    USART_Transmit(' ');
    USART_Transmit('B');
    USART_Transmit('A');
    USART_Transmit('M');
    USART_Transmit('B');
    USART_Transmit('O');
    USART_Transmit('O');

    USART_Transmit('\n');
    _delay_ms(2000);

}

}

// #define F_CPU 16000000UL
// #include <avr/io.h>
// #include <util/delay.h>
 
 
// void AVR_init(void)
// {
// // USART initialization
// // Communication Parameters: 8 Data, 1 Stop, No Parity
// // USART Receiver: Off
// // USART Transmitter: On
// // USART Mode: Asynchronous
// // USART Baud Rate: 9600
// UCSR0A=(0<<RXC0) | (0<<TXC0) | (0<<UDRE0) | (0<<FE0) | (0<<DOR0) | (0<<UPE0) | (0<<U2X0) | (0<<MPCM0);
// UCSR0B=(0<<RXCIE0) | (0<<TXCIE0) | (0<<UDRIE0) | (0<<RXEN0) | (1<<TXEN0) | (0<<UCSZ02) | (0<<RXB80) | (0<<TXB80);
// UCSR0C=(0<<UMSEL01) | (0<<UMSEL00) | (0<<UPM01) | (0<<UPM00) | (0<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00) | (0<<UCPOL0);
// UBRR0H=0x00;
// UBRR0L=0x67;
 
// }

// void USART_Tx(unsigned char data)
// {
// 	while(!(UCSR0A&(1<<UDRE0)));
// 	UDR0=data;
// }
 
// void main(void)
// {
//     AVR_init();
 
//     while(1)
//     {
//         USART_Tx('A');
//         _delay_ms(2000);
//         USART_Tx('\n');
//         _delay_ms(2000);
//     }
// }