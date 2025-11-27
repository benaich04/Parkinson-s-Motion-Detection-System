#include <Arduino.h>
#include "IMU.h"
#include "Buffer.h"

#ifndef PREPROCESS_H
#define PREPROCESS_H

#define FILTER_SIZE 4

void preprocess_windowToMagnitude(const imu_sample_t* inWindow, float* outSignal, uint16_t length);


#endif // Preprocess.h