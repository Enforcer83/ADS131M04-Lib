/* This is a library for the ADS131M04 4-channel ADC
   
   Product information:
   https://www.ti.com/product/ADS131M04

   Datasheet: 
   https://www.ti.com/lit/gpn/ads131m04

   This library was made for Imperial College London Rocketry
   Created by Daniele Valentino Bella
*/

#include <Arduino.h>
#include <SPI.h>
#include "registerDefinitions.h"
#include "ADS131M04.h"

ADS131M04::ADS131M04(int8_t _csPin, int8_t _clkoutPin, SPIClass* _spi, int8_t _clockCh) {
  csPin = _csPin;
  clkoutPin = _clkoutPin;
  spi = _spi;
  clockCh = _clockCh;
  initialised = false;
}

void ADS131M04::begin(void) {
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);

  spi->begin();

  // Set CLKOUT on the ESP32 to give the correct frequency for CLKIN on the DAC
  ledcAttachPin(clkoutPin, clockCh);
  ledcSetup(clockCh, CLKIN_SPD, 8);
  ledcWrite(clockCh, 127);

  initialised=true;
}

void ADS131M04::rawChannels(int8_t * channelArrPtr, int8_t channelArrLen, int32_t * outputArrPtr) {
  /* Writes data from the channels specified in channelArr, to outputArr,
     in the correct order.

     channelArr should have values from 0-3, and channelArrLen should be the
     length of that array, starting from 1.
  */
  
  int32_t rawDataArr[5];

  // Send initial (blank) set of data
  spiCommFrame(&rawDataArr[0]);
  
  // Get data from response
  spiCommFrame(&rawDataArr[0]);
  
  for (int8_t i = 0; i<channelArrLen; i++) {
    *outputArrPtr = rawDataArr[*channelArrPtr];
    outputArrPtr++;
    channelArrPtr++;
  }
}

int32_t ADS131M04::rawChannelSingle(int8_t channel) {
  /* Returns raw value from a single channel
     channel input from 0-3
  */
  
  int32_t outputArr[1];
  int8_t channelArr[1] = {channel};

  rawChannels(&channelArr[0], 1, &outputArr[0]);

  return outputArr[0];
}

uint32_t ADS131M04::spiTransferWord(uint16_t inputData) {
  // Transfer a 24 bit word

  uint32_t data = spi->transfer(inputData>>8);
  data <<= 8;
  data |= spi->transfer(inputData);
  data <<= 8;
  data |= spi->transfer(0x00);

  return data;
}

void ADS131M04::spiCommFrame(int32_t * outPtr, uint16_t command) {
  // Saves all the data of a communication frame to an array with pointer outPtr

  digitalWrite(csPin, LOW);

  spi->beginTransaction(SPISettings(SCLK_SPD, MSBFIRST, SPI_MODE0));

  // Send the command in the first word
  *outPtr = spiTransferWord(command);

  // For the next 4 words, just read the data
  for (uint8_t i=1; i < 5; i++) {
    outPtr++;
    *outPtr = twoCompDeco(spiTransferWord());
  }

  // Save CRC bits
  outPtr++;
  *outPtr = spiTransferWord();

  spi->endTransaction();

  digitalWrite(csPin, HIGH);
}

int32_t ADS131M04::twoCompDeco(uint32_t data) {
  // Take the two's complement of the data

  data <<= 8;
  int32_t dataInt = (int)data;

  return dataInt/pow(2,8);
}