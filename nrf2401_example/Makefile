#include "spi.h" as header
#believe variables must be outside of labels
SPI_LIB = $(shell pwd)/nrf2401_lib
UART_LIB = $(shell pwd)//uart_lib

INC = -I$(UART_LIB) -I$(SPI_LIB)
LIB = -L$(UART_LIB)
LINK = -luart
default:	main
main:
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c $(UART_LIB)/uart.c -o $(UART_LIB)/uart.o #compile static library for uart
	avr-ar rcs $(UART_LIB)/libuart.a $(UART_LIB)/uart.o #create uart static library in libuart.a

	#compile full program
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p nRF2401_Transmitter.c $(INC) $(LIB) $(LINK) -o nRF2401_Transmitter.o #-I tells where header files are, -L tells where .a and .so files are, -l links against spi library, needs to be named libspi.a or libspi.so

	avr-gcc nRF2401_Transmitter.o -o nRF2401_Transmitter.bin 

	#taking output elf (nRF2401_Transmitter.bin that we got from compilation), removing eeprom section, 
	#outputting it as intel hex in nRF2401_Transmitter.hex #avrdude (program that flashes our code) takes in intel hex format
	avr-objcopy -O ihex -R .eeprom nRF2401_Transmitter.bin nRF2401_Transmitter.hex 
	sudo avrdude -F -V -c arduino -p ATMEGA328p -P /dev/ttyUSB1 -b 115200 -U flash:w:nRF2401_Transmitter.hex

clean:

