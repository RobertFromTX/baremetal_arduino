#include "spi.h" as header
#believe variables must be outside of labels
SPI_LIB = $(shell pwd)/../spi_lib

default:	main
main:

	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c $(SPI_LIB)/spi.c -o $(SPI_LIB)/spi.o #compile static library
	avr-ar rcs $(SPI_LIB)/libspi.a $(SPI_LIB)/spi.o #create static library in libspi.a

	#compile full program
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p slave.c -I$(SPI_LIB) -L$(SPI_LIB) -lspi -o slave.o #-I tells where header files are, -L tells where .a and .so files are, -l links against spi library, needs to be named libspi.a or libspi.so
	#avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p slave.c -I$(SPI_LIB) $(SPI_LIB)/libspi.a -o slave.o #alternative to line above

	avr-gcc slave.o -o slave.bin 

	#taking output elf (slave.bin that we got from compilation), removing eeprom section, 
	#outputting it as intel hex in slave.hex #avrdude (program that flashes our code) takes in intel hex format
	avr-objcopy -O ihex -R .eeprom slave.bin slave.hex 
	sudo avrdude -F -V -c arduino -p ATMEGA328p -P /dev/ttyUSB1 -b 115200 -U flash:w:slave.hex

clean:



# #with header as #include "./../spi_lib/spi.h"
# default:
# 	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c ./../spi_lib/spi.c  -o  spi.o #compile static library
# 	avr-ar rcs spi.a spi.o #create static library
# 	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p slave.c -L. spi.a  -o slave.o 

# 	avr-gcc slave.o -o slave.bin 

# 	#taking output elf (slave.bin that we got from compilation), removing eeprom section, 
# 	#outputting it as intel hex in slave.hex #avrdude (program that flashes our code) takes in intel hex format
# 	avr-objcopy -O ihex -R .eeprom slave.bin slave.hex 
# 	sudo avrdude -F -V -c arduino -p ATMEGA328p -P /dev/ttyUSB0 -b 115200 -U flash:w:slave.hex