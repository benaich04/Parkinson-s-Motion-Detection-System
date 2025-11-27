#include <arduinoFFT.h>
#include <Arduino.h>
#include "IMU.h"


#ifndef FFTMODULE_H
#define FFTMODULE_H


//must be a power of 2, since our signal will have 156 data points
//we can use 128 or 256, but 128 is faster
#define FFT_SIZE 128    //number of samples for FFT, must be a power of 2
#define SAMPLING_FREQUENCY 52 // in Hz

// Function to compute the FFT spectrum
void fft_computeSpectrum(const float* inSignal,
                         float* outSpectrum,
                         uint16_t length);


#endif // FFTMODULE_H