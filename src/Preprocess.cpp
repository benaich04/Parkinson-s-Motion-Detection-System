#include <Arduino.h>
#include "IMU_I2C.h"
#include "Buffer.h"
#include "Preprocess.h"

//THE FOLLOWING FUNCTION CONVERTS A WINDOW OF 3-AXIS ACCELEROMETER DATA
//INTO A 1D MAGNITUDE SIGNAL, MEAN-CENTERED AND MOVING AVERAGE FILTERED
//FOR FURTHER PROCESSING (E.G., FFT)

void preprocess_windowToMagnitude(const imu_sample_t* inWindow,
                                  float* outSignal,
                                  uint16_t length)
{
    float signal_sum = 0.0f;

    for (uint16_t i = 0; i < length; i++) {
        int16_t x = inWindow[i].x;
        int16_t y = inWindow[i].y;
        int16_t z = inWindow[i].z;

        // *** FIX: cast to float BEFORE squaring to avoid 16-bit overflow ***
        float fx = (float)x;
        float fy = (float)y;
        float fz = (float)z;

        float mag = sqrtf(fx * fx + fy * fy + fz * fz);
        outSignal[i] = mag;
        signal_sum += mag;
    }

    // Compute mean
    float signal_mean = signal_sum / (float)length;

    // Subtract mean
    for (uint16_t i = 0; i < length; i++) {
        outSignal[i] -= signal_mean;
    }

    // // moving average filter 
    // const uint8_t filterSize = FILTER_SIZE;
    // static float filteredSignal[BUFFER_SIZE];
    // for (uint16_t i = 0; i < length; i++) {
    //     float sum = 0.0f;
    //     uint8_t count = 0;
    //     for (int8_t j = -filterSize/2; j <= filterSize/2; j++) {
    //         int16_t idx = i + j;
    //         if (idx >= 0 && idx < length) {
    //             sum += outSignal[idx];
    //             count++;
    //         }
    //     }
    //     filteredSignal[i] = sum / (float)count;
    // }

    // // Copy filtered signal back to outSignal
    // for (uint16_t i = 0; i < length; i++) {
    //     outSignal[i] = filteredSignal[i];
    // }


    // moving average filter (in-place version)
const uint8_t filterSize = FILTER_SIZE;

for (uint16_t i = 0; i < length; i++) {
    float sum = 0.0f;
    uint8_t count = 0;

    for (int8_t j = -filterSize/2; j <= filterSize/2; j++) {
        int16_t idx = i + j;
        if (idx >= 0 && idx < length) {
            sum += outSignal[idx];
            count++;
        }
    }

    outSignal[i] = sum / (float)count;   // filter directly into outSignal
}

}
