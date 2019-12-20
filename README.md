# ADS131M04-Lib

Library for the ADS131M04 four way ADC, for use with the Arduino framework. Written by Daniele Valentino Bella, for Imperial College London Rocketry.

Click for the [ADS131M04 product page](https://www.ti.com/product/ADS131M04), and the [data sheet](https://www.ti.com/lit/gpn/ads131m04).

## Library Usage

### Reading Channel Data

#### Raw data from multiple channels

```
void rawChannels(int8_t * channelArrPtr, int8_t channelArrLen, int32_t * outputArrPtr);
```

This function sends an empty SPI communication frame, then reads the data from the next communication frame.

Inputs:

* ```int8_t * channelArrPtr```: Pointer to the first element of an array containing the channels desired

* ```int8_t channelArrLen```: Number of elements in the array containing the desired channels

* ```int32_t * outputArrPtr```: Pointer to the first element of an array where the raw data from the channels specified in the channel array will be stored. Note that the order of the channels will be in the order that was specified in the channel array.

#### Raw data from a single channel

```
int32_t rawChannelSingle(int8_t channel);
```

Returns the raw value read from a single channel ```channel```.

It is a wrapper function of the ```rawChannels``` method, thus if multiple channels need to be read, that method will be faster.
