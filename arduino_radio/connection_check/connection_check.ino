//https://forum.arduino.cc/t/nrf24l01-not-working-now-sending-failed/614759/11
#include <RF24.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <printf.h>

#define CE_PIN   9
#define CSN_PIN 10

const byte thisSlaveAddress[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN);

char dataReceived[10]; // this must match dataToSend in the TX
bool newData = false;


void setup() {

    Serial.begin(9600);
    printf_begin();

    Serial.println("CheckConnection Starting");
    Serial.println();
    Serial.println("FIRST WITH THE DEFAULT ADDRESSES after power on");
    Serial.println("  Note that RF24 does NOT reset when Arduino resets - only when power is removed");
    Serial.println("  If the numbers are mostly 0x00 or 0xff it means that the Arduino is not");
    Serial.println("     communicating with the nRF24");
    Serial.println();
    bool gay = radio.begin();
    radio.setPALevel(RF24_PA_MIN);
    radio.printDetails();
    Serial.println();
    Serial.println();
    Serial.println("AND NOW WITH ADDRESS AAAxR  0x41 41 41 78 52   ON P1");
    Serial.println(" and 250KBPS data rate");
    Serial.println();
    radio.openReadingPipe(1, thisSlaveAddress);
    radio.setDataRate( RF24_250KBPS );
    radio.printDetails();
    Serial.println();
    Serial.println(gay);
}


void loop() {

}
