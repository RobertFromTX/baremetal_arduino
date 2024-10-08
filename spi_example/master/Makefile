#include "spi.h" as header
#believe variables must be outside of labels
SPI_LIB = $(shell pwd)/../spi_lib

default:	main
main:

	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c $(SPI_LIB)/spi.c -o $(SPI_LIB)/spi.o #compile static library
	avr-ar rcs $(SPI_LIB)/libspi.a $(SPI_LIB)/spi.o #create static library in libspi.a

	#compile full program
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p master.c -I$(SPI_LIB) -L$(SPI_LIB) -lspi -o master.o #-I tells where header files are, -L tells where .a and .so files are, -l links against spi library, needs to be named libspi.a or libspi.so
	#avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p master.c -I$(SPI_LIB) $(SPI_LIB)/libspi.a -o master.o #alternative to line above

	avr-gcc master.o -o master.bin 

	#taking output elf (master.bin that we got from compilation), removing eeprom section, 
	#outputting it as intel hex in master.hex #avrdude (program that flashes our code) takes in intel hex format
	avr-objcopy -O ihex -R .eeprom master.bin master.hex 
	sudo avrdude -F -V -c arduino -p ATMEGA328p -P /dev/ttyUSB1 -b 115200 -U flash:w:master.hex

clean:






# #with header as #include "./../spi_lib/spi.h"
# default:
# 	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c ./../spi_lib/spi.c  -o  spi.o #compile static library
# 	avr-ar rcs spi.a spi.o #create static library
# 	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p master.c -L. spi.a  -o master.o 

# 	avr-gcc master.o -o master.bin 

# 	#taking output elf (master.bin that we got from compilation), removing eeprom section, 
# 	#outputting it as intel hex in master.hex #avrdude (program that flashes our code) takes in intel hex format
# 	avr-objcopy -O ihex -R .eeprom master.bin master.hex 
# 	sudo avrdude -F -V -c arduino -p ATMEGA328p -P /dev/ttyUSB0 -b 115200 -U flash:w:master.hex

