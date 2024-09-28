default:
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o led.o led.c
	avr-gcc -o led.bin led.o

	#taking output elf (led.bin that we got from compilation), removing eeprom section, 
	#outputting it as intel hex in led.hex #avrdude (program that flashes our code) takes in intel hex format
	avr-objcopy -O ihex -R .eeprom led.bin led.hex 
	sudo avrdude -F -V -c arduino -p ATMEGA328p -P /dev/ttyUSB0 -b 115200 -U flash:w:led.hex

# MEGA2560 Only
# default:
# 	avr-gcc led.c -DF_CPU=16000000UL -mmcu=atmega2560 -Os && \
# 	avr-objcopy -j .text -j .data -O ihex a.out a.hex && \
# 	avrdude -patmega2560 -cwiring -P/dev/ttyACM0 -D -Uflash:w:a.hex:i
