#include <SPI.h>
#include <Arduino.h>
#include "Sensor.h"



//Initialize the IMU sensor
uint8_t IMU_init() {
    SPI.begin();
    //set CS as output
    DDRF |= (1 << DDF0); // PF0 as output
    //set CS high
    PORTF |= (1 << PORTF0); // PF0 high
    //start SPI transaction
    SPI.beginTransaction(SPISettings(IMU_SPI_CLOCK, IMU_SPI_BITORDER,IMU_SPI_MODE));
    delay(10); //wait for sensor to stabilize
    //set CS low to select the sensor
    PORTF &= ~(1 << PORTF0); // PF0 low
    //send the DEVID register address
    SPI.transfer(0x80 | IMU_REG_DEVID); //read command (bit 7 high)
    uint8_t receivedVal = SPI.transfer(0x00); //send dummy byte to receive data
    //set CS high to deselect the sensor
    PORTF |= (1 << PORTF0); // PF0 high
    //check if the received value matches the expected device ID
    if (receivedVal != 0xE5) {
        SPI.endTransaction();
        return IMU_NOT_FOUND; //IMU not found
    }
    SPI.endTransaction();
    return IMU_OK; //IMU initialized successfully
}