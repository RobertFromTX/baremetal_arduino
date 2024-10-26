#include "spi.h" as header
#believe variables must be outside of labels
SPI_LIB = $(shell pwd)/../spi_lib
UART_LIB = $(shell pwd)/../uart_lib

INC = -I$(SPI_LIB) -I$(UART_LIB)
LIB = -L$(SPI_LIB) -L$(UART_LIB)
LINK = -lspi -luart

default:	main
main:

	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c $(SPI_LIB)/spi.c -o $(SPI_LIB)/spi.o #compile static library for spi_lib
	avr-ar rcs $(SPI_LIB)/libspi.a $(SPI_LIB)/spi.o #create spi static library in libspi.a

	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c $(UART_LIB)/uart.c -o $(UART_LIB)/uart.o #compile static library for uart
	avr-ar rcs $(UART_LIB)/libuart.a $(UART_LIB)/uart.o #create uart static library in libuart.a

	#compile full program
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p controller.cpp $(INC) $(LIB) $(LINK) -o controller.o #-I tells where header files are, -L tells where .a and .so files are, -l links against spi library, needs to be named libspi.a or libspi.so

	avr-gcc controller.o -o controller.bin 

	#taking output elf (controller.bin that we got from compilation), removing eeprom section, 
	#outputting it as intel hex in controller.hex #avrdude (program that flashes our code) takes in intel hex format
	avr-objcopy -O ihex -R .eeprom controller.bin controller.hex 
	sudo avrdude -F -V -c arduino -p ATMEGA328p -P /dev/ttyUSB0 -b 115200 -U flash:w:controller.hex

clean:


