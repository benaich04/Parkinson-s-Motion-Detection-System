#include <Arduino.h>
#include "IMU_I2C.h"
#include "Buffer.h"

#ifndef PREPROCESS_H
#define PREPROCESS_H

#define FILTER_SIZE 4

//THE FOLLOWING FUNCTION CONVERTS A WINDOW OF 3-AXIS ACCELEROMETER DATA
//INTO A 1D MAGNITUDE SIGNAL, MEAN-CENTERED AND MOVING AVERAGE FILTERED
//FOR FURTHER PROCESSING (E.G., FFT)
void preprocess_windowToMagnitude(const imu_sample_t* inWindow, float* outSignal, uint16_t length);


#endif // Preprocess.h