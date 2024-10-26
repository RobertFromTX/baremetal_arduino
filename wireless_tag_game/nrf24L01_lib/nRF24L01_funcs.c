#include "nRF24L01.h"
#include "spi.h" 
//#include "./../spi_lib/spi.h"

#include "uart.h"

#include <avr/io.h> //not sure if this ruins it since already included in spi.h
#include <util/delay.h>

#include "nRF24L01_funcs.h"


#include <string.h>//for meme=set

//for uart
#define FOSC 16000000UL//1843200 // Clock Speed
#define BAUD 115200
#define MYUBRR ((FOSC/(16UL*BAUD))-1)



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

#define dataLen 32  //längd på datapacket som skickas/tas emot
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
	uint8_t buf[2];	
	buf[0] = reg;// register address (000AAAAA for read, 001AAAAA for write)
	if (ReadWrite == W) //if "W" then you want to write to the nRF (read mode "R" == 0x00, so skipping that one)	
	{
		//reg = W_REGISTER + reg;	//Add the "write" bit to the "reg", 0x20 = 1<<5 =32
		buf[0] |= (1<<5); //coommented out above line because what if there is already a 1 there
		
	}
	
	//Create an array to be returned at the end
	//Static uint8_t is needed to be able to return an array  notice the "*" to the left of the function: "WriteToNRf())
	static uint8_t ret[dataLen];	
	



	_delay_us(10);		//make sure last command was a while ago...
	CLEARBIT(PORTB, 2);	//CSN low = nRF starts to listen for command
	_delay_us(10);		
	WriteByteSPI(buf[0]);	//set the nRF to Write or read mode of "reg", sending register address
	_delay_us(10); 		
	
	int i;
	for(i=0; i<antVal; i++)
	{
		if (ReadWrite == R && reg != W_TX_PAYLOAD)	//Did you want to read a registry? FIXME does reg need to be changed to buf[0]? 
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
	CLEARBIT(PORTB, 1); //turn off CE to disable device - added


	uint8_t val[5]; //An array of integers to send to the *WriteToNrf function	
	val[0]=0x00;
	WriteToNrf(W, NRF_CONFIG, val, 1);//will be configured later, added

	//EN_AA - (auto-acknowledgements) - Transmitter gets automatic response from receiver when successful transmission! (lovely function!) 
	//Only works if Transmitter has identical RF_address on its channel ex: RX_ADDR_P0 = TX_ADDR
	val[0]=0x00;	// No auto ACK
	WriteToNrf(W, EN_AA, val, 1);	//W=write mode, EN_AA=register to write to, val=data to write, 1=number of data bytes.
	

	//Choose number of enabled data pipes (1-5)	
	val[0]=0x00; // Not enabling any data pipe right now
	WriteToNrf(W, EN_RXADDR, val, 1); //enable data pipe 0

	//RF_Address width setup (how many bytes is the receiver address, the more the merrier 1-5)	
	val[0]=0x03;	//0b0000 00011 = 5 bytes RF_Address
	WriteToNrf(W, SETUP_AW, val, 1); 

	//SETUP_RETR (the setup for "EN_AA")
	val[0]=0x00;	//0b0010 00011 "2" sets it up to 750uS delay between every retry (at least 500us at 250kbps and if payload >5bytes in 1Mbps, 
					//and if payload >15byte in 2Mbps) "F" is number of retries (1-15, now 15)
	WriteToNrf(W, SETUP_RETR, val, 1);
	

	//RF channel setup - choose frequency 2,400-2,527 GHz 1MHz/step
	val[0]=0x00;	//RD Channel registry 0b0000 0001 = 2,401 GHz (same on TX and RX)	
	WriteToNrf(W, RF_CH, val, 1); //Will setup during RX or TX 
	
	//RF setup	- choose power mode and data speed. here is the difference with the (+) version!!! 
	val[0]=0x0E;	//00000111 bit 3="0" 1Mbps=longer range, 2-1 power mode ("11" = -0dB ; "00"=-18dB) 	
	WriteToNrf(W, RF_SETUP, val, 1);

	SETBIT(PORTB, 1); //turn on CE to enable device - added

}

void nrf24_TxMode(uint8_t *Address, uint8_t channel)
{
	CLEARBIT(PORTB, 1); //turn off CE to disable device - added


	uint8_t val[5]; //An array of integers to send to the *WriteToNrf function	
	val[0]=channel;
	WriteToNrf(W, RF_CH, val, 1); //select channel


	WriteToNrf(W, TX_ADDR, Address, 5); //Write the TX address

	//power up device
	uint8_t config = GetReg(NRF_CONFIG);
	config = config | (1<<1); //PWR_UP = 1, don't change anything else
	val[0]=config;
	WriteToNrf(W, NRF_CONFIG, val, 1); //Write the TX address

	SETBIT(PORTB, 1);
}

void nrf24_RxMode(uint8_t *Address, uint8_t channel)
{
	CLEARBIT(PORTB, 1); //turn off CE to disable device - added


	uint8_t val[5]; //An array of integers to send to the *WriteToNrf function	
	val[0]=channel;
	WriteToNrf(W, RF_CH, val, 1); //select channel

	uint8_t en_rxaddr = GetReg(EN_RXADDR);
	en_rxaddr |= (1<<1); //select data pipe 1
	val[0]=en_rxaddr;
	WriteToNrf(W, EN_RXADDR, val, 1); //select data pipe 1

	WriteToNrf(W, RX_ADDR_P1, Address, 5); //Write the TX address

	val[0]=32;
	WriteToNrf(W, RX_PW_P1, val, 1);//32 bit payload size for pipe 1

	//power up device
	uint8_t config = GetReg(NRF_CONFIG);
	config = config | (1<<1) | (1<<0); //PWR_UP = 1, PRX selected
	val[0]=config;
	WriteToNrf(W, NRF_CONFIG, val, 1); //Write the TX address



	SETBIT(PORTB, 1);
}

uint8_t isDataAvailible (int pipenum)
{
	uint8_t status = GetReg(NRF_STATUS);
	if ((status&(1<<6)) && (status&(pipenum<<1))) //check RX FIFO interrupt and if data received in pipe
	{
		//FIXME, need status & ~(1<<6), but not working
		WriteToNrf(W, NRF_STATUS, ~(1 << 6), 1); //clear RX_DR interrupt bit by writing 0 to it, originally was writing 1 to it

		return 1; //1 for success
	}
	return 0;
}

uint8_t NRF24_Transmit(uint8_t *data)
{
	uint8_t val[5]; //An array of integers to send to the *WriteToNrf function	
	uint8_t cmdtosend = 0;
	

	CLEARBIT(PORTB, 2); //CSN to low to select device

	//payload command
	cmdtosend = W_TX_PAYLOAD;
	WriteByteSPI(cmdtosend);
	_delay_ms(100);

	//send the payload
	int i;
	for(i=0; i<32; i++)
	{
		{
			WriteByteSPI(*(data+i));	//Send the commands to the nRF once at a time	
			_delay_ms(10);
		}
	}

	SETBIT(PORTB, 2);

	_delay_ms(1);

	uint8_t fifostatus = GetReg(FIFO_STATUS);

	if ((fifostatus&(1<<4)) && (!(fifostatus&(1<<3))))
	{
		cmdtosend = FLUSH_TX;
		WriteByteSPI(cmdtosend);

		return 1;
	}
	return 0;
}


void NRF24_Receive(uint8_t *data)
{
	uint8_t val[5]; //An array of integers to send to the *WriteToNrf function	
	uint8_t cmdtosend = 0;
	

	CLEARBIT(PORTB, 2); //CSN to low to select device

	//payload command
	cmdtosend = R_RX_PAYLOAD;
	WriteByteSPI(cmdtosend);
	_delay_ms(100);

	//receive the payload
	int i;
	for(i=0; i<32; i++)
	{
		{
			*(data+i) = WriteByteSPI(RF24_NOP);	//Send the commands to the nRF once at a time	
			_delay_ms(10);
		}
	}

	SETBIT(PORTB, 2);

	_delay_ms(1);

	cmdtosend = FLUSH_RX;
	WriteByteSPI(cmdtosend);
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

//added from stm32 vid send ,command to nrf
void nrfsendCmd(uint8_t cmd)
{
	CLEARBIT(PORTB, 2);

	WriteByteSPI(cmd);

	_delay_ms(100);

	SETBIT(PORTB, 2);
}

//stm32 vid
int receiver(void)
{
	// //transmitter
	USART_Init(MYUBRR);
	InitSPI();
	nrf24L01_init();
	uint8_t RxAddress[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
	uint8_t RxData[32];
	nrf24_RxMode(RxAddress, 10);

	while(1)
	{
		memset(RxData, 0, sizeof(RxData));
		USART_Transmit('B');
		USART_Transmit('\n');
		if (isDataAvailible(1)==1)
		{
			NRF24_Receive(RxData);
			
			for (int i=0;i<dataLen;i++)	
			{
				
				USART_Transmit(RxData[i]);
				
			} 
			USART_Transmit('\n');
		}
		_delay_ms(1000);
	}
	
	unsigned char cmdtosend = FLUSH_RX;
	WriteByteSPI(cmdtosend);
}


//stm32 vid
int transmitter(void)
{
	// //transmitter
	USART_Init(MYUBRR);
	InitSPI();
	nrf24L01_init();
	uint8_t TxAddress[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
	uint8_t TxData[32];// = "HHHHHHHHHHHHHHHHHHHH\n";//"Hello World\n";
	for (int i = 0; i < 32; i++) {
        TxData[i] = 'R';
    }
	nrf24_TxMode(TxAddress, 10);

	while(1)
	{
		if(NRF24_Transmit(TxData) == 1)
		{
				USART_Transmit('T');
				USART_Transmit('\n');
				USART_Transmit('S');
				USART_Transmit('\n');
		}
		_delay_ms(1000);
	}
}



