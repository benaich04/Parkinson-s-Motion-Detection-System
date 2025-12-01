#include <Arduino.h>
#include "IMU_I2C.h"   // so we can use imu_vector_t


#ifndef BUFFER_H
#define BUFFER_H



// Sampling settings
#define SAMPLE_RATE_HZ 45   // instead of 52
#define WINDOW_SECONDS 1    // instead of 3
#define BUFFER_SIZE      (SAMPLE_RATE_HZ * WINDOW_SECONDS)  // Total samples in one window


// One accelerometer sample (x, y, z) from the IMU
typedef imu_vector_t imu_sample_t;

// Circular buffer to store a rolling window of IMU samples
typedef struct {
    imu_sample_t samples[BUFFER_SIZE];  // stored accel samples (x,y,z)
    uint16_t writeIndex;                // where the next sample will be written
    uint16_t count;                     // how many valid samples are currently stored
} imu_buffer_t;


// Initialize the buffer (reset indices and counters)
void imuBuffer_init(imu_buffer_t* buf);

// Add a new accelerometer sample to the buffer
void imuBuffer_addSample(imu_buffer_t* buf, const imu_sample_t* sample);

// Check if we have a full WINDOW (3 seconds) of data ready
bool imuBuffer_isWindowReady(const imu_buffer_t* buf);

// Copy the current window of samples into a destination array for processing (FFT, etc.)
void imuBuffer_getWindow(const imu_buffer_t* buf, imu_sample_t* outSamples);




#endif



