#ifdef __cplusplus
extern "C"{
#endif 



void USART_Init(unsigned int ubrr);

unsigned char USART_Receive(void);

void USART_Transmit(unsigned char data);

void USART_Flush(void);

void USART_Print_Int(int data);

#ifdef __cplusplus
}
#endif