default:
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c ./../spi_lib/spi.c  -o  spi.o #compile static library
	avr-ar rcs spi.a spi.o #create static library
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p slave.c -L. spi.a  -o slave.o 

	avr-gcc slave.o -o slave.bin 

	#taking output elf (slave.bin that we got from compilation), removing eeprom section, 
	#outputting it as intel hex in slave.hex #avrdude (program that flashes our code) takes in intel hex format
	avr-objcopy -O ihex -R .eeprom slave.bin slave.hex 
	sudo avrdude -F -V -c arduino -p ATMEGA328p -P /dev/ttyUSB0 -b 115200 -U flash:w:slave.hex