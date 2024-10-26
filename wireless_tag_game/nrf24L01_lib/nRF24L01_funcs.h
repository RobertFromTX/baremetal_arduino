#ifndef NRF24L01_LIB
#define NRF24L01_LIB



#ifdef __cplusplus
extern "C"{
#endif 
void InitSPI(void);
char WriteByteSPI(unsigned char cData);
uint8_t GetReg(uint8_t reg);
uint8_t *WriteToNrf(uint8_t ReadWrite, uint8_t reg, uint8_t *val, uint8_t antVal);
void nrf24L01_init(void);
void nrf24_TxMode(uint8_t *Address, uint8_t channel);
void nrf24_RxMode(uint8_t *Address, uint8_t channel);
uint8_t isDataAvailible (int pipenum);
uint8_t NRF24_Transmit(uint8_t *data);
void NRF24_Receive(uint8_t *data);
void transmit_payload(uint8_t * W_buff);
void receive_payload(void);
void reset(void);
void nrfsendCmd(uint8_t cmd);
int receiver(void);
int transmitter(void);

#ifdef __cplusplus
}
#endif


#endif