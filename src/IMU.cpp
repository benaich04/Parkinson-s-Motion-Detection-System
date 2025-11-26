#include <SPI.h>
#include <Arduino.h>
#include "IMU.h"



//Initialize the IMU sensor
uint8_t IMU_init() {
    SPI.begin();

    // CS pin as output, deselect IMU
    DDRF  |= (1 << DDF0);   // PF0 output
    PORTF |= (1 << PORTF0); // PF0 high

    delay(10); // wait for sensor to stabilize

    // --- Check device ID using helper ---
    uint8_t receivedVal = IMU_readRegister(IMU_REG_DEVID);
    if (receivedVal != IMU_EXPECTED_DEVID) {
        return IMU_NOT_FOUND; // IMU not found
    }
    
    //set the baud rate and measurement mode : 
    //baud rate 100Hz -> (BW_Rate) xxxx1010
    //Low power mode  -> (BW_Rate) xxx0xxxx 
    IMU_writeRegister(IMU_REG_BW_RATE, 0x0A); //set to 100Hz, normal power
    
    //set the data format to 4g resolution, +/-4g
    //SPI bit for 4 wire SPI : (DATA_Format) x0xxxxxx 
    //Full resolution ON :     (DATA_Format) xxxx1xxx
    //Range +/-4g :            (DATA_Format) xxxxxx01
    IMU_writeRegister(IMU_REG_DATA_FORMAT, 0x09); //set to +/-4g, full resolution, 4-wire SPI

    //Set the power control settings
    //Measurement mode ON :   (POWER_CTL) xxxx1xxx   
    //Normal mode (no sleep) :(POWER_CTL) xxxxx0xx   
    //No autosleep / link :   (POWER_CTL) xxx00xxx   
    //Wakeup bits ignored :   (POWER_CTL) xxxxxx00 
    IMU_writeRegister(IMU_REG_POWER_CTL, 0x08); //set to measurement mode, normal operation  

    return IMU_OK; // IMU initialized successfully
}

uint8_t IMU_readRegister(uint8_t reg) {
    SPI.beginTransaction(SPISettings(IMU_SPI_CLOCK, IMU_SPI_BITORDER,IMU_SPI_MODE));
    //set CS low to select the sensor
    PORTF &= ~(1 << PORTF0); // PF0 low
    //send the register address with read command
    SPI.transfer(0x80 | reg); //read command (bit 7 high)
    uint8_t receivedVal = SPI.transfer(0x00); //send dummy byte to receive data
    PORTF |= (1 << PORTF0); //set CS high to deselect the sensor
    SPI.endTransaction();
    return receivedVal;
}


void IMU_writeRegister(uint8_t reg, uint8_t value) {
    SPI.beginTransaction(SPISettings(IMU_SPI_CLOCK, IMU_SPI_BITORDER,IMU_SPI_MODE));
    //set CS low to select the sensor
    PORTF &= ~(1 << PORTF0); // PF0 low
    SPI.transfer(reg); // Send register address (WRITE → bit7 = 0)
    SPI.transfer(value); // Send value to write into that register
    PORTF |= (1 << PORTF0); //set CS high to deselect the sensor
    SPI.endTransaction();
}


void IMU_readAccel(int16_t* x, int16_t* y, int16_t* z){
    uint8_t xl = IMU_readRegister(IMU_REG_DATAX0);
    uint8_t xh = IMU_readRegister(IMU_REG_DATAX1);
    uint8_t yl = IMU_readRegister(IMU_REG_DATAY0);
    uint8_t yh = IMU_readRegister(IMU_REG_DATAY1);
    uint8_t zl = IMU_readRegister(IMU_REG_DATAZ0);
    uint8_t zh = IMU_readRegister(IMU_REG_DATAZ1);

    *x = (int16_t)((xh << 8) | xl);
    *y = (int16_t)((yh << 8) | yl);
    *z = (int16_t)((zh << 8) | zl);
}

// Placeholder function for reading gyroscope data : ADXl345 does not have a gyroscope
void IMU_readGyro(int16_t* x, int16_t* y, int16_t* z) {
    *x = 0;
    *y = 0;
    *z = 0;
}

void IMU_readAll(imu_vector_t* accel, imu_vector_t* gyro){
    int16_t ax, ay, az;

    // Read accel
    IMU_readAccel(&ax, &ay, &az);

    accel->x = ax;
    accel->y = ay;
    accel->z = az;

    // Gyro not available on ADXL345
    gyro->x = 0;
    gyro->y = 0;
    gyro->z = 0;
}