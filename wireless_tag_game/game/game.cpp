#include "spi.h" 
//#include "./../spi_lib/spi.h"
#include "uart.h"

#include <avr/io.h> //not sure if this ruins it since already included in spi.h
#include <util/delay.h>

#include "nRF24L01_funcs.h"

//for uart
#define FOSC 16000000UL//1843200 // Clock Speed
#define BAUD 115200
#define MYUBRR ((FOSC/(16UL*BAUD))-1)

struct position;

int main(void)
{
    receiver();
}









