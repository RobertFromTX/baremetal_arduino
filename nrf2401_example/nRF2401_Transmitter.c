#include <avr/io.h>
#include <stdio.h>
#define FOSC 16000000UL  // 16 MHz

//for uart
#define BAUD 115200
#define MYUBRR ((FOSC/(16UL*BAUD))-1)

#include <util/delay.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "nRF24L01.h"

//ADDED
#define BIT(x) (1<<(x))
#define SETBITS(x,y) ((x) |= y)
#define CLEARBITS(x,y) ((x) &= (~(y)))
#define SETBIT(x,y) SETBITS((x), (BIT((y))))
#define CLEARBIT(x,y) CLEARBITS ((x), (BIT((y))))

#define W 1
#define R 0

#define DDR_SPI DDRB//to find out which data direction register, look up pinout to see where the SPI pins are (MISO, MOSI, SCK)
#define DD_MOSI DDB3
#define DD_SCK DDB5
#define DD_MISO DDB4
#define SS DDB2
#define CE DDB1

#define dataLen 5  //längd på datapacket som skickas/tas emot
uint8_t *data;
uint8_t *arr;

//The nRF chip communicates with the AVR-chip using SPI which has to be initialized in the AVR according to its datasheet. Here is the initializing cod
void InitSPI(void)//initialize spi so that controller reader to talk to transmitter
{
	//Set SCK , MOSI , CSN (SS & PB2) & CE  as output, master mode
	//OBS!!! Has to be set before SPI-Enable below

    DDR_SPI |= (1<<DD_MOSI) | (1<<DD_SCK) | (1<<SS) | (1<<CE); 
	
	//Enable SPI, Master, set clock rate fck/16 .. clock rate not too important
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	
	SETBIT(PORTB, 2);	//CSN IR_High to start with, nothing to be sent to nRF yet
	CLEARBIT(PORTB, 1);	//CE low to start with, nothing to send and receive yet
}


//Now to send and receive a byte from the nRF with the SPI all you have to do is to use this function:
char WriteByteSPI(unsigned char cData)
{

	//Load byte to Data register
	SPDR = cData;	
		
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));

	//Return what is received from the nRF
	return SPDR;
}


// nRF24L01(+) communication
// Now to the fun part...

// How it works
// 1) The nRF starts listening for commands when the CSN-pin goes low.
// 2) after a delay of 10us it accepts a single byte through SPI, which tells the nRF which bytes you want to read/write to, and if you want to read or write to it.
// 3) a 10us delay later it then accepts further bytes which is either written to the above specified register, or a number of dummy bytes (that tells the nRF how many bytes you want to read out)
// 4) when finished close the connection by setting CSN to high again.
uint8_t GetReg(uint8_t reg)
{	
	_delay_us(10); //make sure last command was a while ago
	CLEARBIT(PORTB, 2);	//CSN low - nRF starts to listen for command
	_delay_us(10);
	WriteByteSPI(R_REGISTER + reg);	//R_Register = set the nRF to reading mode, "reg" = this registry will be read back
	_delay_us(10);
	reg = WriteByteSPI(RF24_NOP);	//Send NOP (dummy byte) once to receive back the first byte in the "reg" register
	_delay_us(10);
	SETBIT(PORTB, 2);	//CSN IR - nRF goes back to doing nothing
	return reg;	// Return the read registry
}

// void WriteToNrf(uint8_t reg, uint8_t package)
// {
// 	_delay_us(10); //make sure last command was a while ago
// 	CLEARBIT(PORTB, 2);	//CSN low - nRF starts to listen for command
// 	_delay_us(10);
// 	WriteByteSPI(R_REGISTER + reg);	//R_Register = set the nRF to reading mode, "reg" = this registry will be read back
// 	_delay_us(10);
// 	WriteByteSPI(package);	//Send NOP (dummy byte) once to receive back the first byte in the "reg" register
// 	_delay_us(10);
// 	SETBIT(PORTB, 2);	//CSN IR - nRF goes back to doing nothing
// }

uint8_t *WriteToNrf(uint8_t ReadWrite, uint8_t reg, uint8_t *val, uint8_t antVal)
{
	//"ReadWrite" ("W" or "R"), "reg" (the register), "*val" (an array with the package) & "antVal" (number of integers in the package)
	//cli();	//disable global interrupt
	
	if (ReadWrite == W) //if "W" then you want to write to the nRF (read mode "R" == 0x00, so skipping that one)	
	{
		reg = W_REGISTER + reg;	//Add the "write" bit to the "reg"
	}
	
	//Create an array to be returned at the end
	//Static uint8_t is needed to be able to return an array  notice the "*" to the left of the function: "WriteToNRf())
	static uint8_t ret[dataLen];	
	
	_delay_us(10);		//make sure last command was a while ago...
	CLEARBIT(PORTB, 2);	//CSN low = nRF starts to listen for command
	_delay_us(10);		
	WriteByteSPI(reg);	//set the nRF to Write or read mode of "reg"
	_delay_us(10); 		
	
	int i;
	for(i=0; i<antVal; i++)
	{
		if (ReadWrite == R && reg != W_TX_PAYLOAD)	//Did you want to read a registry?
		{											//When writing to W_TX_Payload you cannot add the "W" since it is on the same level in the registry...	
			ret[i]=WriteByteSPI(RF24_NOP);	//Send dummy bytes to read out the data	
			_delay_us(10);			
		}
		else 
		{
			WriteByteSPI(val[i]);	//Send the commands to the nRF once at a time	
			_delay_us(10);
		}		
	}
	SETBIT(PORTB, 2);	//CSN Hi - nRF goes back to doing nothing 
	
	//sei(); //enable global interrupt
	
	return ret;	//return the array
}



void nrf24L01_init(void)
{
	_delay_ms(100);	//allow radio to reach power down if shut down
	
	uint8_t val[5]; //An array of integers to send to the *WriteToNrf function	

	//EN_AA - (auto-acknowledgements) - Transmitter gets automatic response from receiver when successful transmission! (lovely function!) 
	//Only works if Transmitter has identical RF_address on its channel ex: RX_ADDR_P0 = TX_ADDR
	val[0]=0x01;	//set value
	WriteToNrf(W, EN_AA, val, 1);	//W=write mode, EN_AA=register to write to, val=data to write, 1=number of data bytes.
	
	//SETUP_RETR (the setup for "EN_AA")
	val[0]=0x2F;	//0b0010 00011 "2" sets it up to 750uS delay between every retry (at least 500us at 250kbps and if payload >5bytes in 1Mbps, 
					//and if payload >15byte in 2Mbps) "F" is number of retries (1-15, now 15)
	WriteToNrf(W, SETUP_RETR, val, 1);
	
	//Choose number of enabled data pipes (1-5)	
	val[0]=0x01;
	WriteToNrf(W, EN_RXADDR, val, 1); //enable data pipe 0

	//RF_Address width setup (how many bytes is the receiver address, the more the merrier 1-5)	
	val[0]=0x03;	//0b0000 00011 = 5 bytes RF_Address
	WriteToNrf(W, SETUP_AW, val, 1); 

	//RF channel setup - choose frequency 2,400-2,527 GHz 1MHz/step
	val[0]=0x01;	//RD Channel registry 0b0000 0001 = 2,401 GHz (same on TX and RX)	
	WriteToNrf(W, RF_CH, val, 1); 
	
	//RF setup	- choose power mode and data speed. here is the difference with the (+) version!!! 
	val[0]=0x07;	//00000111 bit 3="0" 1Mbps=longer range, 2-1 power mode ("11" = -0dB ; "00"=-18dB) 	
	WriteToNrf(W, RF_SETUP, val, 1);

	//RX RF_Adress setup 5 byte - Set Receiver address (set RX_ADDR_P0 = TX_ADDR if EN_AA is enabled!!!) 
	int i;
	for(i=0; i<5; i++)	
	{
		val[i]=0x12;	//0x12 x 5 to get a long and secure address.	
	}
	WriteToNrf(W, RX_ADDR_P0, val, 5); //since we chose pipe 0 on EN_RXADDR we give this address to that channel 
	//Here you can give different addresses to different channels (if they are enabled in EN_RXADDR) to listen on several different transmitters)

	//TX RF_Address setup 5 bytes - Set transmitter address (not used in a receiver but can be set anyways)	
	for(i=0; i<5; i++)	
	{
		val[i]=0x12;	//0x12 x 5 - same on the Receiver chip and the RX-RF_Address above if EN_AA is enabled!!!
	}
	WriteToNrf(W, TX_ADDR, val, 5); 

	// payload width setup - 1-32byte (how many bytes to send per transmission) 
	val[0]=dataLen;		//Send 5 bytes per package this time (same on receiver and transmitter)
	WriteToNrf(W, RX_PW_P0, val, 1);
	
	//CONFIG reg setup - Now it's time to boot up the nRF and choose if it's supposed to be a transmitter or receiver
	val[0]=0x1E;  //0b0000 1110 - bit 0="0":transmitter bit 0="1":Receiver, bit 1="1"=power up,
					//bit 4="1" = mask_MAX_RT i.e IRQ-interrupt is not triggered if transmission failed.
	WriteToNrf(W, NRF_CONFIG, val, 1);

//device need 1.5ms to reach standby mode (CE=low)
	_delay_ms(100);	

	//sei();	
}



void nrf24L01_init_receive(void)
{
	_delay_ms(100);	//allow radio to reach power down if shut down
	
	uint8_t val[5]; //An array of integers to send to the *WriteToNrf function	

	//EN_AA - (auto-acknowledgements) - Transmitter gets automatic response from receiver when successful transmission! (lovely function!) 
	//Only works if Transmitter has identical RF_address on its channel ex: RX_ADDR_P0 = TX_ADDR
	val[0]=0x01;	//set value
	WriteToNrf(W, EN_AA, val, 1);	//W=write mode, EN_AA=register to write to, val=data to write, 1=number of data bytes.
	
	//SETUP_RETR (the setup for "EN_AA")
	val[0]=0x2F;	//0b0010 00011 "2" sets it up to 750uS delay between every retry (at least 500us at 250kbps and if payload >5bytes in 1Mbps, 
					//and if payload >15byte in 2Mbps) "F" is number of retries (1-15, now 15)
	WriteToNrf(W, SETUP_RETR, val, 1);
	
	//Choose number of enabled data pipes (1-5)	
	val[0]=0x01;
	WriteToNrf(W, EN_RXADDR, val, 1); //enable data pipe 0

	//RF_Address width setup (how many bytes is the receiver address, the more the merrier 1-5)	
	val[0]=0x03;	//0b0000 00011 = 5 bytes RF_Address
	WriteToNrf(W, SETUP_AW, val, 1); 

	//RF channel setup - choose frequency 2,400-2,527 GHz 1MHz/step
	val[0]=0x01;	//RD Channel registry 0b0000 0001 = 2,401 GHz (same on TX and RX)	
	WriteToNrf(W, RF_CH, val, 1); 
	
	//RF setup	- choose power mode and data speed. here is the difference with the (+) version!!! 
	val[0]=0x07;	//00000111 bit 3="0" 1Mbps=longer range, 2-1 power mode ("11" = -0dB ; "00"=-18dB) 	
	WriteToNrf(W, RF_SETUP, val, 1);

	//RX RF_Adress setup 5 byte - Set Receiver address (set RX_ADDR_P0 = TX_ADDR if EN_AA is enabled!!!) 
	int i;
	for(i=0; i<5; i++)	
	{
		val[i]=0x12;	//0x12 x 5 to get a long and secure address.	
	}
	WriteToNrf(W, RX_ADDR_P0, val, 5); //since we chose pipe 0 on EN_RXADDR we give this address to that channel 
	//Here you can give different addresses to different channels (if they are enabled in EN_RXADDR) to listen on several different transmitters)

	//TX RF_Address setup 5 bytes - Set transmitter address (not used in a receiver but can be set anyways)	
	for(i=0; i<5; i++)	
	{
		val[i]=0x12;	//0x12 x 5 - same on the Receiver chip and the RX-RF_Address above if EN_AA is enabled!!!
	}
	WriteToNrf(W, TX_ADDR, val, 5); 

	// payload width setup - 1-32byte (how many bytes to send per transmission) 
	val[0]=dataLen;		//Send 5 bytes per package this time (same on receiver and transmitter)
	WriteToNrf(W, RX_PW_P0, val, 1);
	
	//CONFIG reg setup - Now it's time to boot up the nRF and choose if it's supposed to be a transmitter or receiver
	val[0]=0x1F;  //0b0000 1110 - bit 0="0":transmitter bit 0="1":Receiver, bit 1="1"=power up,
					//bit 4="1" = mask_MAX_RT i.e IRQ-interrupt is not triggered if transmission failed.
	WriteToNrf(W, NRF_CONFIG, val, 1);

//device need 1.5ms to reach standby mode (CE=low)
	_delay_ms(100);	

	//sei();	
}


//Send data
void transmit_payload(uint8_t * W_buff)
{
	WriteToNrf(R, FLUSH_TX, W_buff, 0); //Sends 0xE1 to flush the registry from old data! W_buff[] is only there because an array has to be called with an array....
	WriteToNrf(R, W_TX_PAYLOAD, W_buff, dataLen);	//Sends the data in W_buff to the nRF
	//Why Flush_TX and W_TX_Payload is sent with an "R" instead of "W" is because they are on the highest byte-level in the nRF (see datasheet below)!

	//sei();	//enable global interrupt (if interrupt is used) 

	_delay_ms(10);		//needs 10ms delay to work after loading the nRF with the payload for some reason	
	SETBIT(PORTB, 1);	//CE high=transmit the data!
	_delay_us(20);		//delay at least 10us!
	CLEARBIT(PORTB, 1);	//CE low = stop transmitting
	_delay_ms(10);		//long delay again before proceeding

	//cli();	//Disable global interrupt... then the USART_RX listening is turned off!

}


//receive data
void receive_payload(void)
{
	//sei();		//Enable global interrupt
	
	SETBIT(PORTB, 1);	//CE IR_High = "listens" for data 
	_delay_ms(1000);	//listens for 1s at a time
	CLEARBIT(PORTB, 1); //ce low again - stop listening 
	
	//cli();	//Disable global interrupt
}

//After every received/transmitted payload the IRQ's in the nRF has to be reset in order to receive/transmit next package. This is done like this:
void reset(void)
{
	_delay_us(10);
	CLEARBIT(PORTB, 2);	//CSN low
	_delay_us(10);
	WriteByteSPI(W_REGISTER + NRF_STATUS);	//write to the NRF_STATUS registry
	_delay_us(10);
	WriteByteSPI(0b01110000);	//Reset all irq in STATUS registry
	_delay_us(10);
	SETBIT(PORTB, 2);	//CSN IR_High
}



int main(void)
{
	// InitSPI();
	// USART_Init(MYUBRR);
    // while(1)
    // {
	// 	USART_Transmit('B');
	// 	USART_Transmit('\n');
    //     USART_Transmit(GetReg(NRF_STATUS));
	// 	USART_Transmit('\n');
	// _delay_ms(500);
    // }




	// // //transmitter
	// USART_Init(MYUBRR);
	// InitSPI();
	// nrf24L01_init();
	// uint8_t W_buffer[5];
	// int i;
	// for(i=0; i<5; ++i)
	// {
	// 	W_buffer[i]=0x24;
	// }
	// while(1)
	// {
	//  	// USART_Transmit('T');
	// 	// USART_Transmit('B');
	//  	// USART_Transmit('\n');

	// 	transmit_payload(W_buffer);
	// 	// while((GetReg(NRF_STATUS) & (1<<4)) !=0)
	// 	// {
	// 	// 	uint8_t new[1];
	// 	// 	new[0] = (GetReg(NRF_STATUS) & ~(1<<4));
	// 	// 	WriteToNrf(W, NRF_STATUS, new, 1);
	// 	// 	USART_Print_Int(new[0]);
	// 	// 	USART_Print_Int((GetReg(NRF_STATUS)));
	// 	// }
	// 	if((GetReg(NRF_STATUS) & (1<<4)) !=0)
	// 	{
	// 		USART_Transmit('T');
	// 		USART_Transmit('E');
	// 		USART_Transmit('\n');
	// 	} else
	// 	{
	// 		USART_Transmit('S');
	// 		USART_Transmit('E');
	// 		USART_Transmit('\n');
	// 	}
		
	// }




// // //receiver
	USART_Init(MYUBRR);
	InitSPI();
	// INT0_interrupt_init();
	nrf24L01_init_receive();

	uint8_t* R_buffer;
	int i;
	for(i=0; i<5; ++i)
	{
		R_buffer[i]=0x54;
	}
	while(1)
	{
	 	// USART_Transmit('R');
		// USART_Transmit('B');
	 	// USART_Transmit('\n');

       	receive_payload();
		if ((GetReg(NRF_STATUS)&(1<<6)) !=0)
		{
			data=WriteToNrf(R, R_RX_PAYLOAD, data, dataLen);	//read out received message
			CLEARBIT(PORTB, 1);		//ce low - stop sending/listening

			//Receiver function to print out on usart:
			R_buffer=WriteToNrf(R, R_RX_PAYLOAD, R_buffer, dataLen);	//read out received message
			reset();

			for (int i=0;i<dataLen;i++)
			{
				USART_Print_Int(R_buffer[i]);
			}
			USART_Transmit('R');
			USART_Transmit('\n');
		}
		// for (int i=0;i<dataLen;i++)	
		// {
		// 	USART_Transmit(data[i]);
		// 	USART_Transmit('\n');
		// }
		// USART_Transmit('R');
		// USART_Transmit('E');
		// USART_Transmit('\n');
	}




	// //check address of receiver
	// USART_Init(MYUBRR);
	// InitSPI();
	// nrf24L01_init();
	// while(1)
	// {
	// 	uint8_t val[5];

	// 	val[0] = 0x01;
	// 	WriteToNrf(W, EN_RXADDR, val, 1); //enable data pipe 0


	// 	//Write 5 bytes to the nRF
	// 	//RX_ADDR_P0 registry. 1-5 bytes - the receiver address in channel P0
	// 	int i;
	// 	for(i=0; i<5; i++)
	// 	{
	// 		val[i] = 0x12;
	// 	}
	// 	WriteToNrf(W, RX_ADDR_P0, val, 5);

	// 	//Read an array of bytes from the nRF
	// 	uint8_t* data;
	// 	data = WriteToNrf(R, RX_ADDR_P0, data, 5);
	// 	for (int i=0;i<dataLen;i++)	
	// 	{
	// 		USART_Print_Int(data[i]);
	// 	} 
	// 	_delay_ms(1000);
	// }

}

// void INT0_interrupt_init(void)	
// {
// 	DDRD &= ~(1<<DDD2);	//Extern interrupt på INT0, dvs sätt den till input!
	
// 	EICRA |=  (1<<ISC01);// INT0 falling edge	PD2
// 	EICRA  &=  ~(1<<ISC00);// INT0 falling edge	PD2

// 	EIMSK |=  (1<<INT0);	//enablar int0
//   	//sei();              // Enable global interrupts görs sen
// } 


// ISR(INT0_vect)	//vektorn som går igång när transmit_payload lyckats sända eller när receive_payload fått data OBS: då Mask_Max_rt är satt i config registret så går den inte igång när MAX_RT är uppnåd å sändninge nmisslyckats!
// {
// 	cli();	//Disable global interrupt
// 	CLEARBIT(PORTB, 1);		//ce low - stop sending/listening
	
// 	SETBIT(PORTB, 0); //led on to show success
// 	_delay_ms(500);
// 	CLEARBIT(PORTB, 0); //led off
	
// 	Receiver function to print out on usart:
// 	data=WriteToNrf(R, R_RX_PAYLOAD, data, dataLen);	//read out received message
// 	reset();

// 	for (int i=0;i<dataLen;i++)
// 	{
// 		USART_Transmit(data[i]);
// 	}
	
// 	sei();

// }