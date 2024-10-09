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


// void USART_Transmit(unsigned char data)//page 150
// {
// /* Wait for empty transmit buffer */
// while (!(UCSR0A & (1<<UDRE0)))
// ;
// /* Put data into buffer, sends the data */
// UDR0 = data;
// }

void USART_Transmit(uint8_t data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void USART_Flush(void)
{
unsigned char dummy;
while (UCSR0A & (1<<RXC0)) dummy = UDR0;
}

void USART_Print_Int(int data)
{
	int divider = 1;
	int digit = 0;
	int values[4] = {};
	int digit_count = 0;
	while(data/divider != 0)
	{
		digit = (data/divider)%10;
		//printf("%d",digit);
		divider *=10;
		values[digit_count] = digit;
		digit_count +=1;

	}
	for(int i = digit_count-1; i>=0; --i)
	{
		USART_Transmit(values[i]+0x30);
		
	}
	USART_Transmit('\n');
}
