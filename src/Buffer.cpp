#include <Arduino.h>
#include "IMU_I2C.h"
#include "Buffer.h"


void imuBuffer_init(imu_buffer_t* buf){
    buf->writeIndex = 0;
    buf->count = 0;
    // Optionally clear the samples
    memset(buf->samples, 0, sizeof(buf->samples));
}

void imuBuffer_addSample(imu_buffer_t* buf, const imu_sample_t* sample){
    buf->samples[buf->writeIndex] = *sample;
    buf->writeIndex = (buf->writeIndex + 1) % BUFFER_SIZE;
    if (buf-> count < BUFFER_SIZE) {
        buf->count++;
    }
    else {
        // Buffer is full, overwrite oldest sample
    }
}

bool imuBuffer_isWindowReady(const imu_buffer_t* buf){
    if (buf-> count == BUFFER_SIZE){
        return true;
    }
    else {
        return false;
    }
}

void imuBuffer_getWindow(const imu_buffer_t* buf, imu_sample_t* outSamples) {

    // 1. Find the index of the oldest sample
    uint16_t startIndex = (buf->writeIndex + BUFFER_SIZE - buf->count) % BUFFER_SIZE;

    // 2. Copy samples in correct chronological order
    for (uint16_t i = 0; i < buf->count; i++) {
        uint16_t srcIndex = (startIndex + i) % BUFFER_SIZE;
        outSamples[i] = buf->samples[srcIndex];
    }
}
