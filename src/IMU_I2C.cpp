#include "IMU_I2C.h"

// Write a single register over I2C
void IMU_I2C_writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(ADXL345_I2C_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

// Read a single register over I2C
uint8_t IMU_I2C_readRegister(uint8_t reg) {
    Wire.beginTransmission(ADXL345_I2C_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);

    Wire.requestFrom(ADXL345_I2C_ADDR, 1);
    return Wire.read();
}

// Initialize IMU via I2C
uint8_t IMU_I2C_init() {
    Wire.begin();
    delay(10);

    uint8_t devid = IMU_I2C_readRegister(IMU_REG_DEVID);
    if (devid != IMU_EXPECTED_DEVID) {
        return IMU_NOT_FOUND;
    }

    // 100 Hz normal power
    IMU_I2C_writeRegister(IMU_REG_BW_RATE, 0x0A);

    // Full resolution, ±4g
    IMU_I2C_writeRegister(IMU_REG_DATA_FORMAT, 0x09);

    // Measurement mode
    IMU_I2C_writeRegister(IMU_REG_POWER_CTL, 0x08);

    return IMU_OK;
}

// Read acceleration data
void IMU_I2C_readAccel(int16_t* x, int16_t* y, int16_t* z) {
    Wire.beginTransmission(ADXL345_I2C_ADDR);
    Wire.write(IMU_REG_DATAX0);
    Wire.endTransmission(false);

    Wire.requestFrom(ADXL345_I2C_ADDR, 6);

    uint8_t xl = Wire.read();
    uint8_t xh = Wire.read();
    uint8_t yl = Wire.read();
    uint8_t yh = Wire.read();
    uint8_t zl = Wire.read();
    uint8_t zh = Wire.read();

    *x = (int16_t)((xh << 8) | xl);
    *y = (int16_t)((yh << 8) | yl);
    *z = (int16_t)((zh << 8) | zl);
}

// Gyro not present on ADXL345
void IMU_I2C_readGyro(int16_t* x, int16_t* y, int16_t* z) {
    *x = 0;
    *y = 0;
    *z = 0;
}

void IMU_I2C_readAll(imu_vector_t* accel, imu_vector_t* gyro) {
    int16_t ax, ay, az;
    IMU_I2C_readAccel(&ax, &ay, &az);

    accel->x = ax;
    accel->y = ay;
    accel->z = az;

    // gyro is not available
    gyro->x = 0;
    gyro->y = 0;
    gyro->z = 0;
}
