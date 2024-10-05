// #ifndef SPI_H
// #define SPI_H



void SPI_MasterInit(void);

char SPI_MasterReceive(void);

void SPI_MasterTransmit(char cData);

void SPI_SlaveInit(void);

char SPI_SlaveReceive(void);

void SPI_SlaveTransmit(char cData);

// #endif