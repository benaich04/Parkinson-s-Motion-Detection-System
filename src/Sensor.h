#include <Arduino.h>  


#ifndef IMU_H
#define IMU_H


//Pin Definitions
#define Sensor_CS A5         // Chip Select pin for the IMU : PF0
#define Sensor_INT 3         // Interrupt pin for the IMU : INT0 : PD0

//IMU Registers
#define IMU_REG_DEVID      0x00      // Device ID register

//Data Registers
#define IMU_REG_DATAX0     0x32 
#define IMU_REG_DATAX1     0x33
#define IMU_REG_DATAY0     0x34
#define IMU_REG_DATAY1     0x35
#define IMU_REG_DATAZ0     0x36
#define IMU_REG_DATAZ1     0x37

//Control Registers
#define IMU_REG_BW_RATE        0x2C     // Data rate and power mode control
#define IMU_REG_POWER_CTL      0x2D
#define IMU_REG_DATA_FORMAT    0x31

//Interrupt control registers from the IMU
#define IMU_REG_INT_ENABLE   0x2E   // Enable interrupts
#define IMU_REG_INT_MAP      0x2F   // Map interrupts to INT1 or INT2 (some IMUs have 2 interrupt pins)
#define IMU_REG_INT_SOURCE   0x30   // Source of interrupts

//SPI Settings
#define IMU_SPI_CLOCK  2000000      // 2 MHz communication speed
#define IMU_SPI_MODE   SPI_MODE3    //SPI Mode 3 (CPOL=1, CPHA=1)
#define IMU_SPI_BITORDER MSBFIRST   //Bit Orders Setting

//IMU Settings
// DATA_FORMAT register (0x31):
// Bit D3 = FULL_RES (must be 1 → adds 0x08)
// Bits D1:D0 = RANGE selection:
//   00 = ±2g, 01 = ±4g, 10 = ±8g, 11 = ±16g
// Final value = 0x08 + RANGE bits
#define IMU_RANGE_2G   0x08
#define IMU_RANGE_4G   0x09
#define IMU_RANGE_8G   0x0A
#define IMU_RANGE_16G  0x0B


//Data structure to hold IMU data
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} imu_vector_t;

//Error Codes
#define IMU_OK          0
#define IMU_ERROR       1
#define IMU_NOT_FOUND   2

//Function Prototypes
uint8_t IMU_init();   // Returns IMU_OK, IMU_NOT_FOUND, etc.

void IMU_readAccel(int16_t* x, int16_t* y, int16_t* z);
void IMU_readGyro(int16_t* x, int16_t* y, int16_t* z);

// Optional combined read
void IMU_readAll(imu_vector_t* accel, imu_vector_t* gyro);



#endif