# default:
# 	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o uart.o uart.c
# 	avr-gcc -o uart.bin uart.o

# 	avr-objcopy -O ihex -R .eeprom uart.bin uart.hex 
# 	sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 115200 -U flash:w:uart.hex

# MEGA2560 Only
# default:
# 	avr-gcc uart.c -DF_CPU=16000000UL -mmcu=atmega2560 -Os && \
# 	avr-objcopy -j .text -j .data -O ihex a.out a.hex && \
# 	avrdude -patmega2560 -cwiring -P/dev/ttyACM0 -D -Uflash:w:a.hex:i

default:
	avr-gcc uart.c -DF_CPU=16000000UL -mmcu=atmega328p -Os && \
	avr-objcopy -j .text -j .data -O ihex a.out a.hex && \
	avrdude -patmega328p -carduino -P/dev/ttyUSB0 -D -Uflash:w:a.hex:i