default:
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -o spi.o spi.c
	avr-gcc spi.o -o spi.bin

	#taking output elf (spi.bin that we got from compilation), removing eeprom section, 
	#outputting it as intel hex in spi.hex #avrdude (program that flashes our code) takes in intel hex format
	avr-objcopy -O ihex -R .eeprom spi.bin spi.hex 
	sudo avrdude -F -V -c arduino -p ATMEGA328p -P /dev/ttyUSB0 -b 115200 -U flash:w:spi.hex