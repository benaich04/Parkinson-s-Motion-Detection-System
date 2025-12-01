#ifndef IMU_I2C_H
#define IMU_I2C_H

#include <Arduino.h>
#include <Wire.h>

// I2C address for ADXL345
#define ADXL345_I2C_ADDR 0x53

// IMU Registers
#define IMU_REG_DEVID        0x00
#define IMU_EXPECTED_DEVID   0xE5

#define IMU_REG_DATAX0       0x32 
#define IMU_REG_DATAX1       0x33
#define IMU_REG_DATAY0       0x34
#define IMU_REG_DATAY1       0x35
#define IMU_REG_DATAZ0       0x36
#define IMU_REG_DATAZ1       0x37

#define IMU_REG_BW_RATE      0x2C
#define IMU_REG_POWER_CTL    0x2D
#define IMU_REG_DATA_FORMAT  0x31

// Same data structure you used
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} imu_vector_t;

// Error codes
#define IMU_OK          0
#define IMU_ERROR       1
#define IMU_NOT_FOUND   2

// Function prototypes
uint8_t IMU_I2C_init();
uint8_t IMU_I2C_readRegister(uint8_t reg);
void IMU_I2C_writeRegister(uint8_t reg, uint8_t value);

void IMU_I2C_readAccel(int16_t* x, int16_t* y, int16_t* z);
void IMU_I2C_readGyro(int16_t* x, int16_t* y, int16_t* z);
void IMU_I2C_readAll(imu_vector_t* accel, imu_vector_t* gyro);

#endif
